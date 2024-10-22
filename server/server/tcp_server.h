#ifndef TCP_SERVER
#define TCP_SERVER

#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>

#include "../black_jack/bl_j_func.h"
#include "../lib/libdotenv/dot_env.h"
#include "../roulette/roul_func.h"
#include "../router/router.h"
#include "../ssl/ssl.h"
#include "../struct/main_message_strcut.h"
#include "../struct/roul_struct.h"

#define TCP_PORT 8080
#define BUFFER_SIZE 1024
#define USER_IS_AUTH 9

int tcp_server(SSL_CTX* ctx);

#endif