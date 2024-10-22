#include "profile_service.h"

void get_balance(struct Profile* profile, const struct TokenPayload* payload) {
  PGconn* conn = NULL;
  db_connetc(&conn);

  PGresult* data =
      db_get(&conn, PROFILE, payload->uuid, 2, "balance", "user_id");
  strncpy(profile->user.login, payload->login, LOGIN_SIZE);
  profile->user.balance = atoi(PQgetvalue(data, 0, 0));

  if (data) PQclear(data);
  PQfinish(conn);
}

void update_balance(const char* uuid, int balance) {
  char balance_str[BALANCE_SIZE] = {0};
  snprintf(balance_str, BALANCE_SIZE, "%d", balance);
  PGconn* conn = NULL;
  db_connetc(&conn);
  db_update(&conn, PROFILE, uuid, 3, "balance", balance_str, "user_id");
  PQfinish(conn);
}
