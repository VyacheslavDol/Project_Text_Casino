#ifndef TOKEN_STRUCT
#define TOKEN_STRUCT

#include <openssl/sha.h>
#include <openssl/ssl.h>
#include <time.h>

#define UUID_SIZE 38
#define LOGIN_SIZE 32
#define TOKEN_SIZE sizeof(struct TokenPayload) + SHA256_DIGEST_LENGTH

struct TokenPayload {
  char uuid[UUID_SIZE];
  char login[LOGIN_SIZE];
  time_t issued_at;
  time_t expires_at;
};

struct Tokens {
  char access_token[TOKEN_SIZE];
  char refresh_token[TOKEN_SIZE];
};

struct Refresh {
  char refresh_token[TOKEN_SIZE];
};

struct MailConf {
  SSL* ssl;
  char uuid[UUID_SIZE];
};

#endif