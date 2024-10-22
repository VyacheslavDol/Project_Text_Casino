#ifndef BL_J_STRUCT_H_
#define BL_J_STRUCT_H_

#include <openssl/ssl.h>

#include "token_struct.h"

#define NUM_ROOM 5
#define NAME_SIZE LOGIN_SIZE
#define NUM_PLACE 5
#define NUM_CARDS 52
#define NUM_SUITS 4
#define NUM_RANKS 14
#define DILER_PLACE NUM_PLACE
#define MAX_SYMBOLS_BET 5
#define MAX_HAND_CARD 9  // максимальное число карт на руке
#define FIRST_HAND 0   // для выдачи карт (card_to_place())
#define SECOND_HAND 1  // для выдачи карт (card_to_place())
#define START_BET 0
#define INSURANCE_BET 1
#define INSURANCE 2
#define DILLER_RULE 18

#define PLAEYR_PLAY 2
#define END_GAME 3
#define DISCONNECTED 6

#define SIZE_ANS 5

#define START 1
#define REGISTRATION_BL 4
#define INFO_ROOM 5
#define INFO 8
#define REQUEST_START_BET 11
#define REPLY_START_BET 12
#define REQUEST_SELECT_1 13
#define REPLY_SELECT_1 14
#define REQUEST_SELECT_2 15
#define ROOM_CHOICE 16
#define REPLY_SELECT_2 100
#define MORE 21
#define ENOUGH 22
#define DOUBLE 23
#define SPLIT 24
#define PLACE_ID_MSG 30
#define FIRST_GIVE 31
#define SECOND_GIVE 32
#define MENU_1_REQUEST 33
#define DILER_21 34
#define END_ROUND 41
#define CHOICE_ROOM 45
#define REPLY_CHOICE_ROOM 46
#define NEW_PLACE_ID_MSG_BL 48

#define LEAVE_ROOM 86
#define WAIT_ALL 88
#define READY 89
#define UNREADY 87

#define BLACK_JACK_GAME 77

#define DISCONNECT_PLAYER 400

#define GAME_WAITING_START 1
#define GAME_IN_PROCCESS 2

typedef struct {
  int id;
  char uuid[UUID_SIZE];
  char name[NAME_SIZE];
  int balance;
  SSL* socket;
} player_t;

typedef struct {
  int start;
  int insurance;
} bet_t;

typedef struct {
  int rank;  // Ранг: 1 = Туз, 2 = 2, ..., 10 = 10, 11 = Валет, 12 = Дама, 13
             // = Король
  int suit;  // Масть: 0 = Черви, 1 = Бубны, 2 = Трефы, 3 = Пики
} card_t;

typedef struct {
  card_t first_hand[MAX_HAND_CARD];
  card_t second_hand[MAX_HAND_CARD];
} hand_card_t;

typedef struct {
  player_t client;
  bet_t pl_bet;
  hand_card_t hand_card;
  int win_status;  // 0 - ----, 1 - победа, 2 - ничья, 3 - поражение, 4 - сдался
  int sum[2];  // sum[0] - первая рука, sum[1] - вторая рука (при разделении)
  int status;  // 0 - ожидание конца игры, 3 - раунд закончил
  int next_insur_giveup;  // 1 - продолжить, 2 - Страховка, 3 - Сдача
} place_t;

typedef struct {
  int id;
  int count[3];
  card_t deck[NUM_CARDS];
  place_t place[NUM_PLACE + 1];
  int num_player;   // кол-во игроков в комнате
  int game_status;  // 1 - игра в процесса, 0 - ожидание начала нового раунда
} room_t;

typedef struct {
  int msg_code;
  int id_place;
  room_t room;
} msg_room;

typedef struct {
  int msg_code;
  int id_place;
  int id_room;
  place_t place;
  int count;
} msg_place;

typedef struct {
  int code_msg;
  int num[NUM_ROOM];
  int access[NUM_ROOM];
  int choice_room;
  int flag;  // 0 - ... 1 - в случае если выбранная комната недоступна(нет мест
             // \ идет игра)
} msg_info_room;

#endif