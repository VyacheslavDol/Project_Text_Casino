#include "../header/roul_interface.h"
#include "../header/roul_struct.h"
#include "../header/roul_func.h"
#include <termios.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>

static const char *MENU_[5][4] = {{"\0", "\0", "\0", "\0"}, 
                                    {"\0", "\0", "\0", "\0"},
                                    {"Roulette", "Exit", "\0", "\0"},
                                    {"Stay in room", "Leave room"},
                                    {"Ready", "Leave room"}};

#define RESET   "\033[0m"  // Сброс цвета
#define GREEN   "\033[32m" // Зеленый цвет
#define RED     "\033[31m" // Красный цвет
#define YELLOW  "\033[33m" // Желтый цвет
#define BLACK   "\033[30m"

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

void set_color(int color) {
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

void print_table()
{
    printf("\n    ");
    for(int i = 3; i < 37; i += 3)
    {
        printf("%s|%d|%s", (get_colour(i) == BLACK_COLOUR) ? BLACK : RED, i, RESET);
    }
    printf(GREEN"\n |0|"RESET);
        for(int i = 2; i < 36; i += 3)
    {
        printf("%s|%d|%s", (get_colour(i) == BLACK_COLOUR) ? BLACK : RED, i, RESET);
    }
    printf("\n    ");
        for(int i = 1; i < 35; i+=3)
    {
        printf("%s|%d|%s", (get_colour(i) == BLACK_COLOUR) ? BLACK : RED, i, RESET);
    }

    printf("\n\n");
}

void print_list_bet(short count)
{
    printf("You placed %d of %d bets (count of bets must be > 0)\nChoose type of bet:\n", count, MAX_NUM_BET);
    printf(
        "1.Black\n2.Red\n3.Even\n4.Odd\n5.More 19\n6.Less "
        "18\n7.Dozen\n8.Straight\n0.Complete bets\n");
}

void spin_animation(int sec)
{   
    time_t start_time, current_time;
    start_time = time(NULL);

    while (1) {
        current_time = time(NULL);

        if (current_time - start_time > sec) {
            break; 
        }

        int number = rand() % 37; // Случайное число от 0 до 36

        printf("\033[H\033[J");
        
        set_color(get_colour(number));
        printf("Number: %d\n", number);
        set_color(0); // Сброс цвета
        usleep(300000); // Задержка 300 мс
    }   
}

void set_conio_mode(struct termios *oldattr) 
{
    struct termios newattr;
    tcgetattr(STDIN_FILENO, &newattr);
    *oldattr = newattr; // Сохраняем текущие атрибуты терминала
    newattr.c_lflag &= ~(ICANON | ECHO); // Отключаем канонический режим и эхо
    tcsetattr(STDIN_FILENO, TCSANOW, &newattr);
}

void reset_conio_mode(struct termios *oldattr) 
{
    tcsetattr(STDIN_FILENO, TCSANOW, oldattr);
}

void conio_mode(int mode)
{   
    struct termios attr;
    tcgetattr(STDIN_FILENO, &attr);
    if(mode == 1)
        attr.c_lflag |= (ICANON); // Включаем канонический режим
    else if(mode == 0)
        attr.c_lflag &= (-ICANON);
    tcsetattr(STDIN_FILENO, TCSANOW, &attr);
}

void display_menu(int selected, int size_menu, int code_menu)
{   
    clean_tty();
    printf("Выбор:\n");
    for (int i = 0; i < size_menu; i++) {
        if (i == selected) {
            printf("> %s <\n", MENU_[code_menu][i]); // Выделяем выбранный элемент
        } else {
            printf("  %s\n", MENU_[code_menu][i]);
        }
    }
}


const char* bet_to_string(short bet_type)
{
    switch (bet_type) {
        case BET_TYPE_BLACK:   return "Black";
        case BET_TYPE_RED:     return "Red";
        case BET_TYPE_EVEN:    return "Even";
        case BET_TYPE_ODD:     return "Odd";
        case BET_TYPE_MORE:    return "More 19";
        case BET_TYPE_LESS:    return "Less 18";
        case BET_TYPE_DOZEN:   return "Dozen";
        case BET_TYPE_STRAIGHT: return "Straight";
    }
    return "\0";
}


int choice_menu(int size_menu, int code_menu, room_roul_t *room, int my_place_id)
{   
    printf("%d", my_place_id);
    clean_tty();    
    int running = 1;
    int selected = 0;
    while (running) 
    {   
        if(room != NULL)
        {
            //print_interface_bl_j(room, get_num_player_roul(room->place), my_place_id);
        }
            display_menu(selected, size_menu, code_menu);
        // Получаем ввод
        char c = getchar();
        if (c == 27) 
        { // ESC означает начало последовательности
            getchar(); // Пропускаем '['
            switch (getchar()) 
            { // Читаем направление стрелки
                case 'A': // Стрелка вверх
                    selected = (selected == 0) ? size_menu - 1 : selected - 1;
                    break;
                case 'B': // Стрелка вниз
                    selected = (selected == size_menu - 1) ? 0 : selected + 1;
                    break;
            }
        } else if (c == '\n') 
        { // Enter
            running = 0;
        }

        printf("\033[H\033[J"); // Очистка экрана
    }

    return selected;
}

void init_menu(struct termios *oldattr)
{
    printf("\033[H\033[J"); // Очистка экрана
    printf("\033[?25l"); // Скрываем курсор
    set_conio_mode(oldattr); // Устанавливаем новый режим терминала
}

void close_menu(struct termios *oldattr)
{
    set_echo_mode(1);
    printf("\033[?25h"); // Показываем курсор перед выходом
    reset_conio_mode(oldattr); // Возвращаем исходные настройки терминала
}

void set_cursor_mode(int mode)
{
    if (mode)
    {
        printf("\033[?25h");
    }
    else{
        printf("\033[?25l");
    }
}

void set_echo_mode(int mode)
{
    struct termios tty;
    tcgetattr(STDIN_FILENO, &tty); // Получаем текущие настройки терминала.

    if (mode) {
        tty.c_lflag |= ECHO; // Включаем echo.
    } else {
        tty.c_lflag &= ~ECHO; // Отключаем echo.
    }

    tcsetattr(STDIN_FILENO, TCSANOW, &tty); // Применяем изменения.
}

void clean_tty(void)
{
    printf("\033[H\033[J"); // Очистка экрана
}
