#ifndef AUTH_SERVICE
#define AUTH_SERVICE

#include <openssl/ssl.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>

#include "../black_jack/bl_j_interface.h"
#include "../components/termio/termio.h"
#include "../error/error.h"
#include "../ssl/ssl.h"
#include "../struct/auth_struct.h"
#include "../struct/main_message_strcut.h"
#include "token_service.h"

extern SSL* ssl;
extern int sock;
extern struct termios oldattr;
extern struct MainMessage send_msg;

void send_type(SSL* ssl);
void register_service(SSL* ssl, struct Register* reg, struct AuthResponse* res);

void login_service(SSL* ssl, struct Login* login, struct AuthResponse* res);
void* logout_service(void* data);

#endif