#ifndef BL_J_COM_H
#define BL_J_COM_H

#include <openssl/ssl.h>
#include <stdio.h>
#include <strings.h>

#include "../black_jack/bl_j_basic_client.h"
#include "../error/error.h"
#include "../roulette/roul_basic_client.h"
#include "../service/token_service.h"
#include "termio/termio.h"

extern int sock;
extern struct MainMessage send_msg;
extern int balance;

void get_balance();
void *bl_j_run(void *data);
void *roulette(void *data);
void *slot_machine(void *data);
void *exit_game(void *data);

#endif