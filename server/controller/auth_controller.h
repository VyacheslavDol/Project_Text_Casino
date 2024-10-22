#ifndef AUTH_CONTROLLER
#define AUTH_CONTROLLER

#include <stdio.h>
#include <string.h>

#include "../service/auth_service.h"
#include "../service/token_service.h"
#include "../struct/auth_struct.h"

// status
#define OK 200
#define VALIDATION_ERR 1

void refresh_controller(struct AuthResponse* res, const struct Refresh* token);
void login_controller(struct AuthResponse* res, const struct Login* login);
void registration_controller(struct AuthResponse* res, struct Register* reg);

#endif