#include "ssl.h"

void ssl_close() {
  if (ssl) {
    SSL_shutdown(ssl);
    SSL_free(ssl);
  }
  if (sock) {
    close(sock);
  }
}

void Interceptor_SSL_write(SSL* ssl, struct MainMessage* msg) {
  memcpy(msg->access_token, access_token, (TOKEN_SIZE));

  struct MainMessage cp_msg = *msg;

  int status = 0;
  int check_req = 0;

  CHECK_ERROR(SSL_write(ssl, msg, sizeof(struct MainMessage)), -1);

  do {
    CHECK_ERROR(SSL_read(ssl, &status, sizeof(status)), -1);

    if (status) {
      struct AuthResponse res;
      if ((refresh(ssl, &res) && check_token_err(res.error)) ||
          check_req == MAX_NUM_REQUEST) {
        remove(FILE_NAME);
        error(res.error);
        error("Unauthorized!");
        ssl_close();
        restore_terminal_settings();
        exit(EXIT_SUCCESS);
      } else {
        memcpy(access_token, res.tokens.access_token, (TOKEN_SIZE));
        write_token_file(res.tokens.refresh_token);
      }

      memcpy(cp_msg.access_token, access_token, (TOKEN_SIZE));
      CHECK_ERROR(SSL_write(ssl, &cp_msg, sizeof(struct MainMessage)), -1);
      ++check_req;
    }
  } while (status);
}
