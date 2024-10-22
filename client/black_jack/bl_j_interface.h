#ifndef BL_J_INTERFACE_H_
#define BL_J_INTERFACE_H_

#include <termios.h>

#include "../struct/bl_j_struct.h"

#define RESET "\033[0m"      // Сброс цвета
#define GREEN "\033[1;32m"   // Зеленый цвет
#define RED "\033[1;31m"     // Красный цвет
#define YELLOW "\033[1;33m"  // Желтый цвет

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

char *get_char_status(const place_t *place, char *status);

void print_main_menu(const place_t *place, int index, int my_place_id);

void set_conio_mode(struct termios *oldattr);

void reset_conio_mode(struct termios *oldattr);

void display_menu(int selected, int size_menu, int code_menu);

void print_interface_bl_j(room_t *room, int num_player, int my_place_id);

int choice_menu(int size_menu, int code_menu, room_t *room, int my_place_id);

void init_menu(struct termios *oldattr);

void close_menu(struct termios *oldattr);

void set_cursor_mode(int mode);

void set_echo_mode(int mode);

void conio_mode(int mode);

void clean_tty(void);

#endif  // BL_J_INTERFACE_H_