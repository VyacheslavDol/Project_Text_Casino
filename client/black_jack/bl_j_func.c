#include "bl_j_func.h"

void get_bet(place_t *place_p, int bet_type) {
  // проверка на то, что ставка меньше или равна балансу
  if (bet_type == START_BET) {
    char buff[MAX_SYMBOLS_BET];
    int bet = 0;
    do {
      input_char(buff, sizeof(buff));
      bet = atoi(buff);
      if (bet <= 0) printf("Bet must be > 0\n");
      if (bet > place_p->client.balance) printf("Bet must be <= balane\n");
    } while (bet <= 0 || bet > place_p->client.balance);
    place_p->pl_bet.start = bet;
    place_p->client.balance -= place_p->pl_bet.start;
  } else if (bet_type == INSURANCE_BET) {
    place_p->pl_bet.insurance = place_p->pl_bet.start;
    place_p->client.balance -= place_p->pl_bet.insurance;
  }
}

const char *card_to_string(card_t card) {
  const char *ranks[] = {"0", "A", "2", "3",  "4", "5", "6",
                         "7", "8", "9", "10", "J", "Q", "K"};
  const char *suits[] = {RED "\u2665" RESET, RED "\u2666" RESET,
                         "\033[1;30m\u2663" RESET, "\033[1;30m\u2660" RESET};

  static char buffer[20];
  snprintf(buffer, sizeof(buffer), "%s %s", ranks[card.rank], suits[card.suit]);
  return buffer;
}

void give_up(place_t *place) {
  place->client.balance += place->pl_bet.start / 2;
  place->pl_bet.start = 0;
  place->win_status = 4;
  place->status = END_GAME;
}

int get_num_player(const place_t *place) {
  int count = 0;

  for (int i = 0; i < NUM_PLACE; i++) {
    if (place[i].client.socket != 0 || place[i].status == DISCONNECTED) {
      count++;
    }
  }

  return count;
}

int get_num_act_pl(const room_t *room) {
  int count = 0;
  for (int i = 0; i < room->num_player; i++) {
    if (room->place[i].status == GAME_IN_PROCCESS) {
      count++;
    }
  }
  return count;
}

int get_num_cards(const card_t *cards) {
  int count = 0;
  while (cards[count].rank != 0) {
    count++;
  }
  return count;
}

void reply_start_bet(room_t *room, place_t *place, int my_place_id) {
  printf("\033[H\033[J");  // Очистка экрана
  print_interface_bl_j(room, get_num_player(room->place), my_place_id);
  set_cursor_mode(1);
  set_echo_mode(1);
  conio_mode(1);
  printf("Start bet: ");

  get_bet(place, START_BET);

  conio_mode(0);
  set_cursor_mode(0);
  set_echo_mode(0);
}

void send_msg_place(SSL *my_sock, const place_t *place, int place_id,
                    int room_id, int code_msg) {
  msg_place msg = {0};
  msg.place = *place;
  msg.id_place = place_id;
  msg.msg_code = code_msg;
  msg.id_room = room_id;

  send_msg.type = BL_J_TYPE;
  send_msg.type_data = TYPE_MSG_PLACE;
  *(msg_place *)send_msg.data = msg;

  Interceptor_SSL_write(my_sock, &send_msg);
}