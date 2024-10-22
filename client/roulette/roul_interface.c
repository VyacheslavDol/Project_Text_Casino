#include "roul_interface.h"

#define BLACK "\033[30m"

void print_bet_player(place_roul_t *place) {
  int i = 0;
  printf("Yours bets: \n");
  while (i < 3 && place->bet[i].bet_type != 0) {
    printf("#%d %s value %d status [%s] parametr [%d]\n", i,
           bet_to_string(place->bet[i].bet_type), place->bet->bet_value,
           (place->bet_status[i] == WIN) ? GREEN "win" RESET : RED "lose" RESET,
           place->bet[i].parametr);
    i++;
  }
}

void set_color_roul(int color) {
  switch (color) {
    case BLACK_COLOUR:  // Черный
      printf("\033[30m");
      break;
    case RED_COLOUR:  // Красный
      printf("\033[31m");
      break;
    case GREEN_COLOUR:  // Зелёный
      printf("\033[32m");
      break;
    default:
      printf("\033[0m");  // Сброс цвета
      break;
  }
}

void print_table() {
  printf("\n    ");
  for (int i = 3; i < 37; i += 3) {
    printf("%s|%d|%s", (get_colour(i) == BLACK_COLOUR) ? BLACK : RED, i, RESET);
  }
  printf(GREEN "\n |0|" RESET);
  for (int i = 2; i < 36; i += 3) {
    printf("%s|%d|%s", (get_colour(i) == BLACK_COLOUR) ? BLACK : RED, i, RESET);
  }
  printf("\n    ");
  for (int i = 1; i < 35; i += 3) {
    printf("%s|%d|%s", (get_colour(i) == BLACK_COLOUR) ? BLACK : RED, i, RESET);
  }

  printf("\n\n");
}

void print_list_bet(short count) {
  printf(
      "You placed %d of %d bets (count of bets must be > 0)\nChoose type of "
      "bet:\n",
      count, MAX_NUM_BET);
  printf(
      "1.Black\n2.Red\n3.Even\n4.Odd\n5.More 19\n6.Less "
      "18\n7.Dozen\n8.Straight\n0.Complete bets\n");
}

void spin_animation(int sec) {
  time_t start_time;
  start_time = time(NULL);

  while (1) {
    time_t current_time = time(NULL);

    if (current_time - start_time > sec) {
      break;
    }

    int number = rand() % 37;  // Случайное число от 0 до 36

    printf("\033[H\033[J");

    set_color_roul(get_colour(number));
    printf("Number: %d\n", number);
    set_color_roul(0);  // Сброс цвета
    usleep(300000);     // Задержка 300 мс
  }
}

const char *bet_to_string(short bet_type) {
  switch (bet_type) {
    case BET_TYPE_BLACK:
      return "Black";
    case BET_TYPE_RED:
      return "Red";
    case BET_TYPE_EVEN:
      return "Even";
    case BET_TYPE_ODD:
      return "Odd";
    case BET_TYPE_MORE:
      return "More 19";
    case BET_TYPE_LESS:
      return "Less 18";
    case BET_TYPE_DOZEN:
      return "Dozen";
    case BET_TYPE_STRAIGHT:
      return "Straight";
  }
  return "\0";
}

void init_menu_roul(struct termios *oldattr) {
  printf("\033[H\033[J");  // Очистка экрана
  printf("\033[?25l");     // Скрываем курсор
  set_conio_mode(oldattr);  // Устанавливаем новый режим терминала
}