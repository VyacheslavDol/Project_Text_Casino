#include "input_char.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void input_char(char* buff,
                long size_buff) {  // функция корректного и удобного ввода с
                                   // помощью fgets с очисткой буффера
  memset(buff, 0, size_buff);
  const char* err = fgets(buff, size_buff, stdin);
  if (err == NULL) return;
  buff[strcspn(buff, "\n")] = '\0';
  if (strcmp(&buff[size_buff - 2], "\0")) {
    int c;
    while ((c = fgetc(stdin)) != '\n' && c != EOF)
      ;
  }
}

int get_ans_int() {  // функция получения ответа в виде чисел (от 0 до 99)
  int ansi;
  char ans[10];
  input_char(ans, sizeof(ans));
  ansi = atoi(ans);
  return ansi;
}