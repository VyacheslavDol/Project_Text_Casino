#ifndef BL_J_FUNC_H_
#define BL_J_FUNC_H_

#include <openssl/ssl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <termio.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

#include "../error/error.h"
#include "../ssl/ssl.h"
#include "../struct/bl_j_struct.h"
#include "../struct/main_message_strcut.h"
#include "bl_j_interface.h"
#include "input_char.h"

// получение начальной ставки
void get_bet(place_t *place_p, int bet_type);

// Функция для преобразования карты в строку
const char *card_to_string(card_t card);

// Функция действия сдача
void give_up(place_t *place);

// Функция подсчета кол-ва игроков в комнате
int get_num_player(const place_t *place);

// Функция поиска следующего места для карты в руке
int get_card_account(const card_t *card_hand);

int get_num_act_pl(const room_t *room);

int get_num_cards(const card_t *cards);

void reply_start_bet(room_t *room, place_t *place, int my_place_id);

void send_msg_place(SSL *my_sock, const place_t *place, int place_id,
                    int room_id, int code_msg);

#endif