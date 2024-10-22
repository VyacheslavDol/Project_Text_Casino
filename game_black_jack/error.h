#ifndef ERROR_H_
#define ERROR_H_

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ERR_SIZE 256

void error(const char* err);
void error_func(const char* func, const char* err);

#define CHECK_ERROR(func, res)   \
  if ((func) == res) {           \
    error_func(__func__, #func); \
  }

#endif