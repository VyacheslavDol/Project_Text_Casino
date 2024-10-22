#include "bl_j_basic_server.h"

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>

#include "../error/error.h"
#include "bl_j_func.h"

void loop_server_black_jack(room_t *room, msg_place *msg_rcv, SSL *socket,
                            const struct User *user) {
  int place_id = msg_rcv->id_place;
  switch (msg_rcv->msg_code) {
    case BLACK_JACK_GAME:
      send_room(room, socket, 0);
      break;

    case REPLY_CHOICE_ROOM:
      for (int k = 0; k < NUM_PLACE; k++)  // Клиент - место
      {
        if (get_num_player(room->place) == NUM_PLACE ||
            room->game_status == GAME_IN_PROCCESS) {
          send_room(room, socket, 1);  // Комната недоступна
          return;
        }
        if (room->place[k].client.socket == NULL)  // посадка игрока на место
        {
          room->place[k].status = PLAEYR_PLAY;
          init_player(&room->place[k].client, socket, user);
          printf("Room is %d/5, num_player = %d\n", k + 1,
                 get_num_player(room->place));
          msg_rcv->id_place = k;

          break;
        }
      }

      room->num_player = get_num_player(room->place);
      msg_rcv->msg_code = PLACE_ID_MSG;
      send_msg_room(socket, room, msg_rcv->id_place, PLACE_ID_MSG);
      // отправка id place
      break;

    case DISCONNECT_PLAYER:
      printf("User leave room\n");
      memset(&room->place[place_id], 0, sizeof(place_t));
      if (room->game_status == GAME_WAITING_START) break;
      room->place[place_id].status = DISCONNECTED;
      printf("count[0] = %d\n", room->count[0]);
      if (room->count[0] == get_num_act_pl(room))
        msg_rcv->msg_code = REPLY_START_BET;
      if (room->count[1] == get_num_act_pl(room))
        msg_rcv->msg_code = REPLY_SELECT_1;

      for (int i = 0; i < NUM_PLACE; i++) {
        printf("Place[%d] status [%d]\n", i, room->place[i].status);
      }
      break;

    case READY:
      room->count[2]++;
      printf("count = %d\n", room->count[2]);
      send_msg_room(socket, room, msg_rcv->id_place, WAIT_ALL);
      break;

    case LEAVE_ROOM:
      printf("Player place[ %d ] leave room [ %d ]\n", place_id, room->id);

      bzero(&(room->place[place_id]), sizeof(place_t));

      printf("--------------num playa = %d, act = %d\n",
             get_num_player(room->place), get_num_act_pl(room));

      room_balance(&room->place[0]);

      room->num_player--;
      int index = 0;
      while (room->place[index].client.socket != 0) {
        printf("New place %d rooom_id = %d\n", index, room->id);
        send_msg_room(room->place[index].client.socket, room, index,
                      NEW_PLACE_ID_MSG_BL);
        index++;
      }
      printf("AFTER LEAVE Active player = %d\n", get_num_act_pl(room));

      send_room(&room[-(room->id)], socket, 0);
      break;

    default:
      break;
  }

  printf("COUNT = %d, num_player %d\n", room->count[2],
         get_num_player(room->place));
  if (room->count[2] == get_num_act_pl(room) &&
      room->game_status == GAME_WAITING_START &&
      get_num_act_pl(room) > 0)  // Выполнилось условие старта
  {
    printf("Запуск игры\n");
    room->count[2] = 0;
    room->num_player = get_num_player(room->place);
    start_room(room);
  }

  if (room->game_status == GAME_IN_PROCCESS) {
    if (msg_rcv->place.client.socket != 0 ||
        msg_rcv->place.status == DISCONNECTED) {
      room->place[place_id] = msg_rcv->place;  // Обновление room
    }

    printf(" act = %d, status = %d, code = %d\n", get_num_act_pl(room),
           room->game_status, msg_rcv->msg_code);
    if ((get_num_act_pl(room) == 0) &&
        (room->game_status == GAME_IN_PROCCESS) &&
        (msg_rcv->msg_code == DISCONNECT_PLAYER)) {
      memset(room, 0, sizeof(room_t));
      init_room(room, msg_rcv->id_room);
      return;
    }

    switch (msg_rcv->msg_code) {
      case REPLY_START_BET:
        printf("Reply start bet\n");
        start_bet_reply(room);
        break;
      case REPLY_SELECT_1:
        select_1_reply(room);
        break;
      case MORE:
        more_reply(room, place_id);
        break;
      case ENOUGH:
        enough_reply(room, place_id);
        break;
      case DOUBLE:
        double_reply(room, place_id);
        break;
      case SPLIT:
        // НЕ ПОДДЕРЖИВАЕТСЯ
        // Реализация Разделение рук
        break;
      default:
        break;
    }
    printf("Active player = %d\n", get_num_act_pl(room));
    // Все игроки завершили раунд
    if (get_num_act_pl(room) == 0 && get_num_player(room->place) != 0) {
      end_round(room);
    }
  }
}

