#include "auth_controller.h"

int valid_login(const char* str, char* res) {
  const int len = (const int)strlen(str);

  if (len < MIN_LEN_LOGIN || len > MAX_LEN_LOGIN || !isalpha(str[0])) {
    strcat(res, "Incorrect login validation");
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int valid_email(const char* str, char* res) {
  const int len = (const int)strlen(str);

  if (strchr(str, '@') == NULL || strchr(str, '.') == NULL ||
      len <= MIN_LEN_EMAIL || len > MAX_LEN_EMAIL) {
    strcat(res, "Incorrect email validation");
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int valid_password(const char* str, char* res) {
  const int len = (const int)strlen(str);

  if (len < MIN_LEN_PASS || len > MAX_LEN_PASS) {
    strcat(res, "Incorrect password validation");
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

void refresh_controller(struct AuthResponse* res, const struct Refresh* token) {
  struct TokenPayload payload;
  if (validate_token((const unsigned char*)token->refresh_token, &payload,
                     REFRESH_KEY)) {
    strcat(res->error, "Incorrect token");
    return;
  }

  refresh_service(token, &payload, res);
}

void login_controller(struct AuthResponse* res, const struct Login* login) {
  if (valid_email(login->email, res->error) ||
      valid_password(login->password, res->error))
    return;

  login_serivce(login, res);
}

void registration_controller(struct AuthResponse* res, struct Register* reg) {
  if (valid_login(reg->login, res->error) ||
      valid_email(reg->email, res->error) ||
      valid_password(reg->password, res->error))
    return;

  registration_serivce(reg, res);
}