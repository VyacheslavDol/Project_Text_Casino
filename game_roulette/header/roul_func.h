#ifndef ROUL_FUNC_H_
#define ROUL_FUNC_H_

#include "../header/roul_struct.h"

int get_num_player_roul(place_roul_t *place);

void send_msg_room_roul(int socket, const room_roul_t *room, int place_id,
                        int code_msg);

void send_msg_place_roul(int socket, const place_roul_t *place, int place_id,
                         int room_id, int code_msg);

void send_msg_info_client(int socket, int choice);

short x_btwn_a_b_or_c_d(short x, short a, short b, short c, short d);

short get_colour(short x);

void print_table();

#endif  // ROUL_FUNC_H_