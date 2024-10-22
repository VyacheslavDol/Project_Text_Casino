#include "token_service.h"

void all_tokens(const char *uuid, const char *login, struct Tokens *tokens) {
  access_token(uuid, login, tokens->access_token);
  refresh_token(uuid, login, tokens->refresh_token);
}

int db_get_token(const char *uuid, char *token_hex) {
  PGconn *conn = NULL;
  db_connetc(&conn);
  PGresult *data = db_get(&conn, TOKEN, uuid, 2, "refresh_token", "user_id");
  if (PQntuples(data) <= 0) {
    return EXIT_FAILURE;
  }

  strncpy(token_hex, PQgetvalue(data, 0, 0), ((TOKEN_SIZE)*2));

  PQclear(data);
  PQfinish(conn);
  return EXIT_SUCCESS;
}

void db_add_token(const char *uuid, const char *login, struct Tokens *tokens) {
  all_tokens(uuid, login, tokens);

  char token_hex[(TOKEN_SIZE)*2 + 1];
  token_bin_to_hex(tokens->refresh_token, token_hex);

  PGconn *conn = NULL;
  db_connetc(&conn);
  db_add(&conn, TOKEN, 2, uuid, token_hex);
  PQfinish(conn);
}

void db_update_token(const char *uuid, const char *login,
                     struct Tokens *tokens) {
  all_tokens(uuid, login, tokens);

  char token_hex[(TOKEN_SIZE)*2 + 1];
  token_bin_to_hex(tokens->refresh_token, token_hex);

  PGconn *conn = NULL;
  db_connetc(&conn);
  db_update(&conn, TOKEN, uuid, 3, "refresh_token", token_hex, "user_id");
  PQfinish(conn);
}

void access_token(const char *uuid, const char *login, char *token) {
  struct TokenPayload payload;
  init_token_payload(uuid, login, &payload, ACCESS_TIME);
  generate_token(&payload, token, ACCESS_KEY);
}

void refresh_token(const char *uuid, const char *login, char *token) {
  struct TokenPayload payload;
  init_token_payload(uuid, login, &payload, REFRESH_TIME);
  generate_token(&payload, token, REFRESH_KEY);
}

void init_token_payload(const char *uuid, const char *login,
                        struct TokenPayload *payload, const time_t t) {
  payload->issued_at = time(NULL);
  payload->expires_at = time(NULL) + t;
  strncpy(payload->uuid, uuid, UUID_SIZE - 1);
  strncpy(payload->login, login, LOGIN_SIZE - 1);
}

void sign_data(const unsigned char *payload, const size_t payload_size,
               const char *secret_key, unsigned char *digest) {
  unsigned int digest_len;
  HMAC(EVP_sha256(), secret_key, strlen(secret_key), payload, payload_size,
       digest, &digest_len);
}

void generate_token(const struct TokenPayload *payload, char *token,
                    const char *secret_key) {
  size_t payload_size = sizeof(*payload);

  unsigned char signature[SHA256_DIGEST_LENGTH];
  sign_data((const unsigned char *)payload, payload_size, secret_key,
            signature);
  memcpy(token, payload, payload_size);
  memcpy(token + payload_size, signature, SHA256_DIGEST_LENGTH);
}

int validate_token(const unsigned char *token, struct TokenPayload *payload,
                   const char *secret_key) {
  size_t payload_size = sizeof(*payload);

  if (TOKEN_SIZE != payload_size + SHA256_DIGEST_LENGTH) {
    fprintf(stderr, "Некорректный размер токена\n");
    return EXIT_FAILURE;
  }

  memcpy(payload, token, payload_size);

  unsigned char expected_signature[SHA256_DIGEST_LENGTH];
  sign_data(token, payload_size, secret_key, expected_signature);

  if (memcmp(token + payload_size, expected_signature, SHA256_DIGEST_LENGTH) !=
      0) {
    fprintf(stderr, "Подпись токена недействительна\n");
    return EXIT_FAILURE;
  }

  if (time(NULL) > payload->expires_at) {
    fprintf(stderr, "Срок действия токена истёк\n");
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

void token_bin_to_hex(const char *token_bin, char *token_hex) {
  for (size_t i = 0; i < TOKEN_SIZE; i++) {
    sprintf(token_hex + (i * 2), "%02x", token_bin[i]);
  }
  token_hex[TOKEN_SIZE * 2] = '\0';
}
