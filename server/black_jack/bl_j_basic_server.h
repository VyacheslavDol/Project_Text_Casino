#ifndef BL_J_BASIC_SERVER_H_
#define BL_J_BASIC_SERVER_H_

#include <openssl/ssl.h>

#include "../service/profile_service.h"

void loop_server_black_jack(room_t *room, msg_place *msg_rcv, SSL *socket,
                            const struct User *user);

void black_jack_result(room_t *room, int i);

void start_bet_reply(room_t *room);

void select_1_reply(room_t *room);

void more_reply(room_t *room, int place_id);

void enough_reply(room_t *room, int place_id);

void double_reply(room_t *room, int place_id);

void diler_game(room_t *room);

void end_round(room_t *room);

void room_balance(place_t place[]);

void start_room(room_t *room);

void send_room(room_t *room, SSL *socket, int flag);

#endif  // BL_J_BASIC_SERVER_H_