#ifndef ROUL_STRUCT_H_
#define ROUL_STRUCT_H_

#define SIZE_NAME 20

#define ROULETTE 99

#define TRUE_T 1
#define FALSE_T 0
#define GREEN_COLOUR 1
#define BLACK_COLOUR 2
#define RED_COLOUR 3

#define NUM_ROOM_ROUL 5
#define MAX_PLACE_ROUL 5
#define MAX_NUM_BET 3
#define NUM_TYPE_BET 8
#define MAX_VALUE_BET 1000

#define ROOM_CLOSED 2
#define ROOM_OPEN 1

#define PLAYER_IN_ROOM_ROUL 9

#define SOCKET_CHECK 0

// CODE MSG ROULETTE
#define CHOICE_ROOM_ROUL 40 // отправка информации о комнатах
#define REPLY_CHOICE_ROOM_ROUL 41 // ответ с выбранной комнатой
#define DISCONNECT_PLAYER_ROUL 44 
#define PLACE_ID_MSG_ROUL 45 //Выдача комнатного id
#define REPLY_BET_ROUL 47 // Клиент прислал ставки
#define REQUEST_BET_ROUL 49 
#define READY_BET_ROUL 53
#define READY_TO_START 56
#define LEAVE_ROOM_ROUL 58
#define NEW_PLACE_ID_MSG_ROUL 60
#define RESULT_ROUL 64
#define DISCONNECTED_ROUL 78
#define REPLY_BET_ROUL_DISCONNECT 79

#define BET_TYPE_BLACK 1
#define BET_TYPE_RED 2
#define BET_TYPE_EVEN 3
#define BET_TYPE_ODD 4
#define BET_TYPE_MORE 5
#define BET_TYPE_LESS 6
#define BET_TYPE_DOZEN 7
#define BET_TYPE_STRAIGHT 8


#define WIN 1
#define LOSE 0

#define FINISHED_BET 3

#define GAME_WAITING_START_ROUL 47 // статус - ожидания гоотвниости всех
#define WAIT_ALL_READY_ROUL 55 // - ответ на ready

// STATUS GAME
#define GAME_IN_PROCESS_ROUL 2
#define END_ROUND 3



typedef struct {
  // char uuid[UUID_SIZE];
  char name[SIZE_NAME];
  int balance;
  int socket;  // SSL*
} player_roul_t;

typedef struct {
  short bet_type;
  short parametr;
  int bet_value;
} bet_roul_t;

typedef struct {
  player_roul_t player;
  bet_roul_t bet[MAX_NUM_BET];
  short count_bet;
  int bet_status[MAX_NUM_BET + 1];
  int status;
} place_roul_t;

typedef struct {
  short value;  // 0-36
  short colour; // 1-green, 2-black, 3-red
} result_roul_t;

typedef struct {
  int id;
  int count[3];
  int status; // 1 - прием ставок открыт (комната доступна), 2 - идет розыгрыш (комната недоступна)
  short num_players;
  result_roul_t result;
  place_roul_t place[MAX_PLACE_ROUL];
} room_roul_t;

typedef struct {
  int msg_code;
  int place_id;
  int room_id;
  place_roul_t place;
} msg_place_roul_t;

typedef struct {
  int code_msg;
  int dest_place_id;
  room_roul_t room;
} msg_room_roul_t;

typedef struct {
  int code_msg;
  short num_players[MAX_PLACE_ROUL];
  short access[MAX_PLACE_ROUL];  // 1 - доступна, 2 - недоступна
  int choice_room;
  short flag;  // 0 - .. , 3 - выбранная комната недоступна
} msg_info_room_roul_t;

#endif  //  ROUL_STRUCT_H_