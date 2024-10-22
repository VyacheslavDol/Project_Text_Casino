#include "auth_service.h"

void send_type(SSL* ssl) {
  CHECK_ERROR(SSL_write(ssl, &send_msg, sizeof(struct MainMessage)), -1);
  bzero(&send_msg, sizeof(struct MainMessage));
  int type = 0;
  CHECK_ERROR(SSL_read(ssl, &type, sizeof(type)), -1);
}

void login_service(SSL* ssl, struct Login* login, struct AuthResponse* res) {
  send_msg.type = LOGIN_TYPE;
  send_type(ssl);

  CHECK_ERROR(SSL_write(ssl, login, sizeof(*login)), -1);
  CHECK_ERROR(SSL_read(ssl, res, sizeof(*res)), -1);
}

void register_service(SSL* ssl, struct Register* reg,
                      struct AuthResponse* res) {
  send_msg.type = REGISTR_TYPE;

  send_type(ssl);

  CHECK_ERROR(SSL_write(ssl, reg, sizeof(*reg)), -1);
  CHECK_ERROR(SSL_read(ssl, res, sizeof(*res)), -1);
}

void* logout_service(void* data) {
  printf("\033[H\033[J");
  (void)data;
  struct MainMessage main_msg = {0};
  main_msg.type = LOGOUT_TYPE;
  Interceptor_SSL_write(ssl, &main_msg);
  remove(FILE_NAME);
  ssl_close();
  restore_terminal_settings();
  close(sock);
  exit(EXIT_SUCCESS);
}