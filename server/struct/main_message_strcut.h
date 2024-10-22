#ifndef MAIN_MES_H
#define MAIN_MES_H

#include "auth_struct.h"
#include "bl_j_struct.h"
#include "token_struct.h"

#define TYPE_MSG_PLACE 1
#define TYPE_MSG_ROOM 2
#define TYPE_MSG_INFO_ROOM 3
#define TYPE_MSG_ROOM_ROUL 4
#define MSG_PLACE_ROUL 36
#define MSG_INFO_ROOM_ROUL 37
#define TYPE_MSG_PLACE_ROUL 38

#define MAX_BUFFER 1024
#define DATA_SIZE_MAIN_MESSAGE sizeof(msg_room)

#define REFRESH_TYPE 1
#define REGISTR_TYPE 2
#define LOGIN_TYPE 3
#define MENU_TYPE 0
#define BL_J_TYPE 10
#define ROULETTE_GAME_TYPE 11
#define GET_BALANCE_TYPE 14
#define LOGOUT_TYPE 15
#define SLOT_MACHINE 12
#define CHAT_TYPE 4

struct MainMessage {
  int type;
  int type_data;
  char access_token[TOKEN_SIZE];
  char data[DATA_SIZE_MAIN_MESSAGE];
};

#endif