#include "client.h"

SSL *ssl;
int sock;

struct MainMessage send_msg = {0};
char access_token[TOKEN_SIZE] = {0};

int balance = 0;

struct termios oldattr;

void cleanup(int signum) {
  printf("\nCaught signal %d, cleaning up...\n", signum);
  printf("\033[H\033[J");
  ssl_close();
  menu_free(menu_tree);
  restore_terminal_settings();
  printf("\033[?25h");
  exit(EXIT_SUCCESS);
}

void client() {
  struct sockaddr_in serv_addr;

  SSL_library_init();
  SSL_load_error_strings();
  OpenSSL_add_ssl_algorithms();

  SSL_CTX *ctx = SSL_CTX_new(SSLv23_client_method());
  ssl = SSL_new(ctx);

  CHECK_ERROR((sock = socket(AF_INET, SOCK_STREAM, 0)), -1);

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(PORT);

  CHECK_ERROR(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr), -1);

  CHECK_ERROR(connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)),
              -1);

  if (SSL_set_fd(ssl, sock) != 1) {
    ERR_print_errors_fp(stderr);
  }

  signal(SIGINT, cleanup);

  if (SSL_connect(ssl) <= 0) {
    ERR_print_errors_fp(stderr);
  } else {
    struct AuthResponse res;

    while (auth_main(ssl, &res))
      ;

    SSL_shutdown(ssl);
    SSL_free(ssl);
    close(sock);
  }
}