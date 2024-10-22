#ifndef TOKEN_SERVICE
#define TOKEN_SERVICE

#include <openssl/evp.h>
#include <openssl/hmac.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../db/db.h"
#include "../struct/token_struct.h"

#define ACCESS_KEY getenv("ACCESS_KEY")
#define REFRESH_KEY getenv("REFRESH_KEY")

#define ACCESS_TIME 5
#define REFRESH_TIME 604800

void init_token_payload(const char *uuid, const char *login,
                        struct TokenPayload *payload, const time_t t);

void generate_token(const struct TokenPayload *payload, char *token,
                    const char *secret_key);

void access_token(const char *uuid, const char *login, char *token);
void refresh_token(const char *uuid, const char *login, char *token);
void all_tokens(const char *uuid, const char *login, struct Tokens *tokens);

int validate_token(const unsigned char *token, struct TokenPayload *payload,
                   const char *secret_key);
void token_bin_to_hex(const char *token_bin, char *token_hex);

int db_get_token(const char *uuid, char *token_hex);
void db_add_token(const char *uuid, const char *login, struct Tokens *tokens);
void db_update_token(const char *uuid, const char *login,
                     struct Tokens *tokens);

#endif