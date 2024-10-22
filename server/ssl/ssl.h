#ifndef SSL_h
#define SSL_H

#include <openssl/err.h>
#include <openssl/ssl.h>

#include "../error/error.h"

void ssl_init(SSL_CTX **ctx);

#endif