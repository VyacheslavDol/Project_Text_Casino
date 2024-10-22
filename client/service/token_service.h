#ifndef TOKEN_SERVICE
#define TOKEN_SERVICE

#include <openssl/ssl.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "../error/error.h"
#include "../struct/main_message_strcut.h"
#include "../struct/token_struct.h"
#include "auth_service.h"

#define MAX_NUM_REQUEST 5
#define FILE_NAME ".token"

extern struct MainMessage send_msg;

bool read_token_file(char* token);
bool refresh(SSL* ssl, struct AuthResponse* res);
void write_token_file(const char* token);
int check_token_err(const char* str);

#endif