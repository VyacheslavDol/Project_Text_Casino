#ifndef AUTH_STRUCT
#define AUTH_STRUCT

#define LOGIN_SIZE 32
#define EMAIL_SIZE 64
#define PASSWORD_SIZE 256

#define MIN_LEN_PASS 8
#define BUFF 1024

#include "token_struct.h"

struct Login {
  char email[EMAIL_SIZE];
  char password[PASSWORD_SIZE];
};

struct Register {
  char login[LOGIN_SIZE];
  char email[EMAIL_SIZE];
  char password[PASSWORD_SIZE];
};

struct User {
  char uuid[UUID_SIZE];
  char login[LOGIN_SIZE];
  int balance;
  int is_active;
};

struct Request {
  char access_token[TOKEN_SIZE];
  int type;
};

struct AuthResponse {
  struct Tokens tokens;
  struct User user;
  char error[BUFF];
};

#endif