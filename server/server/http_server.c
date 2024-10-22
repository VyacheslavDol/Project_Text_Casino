#include "http_server.h"

char *read_file(const char *filename) {
  FILE *file = fopen(filename, "rb");
  if (!file) {
    perror("Failed to open file");
    return NULL;
  }

  fseek(file, 0, SEEK_END);
  long size = ftell(file);
  fseek(file, 0, SEEK_SET);

  char *buffer = malloc(size + 1);
  if (!buffer) {
    perror("Failed to allocate memory");
    fclose(file);
    return NULL;
  }

  long res = (long)fread(buffer, 1, size, file);
  if (res != size) {
    perror("fread");
    free(buffer);
    fclose(file);
    return NULL;
  }
  buffer[size] = '\0';
  fclose(file);

  return buffer;
}

enum MHD_Result handle_request(void *cls, struct MHD_Connection *connection,
                               const char *url, const char *method,
                               const char *version, const char *upload_data,
                               size_t *upload_data_size, void **con_cls) {
  (void)cls;
  (void)url;
  (void)method;
  (void)version;
  (void)upload_data;
  (void)upload_data_size;
  (void)con_cls;

  const char *uuid_param = (const char *)MHD_lookup_connection_value(
      connection, MHD_GET_ARGUMENT_KIND, "uuid");

  if (uuid_param && !acc_conf(uuid_param)) {
    printf("UUID received: %s\n", uuid_param);
    const char *response_text = "Account confirmed successfully!";

    struct MHD_Response *response = MHD_create_response_from_buffer(
        strlen(response_text), (void *)response_text, MHD_RESPMEM_PERSISTENT);
    enum MHD_Result ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
    MHD_destroy_response(response);

    return ret;
  } else {
    const char *response_text = "UUID not found!";

    struct MHD_Response *response = MHD_create_response_from_buffer(
        strlen(response_text), (void *)response_text, MHD_RESPMEM_PERSISTENT);
    enum MHD_Result ret =
        MHD_queue_response(connection, MHD_HTTP_BAD_REQUEST, response);
    MHD_destroy_response(response);
    return ret;
  }
}

void http_server() {
  const struct MHD_Daemon *server;
  char *cert = read_file(getenv("SERVER_CERT_PEM"));
  char *key = read_file(getenv("SERVER_KEY_PEM"));

  server =
      MHD_start_daemon(MHD_USE_SELECT_INTERNALLY | MHD_USE_SSL, HTTP_PORT, NULL,
                       NULL, &handle_request, NULL, MHD_OPTION_HTTPS_MEM_KEY,
                       key, MHD_OPTION_HTTPS_MEM_CERT, cert, MHD_OPTION_END);

  if (server == NULL) {
    fprintf(stderr, "FAILED start server\n");
    free(cert);
    free(key);
    exit(EXIT_FAILURE);
  }

  printf("HTTP server is running on port: %d\n", HTTP_PORT);
}