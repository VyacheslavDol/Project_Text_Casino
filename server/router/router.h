#ifndef ROUTER
#define ROUTER

#include <openssl/ssl.h>
#include <sys/socket.h>
#include <unistd.h>

#include "../black_jack/bl_j_basic_server.h"
#include "../controller/auth_controller.h"
#include "../error/error.h"
#include "../roulette/roul_basic_server.h"
#include "../service/profile_service.h"
#include "../struct/auth_struct.h"
#include "../struct/main_message_strcut.h"
#include "../struct/roul_struct.h"
#include "../struct/token_struct.h"

#define MAX_CLIENTS 25

#define REFRESH 1
#define REGISTRATION 2
#define LOGIN 3
#define BLASK_JACK 10
#define ROUL 11
#define MENU 0
#define SLOT_MACHINE 12
#define GET_BALANCE 14
#define LOGOUT 15
#define CHAT 4

extern SSL *ssl_clients[MAX_CLIENTS];

void refresh_router(SSL *ssl);
void login_router(SSL *ssl);
void registration_router(SSL *ssl);
void mail_conf_router(SSL *ssl, char *uuid);
void black_jack(SSL *ssl, struct MainMessage *main_msg, room_t *room,
                const struct TokenPayload *payload);
void roulette(SSL *ssl, struct MainMessage *main_msg, room_roul_t *room_roul,
              const struct TokenPayload *payload);
void slot_machine(SSL *ssl, const struct TokenPayload *payload);
void get_balance_router(SSL *ssl, const struct TokenPayload *payload);
void chat(const SSL *ssl, struct MainMessage *reg);
void router(SSL *ssl, struct MainMessage *main_msg, room_t *room,
            room_roul_t *room_roul, const struct TokenPayload *payload);

#endif