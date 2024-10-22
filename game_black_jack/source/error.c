#include "../error.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* color(const char* err) {
  static char new_err[256];
  snprintf(new_err, sizeof(new_err), "\033[1;31m%s\033[0m\n", err);
  return new_err;
}

void error(const char* err) { printf("%s", color(err)); }

void error_func(const char* func, const char* err) {
  char new_err[256];
  snprintf(new_err, sizeof(new_err), "[ %s(?) -> %s ]:\nERROR:\t%s", func, err, 
                  strerror(errno));
  fprintf(stderr, "%s", color(new_err));
  exit(EXIT_FAILURE);
}