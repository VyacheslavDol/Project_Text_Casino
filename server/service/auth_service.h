#ifndef AUTH_SERVICE
#define AUTH_SERVICE

#include <string.h>

#include "../db/db.h"
#include "../lib/bcrypt/bcrypt.h"
#include "../struct/auth_struct.h"
#include "mail_service.h"
#include "token_service.h"

void refresh_service(const struct Refresh* token,
                     const struct TokenPayload* payload,
                     struct AuthResponse* res);
void login_serivce(const struct Login* login, struct AuthResponse* res);
void registration_serivce(struct Register* reg, struct AuthResponse* res);
void logout_service(const char* uuid);

#endif