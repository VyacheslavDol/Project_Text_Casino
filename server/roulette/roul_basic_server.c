#include "roul_basic_server.h"

void loop_server_roulette(room_roul_t *room, msg_place_roul_t *msg_rcv,
                          SSL *socket, const struct User *user) {
  int place_id = msg_rcv->place_id;  // id места от которого пришло сообшение

  switch (msg_rcv->msg_code) {
    case ROULETTE:
      send_room_roul(room, socket, 0);
      return;

    case REPLY_CHOICE_ROOM_ROUL:
      for (int k = 0; k < MAX_PLACE_ROUL; k++)  // Клиент - место
      {
        if (get_num_player_roul(room->place) == MAX_PLACE_ROUL ||
            room->status == ROOM_CLOSED) {
          send_room_roul(room, socket, ROOM_CLOSED);  // Комната недоступна
          return;
        }
        if (room->place[k].player.socket == 0)  // !!!!ЗАМЕНИТЬ НА NULL
        {
          room->place[k].status = PLAYER_IN_ROOM_ROUL;
          init_player_roul(&room->place[k].player, socket, user);
          msg_rcv->place_id = k;

          break;
        }
      }
      room->num_players = get_num_player_roul(room->place);
      msg_rcv->msg_code = PLACE_ID_MSG_ROUL;
      send_msg_room_roul(socket, room, msg_rcv->place_id, PLACE_ID_MSG_ROUL);
      // отправка id place
      break;

    case DISCONNECTED_ROUL:
      printf("User leave roulette room\n");
      memset(&room->place[place_id], 0, sizeof(place_roul_t));
      if (room->status == ROOM_CLOSED)  // Отключился во время игры
      {
        if (get_num_player_roul(room->place) == 0) {
          refresh_room(room);
        } else {
          room_balance_roul(&room->place[0]);

          if (get_num_player_roul(room->place) == 0) {
            room->status = ROOM_OPEN;
          }
          int index = 0;
          while (room->place[index].player.socket != SOCKET_CHECK) {
            send_msg_room_roul(room->place[index].player.socket, room, index,
                               NEW_PLACE_ID_MSG_ROUL);
            index++;
          }

          msg_rcv->msg_code = REPLY_BET_ROUL_DISCONNECT;
        }
      } else if (room->status == ROOM_OPEN) {
        room_balance_roul(&room->place[0]);

        if (get_num_player_roul(room->place) == 0) {
          room->status = ROOM_OPEN;
        }
        int index = 0;
        while (room->place[index].player.socket != SOCKET_CHECK) {
          send_msg_room_roul(room->place[index].player.socket, room, index,
                             NEW_PLACE_ID_MSG_ROUL);
          index++;
        }

        break;
      }
      if (room->count[0] == get_num_player_roul(room->place))
        msg_rcv->msg_code = REPLY_BET_ROUL_DISCONNECT;
      break;

    case READY_TO_START:
      room->count[2]++;
      send_msg_room_roul(socket, room, msg_rcv->place_id, WAIT_ALL_READY_ROUL);
      break;

    case LEAVE_ROOM_ROUL:
      bzero(&(room->place[place_id]), sizeof(place_roul_t));
      room_balance_roul(&room->place[0]);

      room->num_players--;
      if (get_num_player_roul(room->place) == 0) {
        room->status = ROOM_OPEN;
      }
      int index = 0;
      while (room->place[index].player.socket != SOCKET_CHECK) {
        send_msg_room_roul(room->place[index].player.socket, room, index,
                           NEW_PLACE_ID_MSG_ROUL);
        index++;
      }
      send_room_roul(&room[-(room->id)], socket, 0);

      break;

    default:
      break;
  }
  if (room->count[2] >= get_num_player_roul(room->place) &&
      room->status == ROOM_OPEN && get_num_player_roul(room->place) != 0) {
    start_room_roul(room);  // send REQUEST BET ROUL
  }

  switch (msg_rcv->msg_code) {
    case REPLY_BET_ROUL:

      room->count[0]++;
      printf("%d %d\n", get_num_player_roul(room->place), room->count[0]);
      room->place[place_id] = msg_rcv->place;  // обновление данных

    /* fall through */
    case REPLY_BET_ROUL_DISCONNECT:
      printf("%d %d\n", get_num_player_roul(room->place), room->count[0]);
      if (room->count[0] >= get_num_player_roul(room->place)) {
        spin_roulette(&room->result.value, &room->result.colour);
        get_result(room);
        //  print_bet_player(&room->place[place_id]);

        for (int i = 0; i < get_num_player_roul(room->place); i++) {
          send_msg_room_roul(room->place[i].player.socket, room, i,
                             RESULT_ROUL);
        }
        refresh_room(room);
      }
      break;
  }
  return;
}

void refresh_room(room_roul_t *room) {
  memset(room, 0,
         sizeof(room_roul_t));  // опустошение комнаты
  init_room_roul(room, room->id);
}

