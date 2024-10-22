#include "auth_service.h"

void refresh_service(const struct Refresh* token,
                     const struct TokenPayload* payload,
                     struct AuthResponse* res) {
  printf("\033[1;33m\tREFRESH\033[0m\n");
  char tokenDB_hex[(TOKEN_SIZE)*2 - 1];
  if (db_get_token(payload->uuid, tokenDB_hex)) {
    strcpy(res->error, "Token not found");
    return;
  }

  char token_hex[(TOKEN_SIZE)*2 - 1];
  token_bin_to_hex(token->refresh_token, token_hex);

  if (strcmp(token_hex, tokenDB_hex)) {
    strcpy(res->error, "Token not cmp");
    return;
  }

  struct Tokens tokens;
  db_update_token(payload->uuid, payload->login, &tokens);
  res->tokens = tokens;

  PGconn* conn = NULL;
  db_connetc(&conn);

  struct User user = {0};
  strncpy(user.login, payload->login, LOGIN_SIZE);
  PGresult* data =
      db_get(&conn, MAIL, payload->uuid, 2, "is_active", "user_id");
  user.is_active = PQgetvalue(data, 0, 0)[0] == 't' ? 1 : 0;

  PQclear(data);
  res->user = user;
}

void login_serivce(const struct Login* login, struct AuthResponse* res) {
  PGconn* conn = NULL;
  db_connetc(&conn);

  PGresult* data =
      db_get(&conn, USERS, login->email, 4, "id", "login", "password", "email");
  if (PQntuples(data) <= 0) {
    strcpy(res->error, "Invalid email or password");
    return;
  }

  char hash[PASSWORD_SIZE] = {0};

  strncpy(hash, PQgetvalue(data, 0, 2), PASSWORD_SIZE - 1);

  struct User user = {0};

  if (bcrypt_checkpw(login->password, hash) == 0) {
    strncpy(user.login, PQgetvalue(data, 0, 1), LOGIN_SIZE - 1);
  } else {
    strcat(res->error, "Invalid email or password");
  }

  char uuid[UUID_SIZE];
  strncpy(uuid, PQgetvalue(data, 0, 0), UUID_SIZE - 1);
  PQclear(data);

  struct Tokens tokens;
  db_update_token(uuid, user.login, &tokens);
  res->tokens = tokens;

  data = db_get(&conn, MAIL, uuid, 2, "is_active", "user_id");

  user.is_active = PQgetvalue(data, 0, 0)[0] == 't' ? 1 : 0;
  PQclear(data);

  res->user = user;

  PQfinish(conn);
}

void registration_serivce(struct Register* reg, struct AuthResponse* res) {
  PGconn* conn = NULL;
  db_connetc(&conn);

  char salt[BCRYPT_HASHSIZE];
  char hash[BCRYPT_HASHSIZE];

  bcrypt_gensalt(12, salt);

  bcrypt_hashpw(reg->password, salt, hash);
  // user
  if (db_add(&conn, USERS, 3, reg->login, reg->email, hash)) {
    strcat(res->error, "User already exists");
    return;
  }

  struct User user = {0};
  strncpy(user.login, reg->login, LOGIN_SIZE);
  user.is_active = 0;
  res->user = user;

  PGresult* data = db_get(&conn, USERS, reg->email, 2, "id", "email");

  char uuid[UUID_SIZE];
  strncpy(uuid, PQgetvalue(data, 0, 0), UUID_SIZE - 1);
  // profile
  db_add(&conn, PROFILE, 2, uuid, "200");

  PQfinish(conn);
  PQclear(data);
  // token
  struct Tokens tokens;
  db_add_token(uuid, reg->login, &tokens);
  res->tokens = tokens;
  // mail
  char uuid_mail[UUID_SIZE];
  gen_uuid_mail(uuid_mail);
  send_confirmation_email(reg->email, uuid_mail);
  db_add_link(uuid, uuid_mail);
}

void logout_service(const char* uuid) {
  PGconn* conn = NULL;
  db_connetc(&conn);
  db_update(&conn, TOKEN, uuid, 3, "refresh_token", "", "user_id");
  PQfinish(conn);
}