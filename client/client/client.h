#ifndef CLIENT

#include <arpa/inet.h>
#include <openssl/err.h>
#include <openssl/ssl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PORT 8080
#define BUFFER_SIZE 1024

#include "../black_jack/bl_j_interface.h"
#include "../components/auth_component.h"
#include "../components/termio/termio.h"
#include "../error/error.h"
#include "../ssl/ssl.h"
#include "../struct/main_message_strcut.h"

void client();

#endif