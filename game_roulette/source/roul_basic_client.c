#include "../header/roul_basic_client.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

#include "../header/input_char.h"
#include "../header/main_message_struct.h"
#include "../header/roul_func.h"
#include "../header/roul_interface.h"
#include "../header/roul_struct.h"

extern struct termios old_attr;

extern volatile int exit_flag;

int loop_client_black_jack(msg_room_roul_t* msg_rcv, place_roul_t* place,
                           int socket) {
  int selected = 0;
 // struct MainMessage msg_send = {0};
  int msg_code = 0;

  int place_id = msg_rcv->dest_place_id;

  init_menu(&old_attr);
  int ans = 0;

  switch (msg_rcv->code_msg) {
    case CHOICE_ROOM_ROUL:

      ans = choice_room_roul((msg_info_room_roul_t*)msg_rcv);

      if (ans == 0) {  // выход в первое меню
        exit_flag = 0;
        return 0;
      } else if (ans == MAX_PLACE_ROUL + 1)  // refresh комнат
      {
        clean_tty();
        send_msg_place_roul(socket, place, 0, 0, ROULETTE);
        return 0;
      }
      send_msg_info_client(socket,
                           ans - 1);  // выбор комнаты REPLY_CHOICE_ROOM_ROUL
      clean_tty();
      return 0;

    case PLACE_ID_MSG_ROUL:
      clean_tty();

      selected = choice_menu(MENU_5_SIZE, MENU_5_CODE, NULL, 0);

      if (selected == 0) {
        msg_code = READY_TO_START;
        place->status = PLAYER_IN_ROOM_ROUL;
      } else if (selected == 1) {
        msg_code = LEAVE_ROOM_ROUL;
      }
      send_msg_place_roul(socket, place, place_id, msg_rcv->room.id, msg_code);
      printf("Room #%d\nMy place is #%d\nWaiting for start..\n",
             msg_rcv->room.id, place_id);
      return 0;

    case NEW_PLACE_ID_MSG_ROUL:
      clean_tty();
      place_id = msg_rcv->dest_place_id;
      printf("Room #%d\nMy place is #%d\nWaiting for start..\n",
             msg_rcv->room.id, place_id);
      return 0;

    default:
      *place = msg_rcv->room.place[place_id];  // обновление своего поля
      printf(
          "Info room id = %d, place_id = %d, socket for server = %d, num_place "
          "= "
          "%d\n",
          msg_rcv->room.id, msg_rcv->dest_place_id,
          msg_rcv->room.place[msg_rcv->dest_place_id].player.socket,
          msg_rcv->room.num_players);
     
      switch (msg_rcv->code_msg) {
        case REQUEST_BET_ROUL:
          get_bet_roul(place);
          send_msg_place_roul(socket, place, place_id, msg_rcv->room.id,
                              REPLY_BET_ROUL);
          break;

        case RESULT_ROUL:

          spin_animation(3);
          clean_tty();
          printf("ROULETTE Number %d \n", msg_rcv->room.result.value);
          print_table();
          set_color(get_colour(msg_rcv->room.result.value));
          set_color(0);
          print_bet_player(&(msg_rcv->room.place[msg_rcv->dest_place_id]));
          printf(
              "\nRound is over\n Your balance = %d\n<Press any key to "
              "continue>\n",
              msg_rcv->room.place[place_id].player.balance);
          getchar();

          clean_tty();
          set_cursor_mode(0);
          conio_mode(0);
          selected =
              choice_menu(MENU_4_SIZE, MENU_4_CODE, NULL, 0);  // leave/stay

          if (selected == 0) {
            send_msg_info_client(socket,
                                 msg_rcv->room.id);  // REPLY_CHOICE_ROOM
          } else if (selected == 1) {
            send_msg_place_roul(socket, place, 0, 0, ROULETTE);
          }

          break;
    }
  }

  return 0;
}

void get_bet_roul(place_roul_t *place) {
  printf("Ставим ставкИ!!\n");
  set_echo_mode(1);
  set_cursor_mode(1);
  conio_mode(1);
  char buff[5];
  int ans;

  int i = 0;
  while (place->count_bet != MAX_NUM_BET &&
         place->bet_status[3] != FINISHED_BET) {
    do {
      clean_tty();
      printf("Your balance: %d\n", place->player.balance);
      print_table();
      print_list_bet(place->count_bet);
      ans = get_ans_int();
    } while ((ans < 0 || ans > NUM_TYPE_BET));

    if (ans == 0 && place->count_bet != 0) {
      place->bet_status[3] = FINISHED_BET;
      continue;
    }else if(ans == 0)
    {
      continue;
    }
    place->bet[i].bet_type = ans;

    if (ans > 6) {
      clean_tty();
      printf("Your balance: %d\n", place->player.balance);
      print_table();
      printf("Parametrs: ");
      switch (ans) {
        case 7:
          printf("1.First\n2.Second\n3.Third\n");
          do {
            input_char(buff, sizeof(buff));
          } while ((ans = atoi(buff)) < 1 || ans > 3);
          place->bet[i].parametr = ans;
          break;
        case 8:
          printf("Choose number: ");
          do {
            input_char(buff, sizeof(buff));
          } while ((ans = atoi(buff)) < 0 || ans > 36);
          place->bet[i].parametr = ans;
        default:
          break;
      }
    }

    do {
      clean_tty();
      printf("Your balance: %d\n", place->player.balance);
      print_table();
      printf("Value of bet (> 0, < your balance, < 1000): ");
      input_char(buff, sizeof(buff));
      ans = atoi(buff);
    } while (ans <= 0 || ans > MAX_VALUE_BET || ans >= place->player.balance);
    
    place->bet[i].bet_value = ans;
    place->player.balance -=ans;
    place->count_bet++;
    i++;
  }
}



int choice_room_roul(msg_info_room_roul_t* msg_rcv) {
  set_echo_mode(1);
  set_cursor_mode(1);
  conio_mode(1);

  for (int i = 0; i < MAX_PLACE_ROUL; i++) {
    printf("%d. Room #%d (%d/%d) [%s]\n", i + 1, i,
           msg_rcv->num_players[i], MAX_PLACE_ROUL,
           (msg_rcv->num_players[i] == MAX_PLACE_ROUL) ? RED "full" RESET
           : (msg_rcv->access[i] == ROOM_CLOSED)       ? RED "in game" RESET
                                                       : GREEN "open" RESET);
  }
  printf("%d. Refresh\n", MAX_PLACE_ROUL + 1);
  printf("0. Exit\n");
  if (((msg_info_room_roul_t*)msg_rcv)->flag == ROOM_CLOSED)
    printf(YELLOW "Room is unreacheable\n" RESET);

  int ans = 0;
  do {
    printf("Choose room: ");
    ans = get_ans_int();
  } while (ans < 0 || ans > MAX_PLACE_ROUL + 1);
  conio_mode(0);
  return ans;
}