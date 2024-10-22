#ifndef AUTH_COMPONENTS
#define AUTH_COMPONENTS

#include <openssl/ssl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "../components/base/base.h"
#include "../error/error.h"
#include "../service/auth_service.h"
#include "../service/token_service.h"
#include "../struct/auth_struct.h"
#include "game_component.h"
#include "termio/termio.h"

extern char access_token[TOKEN_SIZE];

void auth_login(SSL* ssl, struct AuthResponse* res);
void auth_register(SSL* ssl, struct AuthResponse* res);
bool auth_main(SSL* ssl, struct AuthResponse* res);

#endif