void get_result(room_roul_t *room) {
  for (int i = 0; i < get_num_player_roul(room->place); i++) {
    for (int k = 0; k < MAX_NUM_BET; k++) {
      if (room->result.value == 0 && room->place[i].bet[k].bet_value != 0) {
        room->place[i].bet[k].bet_value = 0;
        continue;
      }
      switch (room->place[i].bet[k].bet_type) {
        case BET_TYPE_BLACK:
          if (room->result.colour == BLACK_COLOUR) {
            room->place[i].player.balance +=
                2 * room->place[i].bet[k].bet_value;
            room->place[i].bet_status[k] = WIN;
          } else {
            room->place[i].bet_status[k] = LOSE;
          }
          break;
        case BET_TYPE_RED:
          if (room->result.colour == RED_COLOUR) {
            room->place[i].player.balance +=
                2 * room->place[i].bet[k].bet_value;
            room->place[i].bet_status[k] = WIN;
          } else {
            room->place[i].bet_status[k] = LOSE;
          }
          break;
        case BET_TYPE_EVEN:
          if (room->result.value % 2 == 0) {
            room->place[i].player.balance +=
                2 * room->place[i].bet[k].bet_value;
            room->place[i].bet_status[k] = WIN;
          } else {
            room->place[i].bet_status[k] = LOSE;
          }
          break;
        case BET_TYPE_ODD:
          if (room->result.value % 2 == 1) {
            room->place[i].player.balance +=
                2 * room->place[i].bet[k].bet_value;
            room->place[i].bet_status[k] = WIN;
          } else {
            room->place[i].bet_status[k] = LOSE;
          }
          break;
        case BET_TYPE_MORE:
          if (room->result.value >= 19) {
            room->place[i].player.balance +=
                2 * room->place[i].bet[k].bet_value;
            room->place[i].bet_status[k] = WIN;
          } else {
            room->place[i].bet_status[k] = LOSE;
          }
          break;
        case BET_TYPE_LESS:
          if (room->result.value <= 18) {
            room->place[i].player.balance +=
                2 * room->place[i].bet[k].bet_value;
            room->place[i].bet_status[k] = WIN;
          } else {
            room->place[i].bet_status[k] = LOSE;
          }
          break;
        case BET_TYPE_DOZEN:
          if ((room->result.value - 1) % 12 ==
              room->place[i].bet[k].parametr - 1) {
            room->place[i].player.balance +=
                3 * room->place[i].bet[k].bet_value;
            room->place[i].bet_status[k] = WIN;
          } else {
            room->place[i].bet_status[k] = LOSE;
          }
          break;
        case BET_TYPE_STRAIGHT:
          if (room->result.value == room->place[i].bet[k].bet_value) {
            room->place[i].player.balance +=
                36 * room->place[i].bet[k].bet_value;
            room->place[i].bet_status[k] = WIN;
          } else {
            room->place[i].bet_status[k] = LOSE;
          }
          break;
      }
    }
    update_balance(room->place[i].player.uuid, room->place[i].player.balance);
  }
}

void start_room_roul(room_roul_t *room) {
  room->status = ROOM_CLOSED;
  room->count[2] = 0;
  for (int i = 0; i < get_num_player_roul(room->place); i++) {
    send_msg_room_roul(room->place[i].player.socket, room, i, REQUEST_BET_ROUL);
  }
}

void init_player_roul(player_roul_t *player, SSL *socket,
                      const struct User *user) {
  strcpy(player->name, user->login);
  strcpy(player->uuid, user->uuid);
  player->balance = user->balance;
  player->socket = socket;
}

void spin_roulette(short *value, short *colour) {
  srand(time(0));
  *value = rand() % 37;
  *colour = get_colour(*value);
}

void init_room_roul(room_roul_t *room, int id) {
  room->id = id;
  room->status = ROOM_OPEN;
}

void room_balance_roul(place_roul_t *place) {
  place_roul_t balanced_place[MAX_PLACE_ROUL] = {0};
  int index = 0;
  for (int i = 0; i < MAX_PLACE_ROUL; i++) {
    if (place[i].player.socket != 0) {
      balanced_place[index++] = place[i];
    }
    memset(&place[i], 0, sizeof(place_roul_t));
  }
  index = 0;
  while (balanced_place[index].player.socket != SOCKET_CHECK) {
    place[index] = balanced_place[index];
    index++;
  }
}

void send_room_roul(room_roul_t *room, SSL *socket, int flag) {
  msg_info_room_roul_t info_room = {0};
  info_room.code_msg = CHOICE_ROOM_ROUL;
  info_room.flag = flag;
  for (int i = 0; i < NUM_ROOM_ROUL; i++) {
    info_room.num_players[i] = get_num_player_roul(room[i].place);
    info_room.access[i] = room[i].status;
  }

  struct MainMessage main_msg = {0};

  *(msg_info_room_roul_t *)main_msg.data = info_room;

  CHECK_SSL_WRITE(socket, &main_msg, sizeof(struct MainMessage));
}