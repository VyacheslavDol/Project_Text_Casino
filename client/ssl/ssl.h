#ifndef SSL_H
#define SSL_H

#include <openssl/ssl.h>

#include "../components/termio/termio.h"
#include "../service/token_service.h"
#include "../struct/main_message_strcut.h"

extern SSL* ssl;
extern int sock;
extern char access_token[TOKEN_SIZE];

void ssl_close();
void Interceptor_SSL_write(SSL* ssl, struct MainMessage* msg);

#endif