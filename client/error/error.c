#include "error.h"

char *color(const char *err) {
  static char new_err[256];
  size_t len = snprintf(new_err, sizeof(new_err), "\033[1;31m%s\033[0m\n", err);
  if (len >= sizeof(new_err)) {
    fprintf(stderr, "Error: string too long for color()\n");
    exit(EXIT_FAILURE);
  }
  return new_err;
}

void error(const char *err) { printf("%s", color(err)); }

void error_func(const char *func, const char *err) {
  char new_err[512];
  char formatted_msg[512];

  int ret =
      snprintf(formatted_msg, sizeof(formatted_msg),
               "[ %s(?) -> %s ]:\nERROR:\t%s", func, err, strerror(errno));

  if (ret < 0 || ret >= (int)sizeof(formatted_msg)) {
    fprintf(stderr, "Error: formatting error message\n");
    exit(EXIT_FAILURE);
  }

  strlcpy(new_err, formatted_msg, sizeof(new_err));

  fprintf(stderr, "%s", color(new_err));
  exit(EXIT_FAILURE);
}
