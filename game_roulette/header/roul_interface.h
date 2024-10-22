#ifndef BL_J_INTERFACE_H_
#define BL_J_INTERFACE_H_

#include "roul_struct.h"
#include <termios.h>

#define RESET   "\033[0m"  // Сброс цвета
#define GREEN   "\033[32m" // Зеленый цвет
#define RED     "\033[31m" // Красный цвет
#define YELLOW  "\033[33m" // Желтый цвет

#define MENU_1_SIZE 3
#define MENU_1_CODE 0
#define MENU_2_SIZE 4
#define MENU_2_CODE 1
#define MENU_3_SIZE 2
#define MENU_3_CODE 2
#define MENU_4_SIZE 2
#define MENU_4_CODE 3
#define MENU_5_SIZE 2
#define MENU_5_CODE 4

const char* bet_to_string(short bet_type);

void print_table();

void set_color(int color);

void print_list_bet(short count);

void print_bet_player(place_roul_t* place);

void spin_animation(int sec);

void set_color(int color);

void set_conio_mode(struct termios *oldattr);

void reset_conio_mode(struct termios *oldattr);

int choice_menu(int size_menu, int code_menu, room_roul_t *room, int my_place_id); 

void init_menu(struct termios *oldattr);

void close_menu(struct termios *oldattr);

void set_cursor_mode(int mode);

void set_echo_mode(int mode);

void conio_mode(int mode);

void clean_tty(void);



#endif  // BL_J_INTERFACE_H_