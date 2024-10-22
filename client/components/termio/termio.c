#include "termio.h"

#include <stdio.h>
#include <stdlib.h>
#include <termio.h>
#include <unistd.h>

struct termios old_attr_bl;

void disable_echo() {
  struct termios t;
  tcgetattr(STDIN_FILENO, &t);
  t.c_lflag &= ~(ECHO | ICANON);
  tcsetattr(STDIN_FILENO, TCSANOW, &t);
}

void enable_echo() {
  struct termios t;
  tcgetattr(STDIN_FILENO, &t);
  t.c_lflag |= (ECHO | ICANON);
  tcsetattr(STDIN_FILENO, TCSANOW, &t);
}

void get_password(char *password, int max_len) {
  int i = 0;

  while (i < max_len - 1) {
    char ch = getchar();
    if (ch == '\n') {
      break;
    } else if (ch == 127 || ch == '\b') {
      if (i > 0) {
        printf("\b \b");
        i--;
      }
    } else {
      password[i++] = ch;
      printf("*");
    }
  }
  password[i] = '\0';
  printf("\n");
}

void client_termio_set_nonblock(void) {
  struct termios old_settings, new_settings;
  tcgetattr(STDIN_FILENO, &old_settings);
  new_settings = old_settings;
  new_settings.c_lflag &= ~(ICANON | ECHO);
  if (tcsetattr(STDIN_FILENO, TCSANOW, &new_settings) < 0) {
    fprintf(stderr, "Error: Failed to set terminal to non-blocking mode\n");
  }
}

void save_terminal_settings() {
  if (tcgetattr(STDIN_FILENO, &old_attr_bl) != 0) {
    perror("tcgetattr");
    exit(EXIT_FAILURE);
  }
}

void restore_terminal_settings() {
  if (tcsetattr(STDIN_FILENO, TCSANOW, &old_attr_bl) != 0) {
    perror("tcsetattr");
    exit(EXIT_FAILURE);
  }
}
