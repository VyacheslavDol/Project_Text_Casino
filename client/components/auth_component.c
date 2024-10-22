#include "auth_component.h"

bool auth_main(SSL *ssl, struct AuthResponse *res) {
  if (refresh(ssl, res)) goto refresh;

auth:
  printf(
      "1. \U0001F511 Login \n2. \U0001F4DD Registration \n3. \U0001F6AA "
      "Exit\n");
  char type[10];
  CHECK_ERROR(fgets(type, sizeof(type), stdin), NULL);

  switch (atoi(type)) {
    case 1:
      auth_login(ssl, res);
      break;
    case 2:
      auth_register(ssl, res);
      break;
    case 3:
      ssl_close();
      printf("\033[H\033[J");
      exit(EXIT_SUCCESS);
      break;
    default:
      break;
  }

refresh:
  if (res->error[0]) {
    if (check_token_err(res->error)) {
      remove(FILE_NAME);
    }
    printf("\033[H\033[J");
    error(res->error);
    goto auth;
  }

  memcpy(access_token, res->tokens.access_token, (TOKEN_SIZE));
  write_token_file(res->tokens.refresh_token);

  struct Client client = {0};

  if (res->user.is_active) {
    client.user = res->user;
    client.message.input_index = 0;
    memset(client.message.text, 0, MAX_BUFFER);
    get_balance();
    client_run(&client);
  } else {
    printf("\033[H\033[J\U0001F4E8 Confirm your account by email\n");
  }

  return EXIT_SUCCESS;
}

void auth_login(SSL *ssl, struct AuthResponse *res) {
  struct Login login;

  printf("\U0001F4E7 Email: ");
  CHECK_ERROR(fgets(login.email, EMAIL_SIZE, stdin), NULL);

  printf("\U0001F512 Password: ");
  disable_echo();
  get_password(login.password, sizeof(login.password));
  enable_echo();

  login_service(ssl, &login, res);
}

void auth_register(SSL *ssl, struct AuthResponse *res) {
  struct Register reg;

  printf("\U0001F464 Login: ");
  CHECK_ERROR(fgets(reg.login, LOGIN_SIZE, stdin), NULL);

  printf("\U0001F4E7 Email: ");
  CHECK_ERROR(fgets(reg.email, EMAIL_SIZE, stdin), NULL);

  char conf_pass[PASSWORD_SIZE] = {0};

  printf("\U0001F512 Password: ");
  disable_echo();
  get_password(reg.password, sizeof(reg.password));
  enable_echo();

  printf("\U0001F512 Password: ");
  disable_echo();
  get_password(conf_pass, sizeof(conf_pass));
  enable_echo();

  if (strncmp(reg.password, conf_pass, PASSWORD_SIZE)) {
    strcpy(res->error, "Password don`t match");
    return;
  }

  register_service(ssl, &reg, res);
}