#include "mail_service.h"

struct upload_status {
  const char *payload_text;
  size_t bytes_read;
};

static size_t payload_source(char *ptr, size_t size, size_t nmemb,
                             void *userp) {
  struct upload_status *upload = (struct upload_status *)userp;
  const char *data;

  if ((size == 0) || (nmemb == 0) || ((size * nmemb) < 1)) {
    return 0;
  }

  size_t room = size * nmemb;
  data = upload->payload_text + upload->bytes_read;

  if (data) {
    size_t len = strlen(data);
    if (room < len) len = room;
    memcpy(ptr, data, len);
    upload->bytes_read += len;

    return len;
  }

  return 0;
}

int send_confirmation_email(const char *recipient_mail, const char *uuid) {
  CURL *curl;
  CURLcode res;
  struct curl_slist *recipients = NULL;
  const char *mail_username = MAIL_USERNAME;

  char payload_text[PAYLOAD_SIZE];
  snprintf(payload_text, PAYLOAD_SIZE,
           "To: %s\r\n"
           "From: %s\r\n"
           "Subject: CASINO\r\n"
           "\r\n"
           "Активация аккаунта для CASINO.\r\n"
           "\r\n"
           "Для активации аккаунта перейдите по "
           "ссылке:\nhttps://localhost:8888/confirm?uuid=%s\n\r",
           recipient_mail, mail_username, uuid);

  struct upload_status upload = {payload_text, 0};

  curl = curl_easy_init();

  if (!curl) {
    fprintf(stderr, "Failed to initialize curl\n");
    return EXIT_FAILURE;
  }

  curl_easy_setopt(curl, CURLOPT_URL, MAIL_SMTP_SERVER);
  curl_easy_setopt(curl, CURLOPT_USERNAME, mail_username);
  curl_easy_setopt(curl, CURLOPT_PASSWORD, MAIL_PASSWORD);
  curl_easy_setopt(curl, CURLOPT_USE_SSL, (long)CURLUSESSL_ALL);
  curl_easy_setopt(curl, CURLOPT_MAIL_FROM, mail_username);

  recipients = curl_slist_append(recipients, recipient_mail);
  curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);

  curl_easy_setopt(curl, CURLOPT_READFUNCTION, payload_source);
  curl_easy_setopt(curl, CURLOPT_READDATA, &upload);
  curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

  res = curl_easy_perform(curl);
  if (res != CURLE_OK) {
    fprintf(stderr, "curl_easy_perform() failed: %s\n",
            curl_easy_strerror(res));
    return EXIT_FAILURE;
  } else {
    printf("Email sent successfully\n");
  }

  curl_slist_free_all(recipients);
  curl_easy_cleanup(curl);

  return EXIT_SUCCESS;
}

void gen_uuid_mail(char *uuid_mail) {
  uuid_t uuid;
  uuid_generate(uuid);
  uuid_unparse(uuid, uuid_mail);
  printf("Generated UUID: %s\n", uuid_mail);
}

void db_add_link(const char *uuid_user, const char *uuid_mail) {
  PGconn *conn = NULL;
  db_connetc(&conn);
  db_add(&conn, MAIL, 2, uuid_user, uuid_mail);
  PQfinish(conn);
}

int acc_conf(const char *uuid) {
  PGconn *conn = NULL;
  db_connetc(&conn);
  int status =
      db_update(&conn, MAIL, uuid, 5, "is_active", "TRUE", "link", "", "link");

  PQfinish(conn);
  return status;
}