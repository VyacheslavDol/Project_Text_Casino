#include "token_service.h"

bool refresh(SSL* ssl, struct AuthResponse* res) {
  struct Refresh token;
  if (read_token_file(token.refresh_token)) {
    return 0;
  }

  send_msg.type = REFRESH_TYPE;
  send_type(ssl);

  CHECK_ERROR(SSL_write(ssl, &token, sizeof(struct Refresh)), -1);
  CHECK_ERROR(SSL_read(ssl, res, sizeof(*res)), -1);

  return 1;
}

void write_token_file(const char* token) {
  FILE* file = fopen(FILE_NAME, "wb");
  if (file == NULL) {
    return;
  }

  if (fwrite(token, 1, TOKEN_SIZE, file) != TOKEN_SIZE) {
    perror("ERR WRITE FILE");
  }

  fclose(file);
}

bool read_token_file(char* token) {
  FILE* file = fopen(FILE_NAME, "rb");
  if (file == NULL) {
    return 1;
  }

  if (fread(token, 1, TOKEN_SIZE, file) != TOKEN_SIZE) {
    perror("ERR READ FILE");
    fclose(file);
    return 1;
  }

  fclose(file);
  return 0;
}

int check_token_err(const char* str) {
  return (!strcmp(str, "Incorrect token") || !strcmp(str, "Token not found") ||
          !strcmp(str, "Token not cmp"));
}