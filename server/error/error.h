#ifndef ERROR
#define ERROR

#include <errno.h>
#include <openssl/err.h>
#include <openssl/ssl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define ERR_SIZE 256

void error(const char* err);
void error_func(const char* func, const char* err);
void handle_ssl_error(SSL* ssl, int ssl_error);

#define CHECK_ERROR(func, res)   \
  if ((func) == res) {           \
    error_func(__func__, #func); \
  }

#define CHECK_SSL_WRITE(ssl, msg, len)                   \
  do {                                                   \
    int bytes_written = SSL_write(ssl, msg, len);        \
    if (bytes_written <= 0) {                            \
      int ssl_error = SSL_get_error(ssl, bytes_written); \
      handle_ssl_error(ssl, ssl_error);                  \
      return;                                            \
    }                                                    \
  } while (0)

#define CHECK_SSL_READ(ssl, msg, len)                    \
  do {                                                   \
    int bytes_written = SSL_read(ssl, msg, len);         \
    if (bytes_written <= 0) {                            \
      int ssl_error = SSL_get_error(ssl, bytes_written); \
      handle_ssl_error(ssl, ssl_error);                  \
      return;                                            \
    }                                                    \
  } while (0)

#endif