void room_balance(place_t place[]) {
  place_t balanced_place[NUM_PLACE] = {0};
  int index = 0;
  for (int i = 0; i < NUM_PLACE; i++) {
    if (place[i].client.socket != NULL) {
      balanced_place[index++] = place[i];
    }
    memset(&place[i], 0, sizeof(place_t));
  }
  index = 0;
  while (balanced_place[index].client.socket != 0) {
    place[index] = balanced_place[index];
    index++;
  }
}

void start_bet_reply(room_t *room) {
  room->count[0]++;  // Счетчик ответов
  printf("BET REPLY count[0] = %d, active player = %d\n", room->count[0],
         get_num_act_pl(room));

  if (room->count[0] >= get_num_act_pl(room))  // Все игроки поставили ставку
  {
    room->count[0] = 0;
    give_cards(room, FIRST_HAND, 0);  //  Выдача 1ой карты каждому игроку
    // Отправка сообщения
    for (int i = 0; i < room->num_player; i++) {
      if (room->place[i].client.socket != 0)
        send_msg_room(room->place[i].client.socket, room, i, FIRST_GIVE);
    }
    give_cards(room, FIRST_HAND, 1);  //  Выдача 2ой карты каждому игроку
    // Расчет сумм
    for (int i = 0; i < room->num_player; i++) {
      room->place[i].sum[0] = calc_sum(&room->place[i]);
    }
    // Отправка каждому сообщение
    for (int i = 0; i < room->num_player; i++) {
      if (room->place[i].client.socket != 0)
        send_msg_room(room->place[i].client.socket, room, i, SECOND_GIVE);
    }
    // Отправка каждому сообщениe
    for (int i = 0; i < room->num_player; i++) {
      if (room->place[i].client.socket != 0)
        send_msg_room(room->place[i].client.socket, room, i, REQUEST_SELECT_1);
    }
  }
}

void select_1_reply(room_t *room) {
  room->count[1]++;
  for (int i = 0; i < room->num_player; i++) {
    if (room->place[i].client.socket != 0)
      send_msg_room(room->place[i].client.socket, room, i, INFO);
  }
  if (room->count[1] >= get_num_act_pl(room)) {
    room->count[1] = 0;
    // Проверка на 21 у дилера
    if (room->place[DILER_PLACE].sum[FIRST_HAND] == 21) {
      for (int i = 0; i < room->num_player; i++) {
        if (room->place[i].client.socket != 0)
          send_msg_room(room->place[i].client.socket, room, i, DILER_21);
      }
    }
    // Проверка на 21 после раздачи
    for (int i = 0; i < room->num_player; i++) {
      check_21(&room->place[i], &room->place[DILER_PLACE]);
    }
    // Отправка информации
    for (int i = 0; i < room->num_player; i++) {
      if (room->place[i].client.socket != 0)
        send_msg_room(room->place[i].client.socket, room, i, INFO);
    }
    // Отправка запроса с выбором, тем кто еще не закончил раунд
    for (int i = 0; i < room->num_player; i++) {
      if (room->place[i].status != END_GAME) {
        if (room->place[i].client.socket != 0)
          send_msg_room(room->place[i].client.socket, room, i,
                        REQUEST_SELECT_2);
      }
    }
  }
}

