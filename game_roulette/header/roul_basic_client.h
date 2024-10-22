#ifndef ROUL_BASIC_CLIENT_H_
#define ROUL_BASIC_CLIENT_H_

#include "../header/roul_struct.h"

int loop_client_black_jack(msg_room_roul_t* msg_rcv, place_roul_t* place,
                            int socket);

int choice_room_roul(msg_info_room_roul_t *msg_rcv);

void get_bet_roul(place_roul_t *place);
 

#endif // ROUL_BASIC_CLIENT_H_