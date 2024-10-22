#include "roul_func.h"

int get_num_player_roul(place_roul_t *place) {
  int count = 0;
  for (int i = 0; i < MAX_PLACE_ROUL; i++) {
    if (place[i].player.socket != NULL) {
      count++;
    }
  }
  return count;
}

short x_btwn_a_b_or_c_d(short x, short a, short b, short c, short d) {
  return ((x >= a && x <= b) || (x >= c && x <= d)) ? 1 : 0;
}

short get_colour(short x) {
  if (x == 0) {
    return GREEN_COLOUR;
  } else if (((x_btwn_a_b_or_c_d(x, 1, 10, 19, 28)) && (x % 2 == 0)) ||
             ((x_btwn_a_b_or_c_d(x, 11, 18, 29, 36)) && (x % 2 == 1))) {
    return BLACK_COLOUR;
  } else {
    return RED_COLOUR;
  }
}

void send_msg_room_roul(SSL *socket, const room_roul_t *room, int place_id,
                        int code_msg) {
  msg_room_roul_t msg;
  msg.room = *room;
  msg.dest_place_id = place_id;
  msg.code_msg = code_msg;
  struct MainMessage main_msg = {0};
  main_msg.type_data = TYPE_MSG_ROOM_ROUL;
  main_msg.type = TYPE_MSG_ROOM_ROUL;
  *(msg_room_roul_t *)main_msg.data = msg;
  if (socket != SOCKET_CHECK) Interceptor_SSL_write(socket, &main_msg);
}

void send_msg_place_roul(SSL *socket, const place_roul_t *place, int place_id,
                         int room_id, int code_msg) {
  msg_place_roul_t msg;
  msg.place = *place;
  msg.place_id = place_id;
  msg.msg_code = code_msg;
  msg.room_id = room_id;
  struct MainMessage main_msg = {0};
  main_msg.type_data = TYPE_MSG_PLACE_ROUL;
  main_msg.type = TYPE_MSG_ROOM_ROUL;
  *(msg_place_roul_t *)main_msg.data = msg;
  if (socket != SOCKET_CHECK) Interceptor_SSL_write(socket, &main_msg);
}

void send_msg_info_client(SSL *socket, int choice) {
  struct MainMessage snd_msg = {0};
  snd_msg.type = TYPE_MSG_ROOM_ROUL;
  snd_msg.type_data = MSG_INFO_ROOM_ROUL;
  ((msg_place_roul_t *)snd_msg.data)->msg_code = REPLY_CHOICE_ROOM_ROUL;
  ((msg_info_room_roul_t *)snd_msg.data)->choice_room = choice;
  if (socket != SOCKET_CHECK) Interceptor_SSL_write(socket, &snd_msg);
}