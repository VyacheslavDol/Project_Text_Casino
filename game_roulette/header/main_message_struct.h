#ifndef MAIN_MES_H
#define MAIN_MES_H

//#include "auth_struct.h"
#include "roul_struct.h"
//#include "token_struct.h"

#define DATA_SIZE_MAIN_MESSAGE sizeof(msg_room_roul_t)

#define TYPE_MSG_PLACE 1
#define TYPE_MSG_ROOM 2
#define TYPE_MSG_INFO_ROOM 3

#define ROULETTE_GAME_TYPE 15

#define TYPE_MSG_ROOM_ROUL 4
#define MSG_PLACE_ROUL 36
#define MSG_INFO_ROOM_ROUL 37
#define TYPE_MSG_PLACE_ROUL 38

struct MainMessage {
  int type;
  int type_data;
  //char access_token[TOKEN_SIZE];
  char data[DATA_SIZE_MAIN_MESSAGE];
};

#endif