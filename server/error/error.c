#include "error.h"

char* color(const char* err) {
  static char new_err[1024];
  snprintf(new_err, sizeof(new_err), "\033[1;31m%s\033[0m\n", err);
  return new_err;
}

void error(const char* err) {
  printf("%s", color(err));
  exit(EXIT_FAILURE);
}

void error_func(const char* func, const char* err) {
  char new_err[256];
  snprintf(new_err, sizeof(new_err), "[ %s(?) -> %s ]:\nERROR:\t%s", func, err,
           strerror(errno));
  fprintf(stderr, "%s", color(new_err));
  exit(EXIT_FAILURE);
}

void handle_ssl_error(SSL* ssl, int ssl_error) {
  switch (ssl_error) {
    case SSL_ERROR_ZERO_RETURN:
    case SSL_ERROR_SYSCALL:
      printf("Client disconnected or socket error occurred.\n");
      break;
    default:
      printf("SSL error occurred: %d\n", ssl_error);
      break;
  }

  if (ssl) {
    SSL_shutdown(ssl);
    SSL_free(ssl);
  }
}