void more_reply(room_t *room, int place_id) {
  int k = more(&room->place[place_id], room->deck, FIRST_HAND);
  for (int i = 0; i < room->num_player; i++) {
    if (room->place[i].client.socket != 0)
      send_msg_room(room->place[i].client.socket, room, i, INFO);
  }
  if (k == 0) {
    send_msg_room(room->place[place_id].client.socket, room, place_id,
                  REQUEST_SELECT_2);
  }
}

void enough_reply(room_t *room, int place_id) {
  enough(&room->place[place_id]);
  for (int i = 0; i < room->num_player; i++) {
    if (room->place[i].client.socket != 0)
      send_msg_room(room->place[i].client.socket, room, i, INFO);
  }
}

void double_reply(room_t *room, int place_id) {
  double_bet(&room->place[place_id], room->deck, FIRST_HAND);
  for (int i = 0; i < room->num_player; i++) {
    if (room->place[i].client.socket != 0)
      send_msg_room(room->place[i].client.socket, room, i, INFO);
  }
}

void diler_game(room_t *room)  // Доигровка за дилера
{
  int card_account = 2;
  while (room->place[DILER_PLACE].status != END_GAME) {
    if (diler_end_game(&room->place[DILER_PLACE], DILLER_RULE, room->deck,
                       card_account) == 0) {
      for (int i = 0; i < room->num_player; i++) {
        if (room->place[i].client.socket != 0)
          send_msg_room(room->place[i].client.socket, room, i, INFO);
      }
      card_account++;
    };
  }
}

void start_room(room_t *room) {
  init_deck(room->deck);
  strcpy(room->place[DILER_PLACE].client.name, "DILER\0");
  room->game_status = GAME_IN_PROCCESS;
  for (int i = 0; i < room->num_player; i++) {
    if (room->place[i].client.socket != 0)
      send_msg_room(room->place[i].client.socket, room, i, REQUEST_START_BET);
  }
}

void end_round(room_t *room) {
  diler_game(room);  // Доигровка за дилера
  // Результаты игры
  for (int i = 0; i < room->num_player; i++) {
    black_jack_result(room, i);
  }
  // Отправка информации
  for (int i = 0; i < room->num_player; i++) {
    send_msg_room(room->place[i].client.socket, room, i, INFO);
  }
  // Конец раунда
  for (int i = 0; i < room->num_player; i++) {
    update_balance(room->place[i].client.uuid, room->place[i].client.balance);
    send_msg_room(room->place[i].client.socket, room, i,
                  END_ROUND);  // вопрос прододжить в этой комнате
  }

  int id = room->id;
  memset((void *)room, 0, sizeof(room_t));  // опустошение комнаты
  init_room(room, id);
}

void black_jack_result(room_t *room, int i) {
  if (room->place[i].pl_bet.start != 0) {
    if (room->place[DILER_PLACE].sum[FIRST_HAND] > 21 ||
        room->place[DILER_PLACE].sum[FIRST_HAND] <
            room->place[i].sum[FIRST_HAND]) {
      // win
      room->place[i].win_status = 1;
      room->place[i].client.balance += room->place[i].pl_bet.start * 2;
      room->place[i].pl_bet.start = 0;
    } else if (room->place[DILER_PLACE].sum[FIRST_HAND] >
               room->place[i].sum[FIRST_HAND])  // поражение
    {
      // lose
      room->place[i].win_status = 3;
      room->place[i].pl_bet.start = 0;
    } else if (room->place[DILER_PLACE].sum[FIRST_HAND] ==
               room->place[DILER_PLACE].sum[FIRST_HAND])  // ничья
    {
      // draw
      room->place[i].win_status = 2;
      room->place[i].client.balance += room->place[i].pl_bet.start;
      room->place[i].pl_bet.start = 0;
    }
  }
}

void send_room(room_t *room, SSL *socket, int flag) {
  msg_info_room info_room = {0};
  info_room.code_msg = CHOICE_ROOM;
  info_room.flag = flag;

  for (int i = 0; i < NUM_PLACE; i++) {
    info_room.num[i] = get_num_player(room[i].place);
    info_room.access[i] = room[i].game_status;
  }

  struct MainMessage main_msg = {0};

  *(msg_info_room *)main_msg.data = info_room;
  CHECK_SSL_WRITE(socket, &main_msg, sizeof(struct MainMessage));

  printf("Send Rooms\n");
}
