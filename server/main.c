#include "./server/http_server.h"
#include "./server/tcp_server.h"
#include "./ssl/ssl.h"

void env_start() {
  if (env_load("", 0) != 0) {
    fprintf(stderr, "Error loading .env file\n");
  }
}

int main() {
  printf("\033[H\033[J");
  env_start();
  SSL_CTX* ctx;
  ssl_init(&ctx);

  PGconn* conn = NULL;
  db_connetc(&conn);
  PQfinish(conn);

  http_server();
  tcp_server(ctx);
}