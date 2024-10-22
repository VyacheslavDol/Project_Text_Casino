#ifndef ROUL_BASIC_SERVER_H_
#define ROUL_BASIC_SERVER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>

#include "../error/error.h"
#include "../service/profile_service.h"
#include "../struct/auth_struct.h"
#include "../struct/main_message_strcut.h"
#include "../struct/roul_struct.h"
#include "roul_func.h"

void loop_server_roulette(room_roul_t *room, msg_place_roul_t *msg_rcv,
                          SSL *socket, const struct User *user);

void spin_roulette(short *value, short *colour);

void init_player_roul(player_roul_t *player, SSL *socket,
                      const struct User *user);

void send_room_roul(room_roul_t *room, SSL *socket, int flag);

void init_room_roul(room_roul_t *room, int id);

void room_balance_roul(place_roul_t *place);

void get_result(room_roul_t *room);

void start_room_roul(room_roul_t *room);

void refresh_room(room_roul_t *room);

#endif  // ROUL_BASIC_CLIENT_H_