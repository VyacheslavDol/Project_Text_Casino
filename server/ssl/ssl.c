#include "ssl.h"

void ssl_init(SSL_CTX **ctx) {
  SSL_library_init();
  SSL_load_error_strings();
  OpenSSL_add_ssl_algorithms();

  const SSL_METHOD *method;

  method = SSLv23_server_method();
  *ctx = SSL_CTX_new(method);

  if (!*ctx) {
    ERR_print_errors_fp(stderr);
    exit(EXIT_FAILURE);
  }

  if (SSL_CTX_use_certificate_file(*ctx, getenv("SERVER_CERT_PEM"),
                                   SSL_FILETYPE_PEM) <= 0 ||
      SSL_CTX_use_PrivateKey_file(*ctx, getenv("SERVER_KEY_PEM"),
                                  SSL_FILETYPE_PEM) <= 0) {
    ERR_print_errors_fp(stderr);
    error("NO SSL CERTIFICATES FOUND !!!");
  }
}