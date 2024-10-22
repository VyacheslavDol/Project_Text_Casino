#ifndef ROUL_BASIC_SERVER_H_
#define ROUL_BASIC_SERVER_H_

#include "../header/roul_struct.h"

void loop_server_roulette(room_roul_t *room, msg_place_roul_t *msg_rcv, int socket);

void spin_roulette(short *value, short *colour);

void init_player(player_roul_t* player, int socket);

void send_room_roul(room_roul_t *room, int socket, int flag);

void init_room(room_roul_t *room, int id);

void room_balance_roul(place_roul_t *place);

void get_result(room_roul_t* room);

void start_room_roul(room_roul_t *room);

void refresh_room(room_roul_t *room);

#endif // ROUL_BASIC_CLIENT_H_