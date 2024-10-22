#include "bl_j_basic_client.h"

#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

/* КЛИЕНТ \ */

int loop_client_black_jack(msg_room *rcv_msg, place_t *place, SSL *socket) {
  int selected = 0;
  int msg_code = 0;

  init_menu(&oldattr);

  switch (rcv_msg->msg_code) {
    case CHOICE_ROOM:
      set_echo_mode(1);
      set_cursor_mode(1);
      conio_mode(1);

      for (int i = 0; i < NUM_PLACE; i++) {
        printf("%d. \U0001F465 Room #%d (%d/%d) [%s]\n", i + 1, i,
               ((msg_info_room *)rcv_msg)->num[i], NUM_PLACE,
               (((msg_info_room *)rcv_msg)->num[i] == NUM_PLACE) ? RED
                   "full" RESET
               : (((msg_info_room *)rcv_msg)->access[i] == GAME_IN_PROCCESS)
                   ? RED "in game" RESET
                   : GREEN "open" RESET);
      }

      printf("%d. \033[1;32m\u21BB\033[0m  Refresh\n", NUM_PLACE + 1);
      printf("0. \U0001F6AA Exit\n");

      if (((msg_info_room *)rcv_msg)->flag == 1)
        printf(YELLOW "Room is unreacheable\n" RESET);

      int ans = 0;

      do {
        printf("Choose room: ");
        ans = get_ans_int();
      } while (ans < 0 || ans > NUM_PLACE + 1);
      conio_mode(0);
      if (ans == 0) return 1;

      if (ans == NUM_PLACE + 1) {
        clean_tty();
        send_msg.type = BL_J_TYPE;
        send_msg.type_data = TYPE_MSG_PLACE;
        ((msg_place *)send_msg.data)->msg_code = BLACK_JACK_GAME;
        Interceptor_SSL_write(ssl, &send_msg);
        break;
      }

      send_msg.type = BL_J_TYPE;
      send_msg.type_data = TYPE_MSG_INFO_ROOM;
      ((msg_info_room *)send_msg.data)->code_msg = REPLY_CHOICE_ROOM;
      ((msg_info_room *)send_msg.data)->choice_room = ans - 1;
      Interceptor_SSL_write(ssl, &send_msg);

      break;

    case PLACE_ID_MSG:
      clean_tty();

      selected = choice_menu(MENU_5_SIZE, MENU_5_CODE, NULL, 0);

      if (selected == 0) {
        msg_code = READY;
        place->status = GAME_IN_PROCCESS;
      } else if (selected == 1) {
        msg_code = LEAVE_ROOM;
      }
      send_msg_place(socket, place, rcv_msg->id_place, rcv_msg->room.id,
                     msg_code);
      clean_tty();
      printf("Room #%d\nMy place is #%d\nWaiting for start..\n",
             rcv_msg->room.id, rcv_msg->id_place);
      break;
    case NEW_PLACE_ID_MSG_BL:
      clean_tty();
      printf("Room #%d\nMy place is #%d\nWaiting for start..\n",
             rcv_msg->room.id, rcv_msg->id_place);

      break;

    case WAIT_ALL:
      clean_tty();
      printf("Room #%d\nMy place is #%d\nWaiting for start..\n",
             rcv_msg->room.id, rcv_msg->id_place);
      break;

    case END_ROUND:
      print_interface_bl_j(&rcv_msg->room, get_num_player(rcv_msg->room.place),
                           rcv_msg->id_place);
      printf(
          "\nRound is over\n Result %s, Your balance = %d\n<Press any key to "
          "continue>\n",
          (place->win_status == 1)   ? GREEN "win" RESET
          : (place->win_status == 2) ? YELLOW "draw" RESET
                                     : RED "lose" RESET,
          place->client.balance);
      getchar();
      clean_tty();
      selected = choice_menu(MENU_4_SIZE, MENU_4_CODE, NULL, 0);  // leave/stay

      if (selected == 0) {
        send_msg.type = BL_J_TYPE;
        send_msg.type_data = TYPE_MSG_INFO_ROOM;
        ((msg_info_room *)send_msg.data)->code_msg = REPLY_CHOICE_ROOM;
        ((msg_info_room *)send_msg.data)->choice_room = rcv_msg->room.id;
        ((msg_info_room *)send_msg.data)->flag = 0;
        Interceptor_SSL_write(ssl, &send_msg);

      } else if (selected == 1) {
        send_msg.type = BL_J_TYPE;
        send_msg.type_data = TYPE_MSG_PLACE;
        ((msg_place *)send_msg.data)->msg_code = BLACK_JACK_GAME;
        Interceptor_SSL_write(ssl, &send_msg);
      }
      break;

    default:

      init_menu(&oldattr);

      int place_id = rcv_msg->id_place;

      printf("code rcv = %d\n, ", rcv_msg->msg_code);

      *place = rcv_msg->room.place[place_id];  // Обноваление поля своего place

      /* Рекция на запросы */
      int flag = 1;
      switch (rcv_msg->msg_code) {
          // Стартовая ставка
        case REQUEST_START_BET:
          reply_start_bet(&rcv_msg->room, place, place_id);
          send_msg_place(socket, place, place_id, rcv_msg->room.id,
                         REPLY_START_BET);
          printf("\033[H\033[J");  // Очистка экрана
          print_interface_bl_j(&rcv_msg->room,
                               get_num_player(rcv_msg->room.place), place_id);

          break;

        case REQUEST_SELECT_1:

          printf("\033[H\033[J");  // Очистка экрана
          selected =
              choice_menu(MENU_1_SIZE, MENU_1_CODE, &rcv_msg->room, place_id);
          place->next_insur_giveup = selected + 1;

          if (place->next_insur_giveup == END_GAME)  // Сдача
          {
            give_up(place);
          } else if (place->next_insur_giveup == INSURANCE)  // Страховка
          {
            get_bet(place, INSURANCE_BET);
          }
          send_msg_place(socket, place, place_id, rcv_msg->room.id,
                         REPLY_SELECT_1);

          break;

        case REQUEST_SELECT_2:
          do {
            printf("\033[H\033[J");  // Очистка экрана
            selected =
                choice_menu(MENU_2_SIZE, MENU_2_CODE, &rcv_msg->room, place_id);
            if (selected == 2 && place->client.balance < place->pl_bet.start) {
              printf("Too low balance\n");
              flag = 1;
            } else if (selected == 3) {
              printf("This action is not supported\n");
              flag = 1;
            } else {
              flag = 0;
            }
          } while (flag);
          if (selected == 0) {
            msg_code = MORE;
          } else if (selected == 1) {
            msg_code = ENOUGH;
          } else if (selected == 2) {
            msg_code = DOUBLE;
          } else if (selected == 3) {
            msg_code = SPLIT;
          }

          send_msg_place(socket, place, place_id, rcv_msg->room.id, msg_code);

          break;

        default:
          printf("\033[H\033[J");  // Очистка экрана
          print_interface_bl_j(&rcv_msg->room,
                               get_num_player(rcv_msg->room.place), place_id);
          break;
      }

      break;  // Главного switch
  }
  close_menu(&oldattr);
  return 0;
}