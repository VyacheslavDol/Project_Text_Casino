#include "game_component.h"

#include "base/base.h"

volatile int exit_flag = 1;

void get_balance() {
  struct MainMessage send_get_bal = {0};
  send_get_bal.type = GET_BALANCE_TYPE;
  Interceptor_SSL_write(ssl, &send_get_bal);
  CHECK_ERROR(SSL_read(ssl, &balance, sizeof(int)), -1);
}

void *bl_j_run(void *data) {
  exit_flag = 1;
  struct Client *client = (struct Client *)data;

  init_menu(&oldattr);

  place_t place = {0};
  send_msg.type = BL_J_TYPE;
  send_msg.type_data = TYPE_MSG_PLACE;
  ((msg_place *)send_msg.data)->msg_code = BLACK_JACK_GAME;
  Interceptor_SSL_write(ssl, &send_msg);

  while (exit_flag) {
    struct MainMessage recv_msg = {0};

    CHECK_ERROR(SSL_read(ssl, &recv_msg, sizeof(struct MainMessage)), -1);

    msg_room room = *((msg_room *)recv_msg.data);

    if (loop_client_black_jack(&room, &place, ssl) == 1) break;
  }

  get_balance();

  struct MenuNode *node =
      menu_find_parent_by_index(menu_tree, client->data.type);
  client->data.type = (node ? node->index : client->data.type);

  return NULL;
}

void *roulette(void *data) {
  exit_flag = 1;
  struct Client *client = (struct Client *)data;

  struct MainMessage msg_send = {0};

  init_menu_roul(&oldattr);

  place_roul_t place = {0};  // Мое место

  msg_send.type = ROULETTE_GAME_TYPE;
  ((msg_place_roul_t *)msg_send.data)->msg_code = ROULETTE;
  Interceptor_SSL_write(ssl, &msg_send);

  struct MainMessage rcv_msg = {0};
  while (exit_flag) {
    memset(&rcv_msg, 0, sizeof(struct MainMessage));

    CHECK_ERROR(SSL_read(ssl, &rcv_msg, sizeof(struct MainMessage)), -1);

    loop_client_roulette((msg_room_roul_t *)rcv_msg.data, &place, ssl);
  }

  get_balance();

  struct MenuNode *node =
      menu_find_parent_by_index(menu_tree, client->data.type);
  client->data.type = (node ? node->index : client->data.type);

  return NULL;
}

#define DELAY 150000

void *slot_machine(void *data) {
  printf("\033[H\033[J");

  struct Client *client = (struct Client *)data;
  printf("\033[1;32m\U0001F4B0 Your balance: %d\033[0m\n", balance);

  while (1) {
    printf("1. \U0001F6AA Exit\n<Press any key to continue>\n");
    char type[10];
    CHECK_ERROR(fgets(type, sizeof(type), stdin), NULL);
    printf("\033[H\033[J");
    if (atoi(type) == 1) break;
    send_msg.type = SLOT_MACHINE;
    Interceptor_SSL_write(ssl, &send_msg);

    int recv[4] = {0};
    CHECK_ERROR(SSL_read(ssl, recv, sizeof(recv)), -1);

    const char *symbols[] = {"\U0001F352", "\U0001F34C", "\U0001FA93",
                             "\U0001F514", "\u2B50"};
    for (int i = 0; i < 3; i++) {
      for (int j = 0; j < 8; j++) {
        printf("\t╔════╗ ╔════╗ ╔════╗\n");
        printf("  \U0001F3B0 \t║ %s ║ ║ %s ║ ║ %s ║\n", symbols[rand() % 5],
               symbols[rand() % 5], symbols[rand() % 5]);
        printf("\t╚════╝ ╚════╝ ╚════╝\n");
        usleep(DELAY);
        printf("\033[H\033[J");
      }
    }

    printf("\t╔════╗ ╔════╗ ╔════╗\n");
    printf("  \U0001F3B0 \t║ %s ║ ║ %s ║ ║ %s ║\n", symbols[recv[0]],
           symbols[recv[1]], symbols[recv[2]]);
    printf("\t╚════╝ ╚════╝ ╚════╝\n");

    get_balance();

    if (recv[3]) {
      printf("\033[1;32mYou win %d $\033[0m\n", recv[3]);
    } else
      printf("\033[1;31mYou lose\033[0m\n");
    printf("\033[1;32m\U0001F4B0 Your balance: %d\033[0m\n", balance);
  }

  struct MenuNode *node =
      menu_find_parent_by_index(menu_tree, client->data.type);
  client->data.type = (node ? node->index : client->data.type);

  return NULL;
}

void *exit_game(void *data) {
  (void)data;
  printf("\033[H\033[J");
  restore_terminal_settings();
  printf("\033[?25h");
  exit(EXIT_SUCCESS);
}