#ifndef ROUL_BASIC_CLIENT_H_
#define ROUL_BASIC_CLIENT_H_

#include <errno.h>
#include <openssl/ssl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "../struct/main_message_strcut.h"
#include "../struct/roul_struct.h"
#include "input_char.h"
#include "roul_func.h"
#include "roul_interface.h"

extern struct termios oldattr;

extern volatile int exit_flag;

int loop_client_roulette(msg_room_roul_t* msg_rcv, place_roul_t* place,
                         SSL* socket);

int choice_room_roul(msg_info_room_roul_t* msg_rcv);

void get_bet_roul(place_roul_t* place);

#endif  // ROUL_BASIC_CLIENT_H_