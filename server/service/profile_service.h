#ifndef PROFILE_SERVICE_H
#define PROFILE_SERVICE_H

#include "../db/db.h"
#include "../struct/auth_struct.h"
#include "../struct/main_message_strcut.h"

#define BALANCE_SIZE 128

void get_balance(struct Profile* profile, const struct TokenPayload* payload);
void update_balance(const char* uuid, int balance);

#endif