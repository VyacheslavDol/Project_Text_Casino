#ifndef ROULETTE_INTERFACE_H_
#define ROULETTE_INTERFACE_H_

#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

#include "../black_jack/bl_j_interface.h"
#include "../struct/roul_struct.h"
#include "roul_func.h"

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

const char *bet_to_string(short bet_type);

void print_table();

void set_color(int color);

void print_list_bet(short count);

void print_bet_player(place_roul_t *place);

void spin_animation(int sec);

void set_color_roul(int color);

void reset_conio_mode_roul(struct termios *oldattr);

void init_menu_roul(struct termios *oldattr);

void set_cursor_mode_roul(int mode);

#endif  // ROULETTE_INTERFACE_H_