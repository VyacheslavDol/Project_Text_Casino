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

// иницализация игрока (позже данные будут постпуать из БД)
void init_player(player_t *plp, SSL *pl_socket, const struct User *user);

// инициализация колоды
void init_deck(card_t deck[]);

// перемешивание колоды
void shaffle_deck(card_t deck[]);

// выдача карты, возвращаемое значение - кол-во карт, оставшихся в колоде
int card_to_place(card_t deck[], place_t *place_p, int hand, int card_account);

// Функция подсчета суммы   (Добавить расчет второй руки)
int calc_sum(const place_t *place);

// Функция действия ход (взятие карты), возвр знаечние 1 - перебор, 0 - < 21
int more(place_t *place, card_t *deck, int hand);

// Фенкция действия удвоения. Возвр значения: -1 - низкий баланс, 1 - > 21
int double_bet(place_t *place, card_t *deck, int hand);

// Функция действия сдача
void give_up(place_t *place);

// Функция проверки 21 после раздачи двух карт
void check_21(place_t *place_player, const place_t *place_diler);

// Функция действия отказ
void enough(place_t *place);

// Функция доигрывания дилера. Возвр значения: - 1 без добора, 0 - взял еще
// карту
int diler_end_game(place_t *place_diler, int stop_number, card_t *deck,
                   int card_account);

// Функция подсчета кол-ва игроков в комнате
int get_num_player(const place_t *place);

// Функция инициализации комнаты
void init_room(room_t *room, int id);

// Функция подсчета активных (не окончивших раунд)
int get_num_act_pl(const room_t *room);

// Функция поиска следующего места для карты в руке
int get_card_account(const card_t *card_hand);

// Функция клиента
// void client_black_jack(int my_sock, int place_id);

// Функция выдачи карт всему столу
void give_cards(room_t *room, int hand, int card_account);

// Функция возваращающая кол-во карт
int get_num_cards(const card_t *cards);

void send_msg_place(SSL *my_sock, const place_t *place, int place_id,
                    int room_id, int code_msg);

void send_msg_room(SSL *socket, const room_t *room, int place_id, int code_msg);

#endif