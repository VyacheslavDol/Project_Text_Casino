#ifndef BL_J_BASIC_CLIENT_H_
#define BL_J_BASIC_CLIENT_H_

#include <openssl/ssl.h>

#include "../struct/bl_j_struct.h"
#include "bl_j_func.h"
#include "bl_j_interface.h"

extern struct termios oldattr;
extern struct MainMessage send_msg;

int loop_client_black_jack(msg_room *rcv_msg, place_t *place, SSL *socket);

#endif  // BL_J_BASIC_CLIENT_H_