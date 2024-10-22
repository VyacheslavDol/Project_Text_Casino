#include "router.h"

void router(SSL *ssl, struct MainMessage *main_msg, room_t *room,
            room_roul_t *room_roul, const struct TokenPayload *payload) {
  switch (main_msg->type) {
    case REFRESH:
      refresh_router(ssl);
      break;
    case REGISTRATION:
      printf("\033[1;33m\tREGISTRATION\033[0m\n");
      registration_router(ssl);
      break;
    case LOGIN:
      printf("\033[1;33m\tLOGIN\033[0m\n");
      login_router(ssl);
      break;
    case BLASK_JACK:
      printf("\033[1;33m\tBLACK_JACK\033[0m\n");
      black_jack(ssl, main_msg, room, payload);
      break;
    case ROUL:
      printf("\033[1;33m\tROULETTE\033[0m\n");
      roulette(ssl, main_msg, room_roul, payload);
      break;
    case SLOT_MACHINE:
      printf("\033[1;33m\tSLOT_MACHINE\033[0m\n");
      slot_machine(ssl, payload);
      break;
    case GET_BALANCE:
      printf("\033[1;33m\tGET_BALANCE\033[0m\n");
      get_balance_router(ssl, payload);
      break;
    case LOGOUT:
      printf("\033[1;33m\tLOGOUT\033[0m\n");
      logout_service(payload->uuid);
      break;
    case CHAT:
      printf("\033[1;33m\tCHAT\033[0m\n");
      chat(ssl, main_msg);
      break;
    default:
      printf("\033[1;33m\tDEFAULT\033[0m\n");
      break;
  }
}

void refresh_router(SSL *ssl) {
  int status = OK;
  CHECK_SSL_WRITE(ssl, &status, sizeof(status));

  struct Refresh token;

  CHECK_SSL_READ(ssl, &token, sizeof(token));

  struct AuthResponse res;
  res.error[0] = '\0';

  refresh_controller(&res, &token);

  CHECK_SSL_WRITE(ssl, &res, sizeof(res));
}

void login_router(SSL *ssl) {
  int status = OK;
  CHECK_SSL_WRITE(ssl, &status, sizeof(status));

  struct Login login;

  CHECK_SSL_READ(ssl, &login, sizeof(login));

  struct AuthResponse res;
  res.error[0] = '\0';

  login_controller(&res, &login);

  CHECK_SSL_WRITE(ssl, &res, sizeof(res));
}

void registration_router(SSL *ssl) {
  int status = OK;
  CHECK_SSL_WRITE(ssl, &status, sizeof(status));

  struct Register reg;

  CHECK_SSL_READ(ssl, &reg, sizeof(reg));

  struct AuthResponse res;
  res.error[0] = '\0';

  registration_controller(&res, &reg);

  CHECK_SSL_WRITE(ssl, &res, sizeof(res));
}

void black_jack(SSL *ssl, struct MainMessage *main_msg, room_t *room,
                const struct TokenPayload *payload) {
  struct Profile profile = {0};

  strncpy(profile.access_token, main_msg->access_token, TOKEN_SIZE);
  strncpy(profile.user.uuid, payload->uuid, UUID_SIZE);

  if (profile.user.uuid[0]) get_balance(&profile, payload);

  msg_place msg_rcv = *(msg_place *)main_msg->data;
  int room_id = 0;
  if (msg_rcv.msg_code == REPLY_CHOICE_ROOM) {
    room_id = ((msg_info_room *)&msg_rcv)->choice_room;
    printf("CHOICE ROOM = %d\n", ((msg_info_room *)&msg_rcv)->choice_room);
  } else {
    room_id = msg_rcv.id_room;
  }

  loop_server_black_jack(&room[room_id], ((msg_place *)main_msg->data), ssl,
                         &(profile.user));
}

void roulette(SSL *ssl, struct MainMessage *main_msg, room_roul_t *room_roul,
              const struct TokenPayload *payload) {
  struct Profile profile = {0};

  strncpy(profile.access_token, main_msg->access_token, TOKEN_SIZE);
  strncpy(profile.user.uuid, payload->uuid, UUID_SIZE);

  if (profile.user.uuid[0]) get_balance(&profile, payload);

  msg_place_roul_t msg_rcv = {0};
  int room_id = 0;
  msg_rcv = *(msg_place_roul_t *)main_msg->data;

  printf("Recv: Code mes %d\n", msg_rcv.msg_code);

  if (msg_rcv.msg_code == REPLY_CHOICE_ROOM_ROUL) {
    room_id = ((msg_info_room_roul_t *)&msg_rcv)->choice_room;
  } else {
    room_id = msg_rcv.room_id;
  }

  loop_server_roulette(&room_roul[room_id], &msg_rcv, ssl, &profile.user);
}

void slot_machine(SSL *ssl, const struct TokenPayload *payload) {
  int recv[4] = {0};
  for (int i = 0; i < 3; i++) {
    recv[i] = (rand() % 5);
  }

  if (recv[0] == recv[1] && recv[1] == recv[2]) {
    struct Profile profile = {0};
    get_balance(&profile, payload);
    recv[3] = ((recv[0] + 1) * 100);
    update_balance(payload->uuid, recv[3] + profile.user.balance);
  }
  CHECK_SSL_WRITE(ssl, recv, sizeof(recv));
}

void get_balance_router(SSL *ssl, const struct TokenPayload *payload) {
  struct Profile profile = {0};
  get_balance(&profile, payload);
  CHECK_SSL_WRITE(ssl, &profile.user.balance, sizeof(int));
}

void chat(const SSL *ssl, struct MainMessage *reg) {
  printf("\tCHAT %s\n", reg->data);

  struct MainMessage main_msg = {0};

  strlcpy(main_msg.data, reg->data, sizeof(main_msg.data));

  for (int i = 0; i < MAX_CLIENTS; i++) {
    if (ssl_clients[i] && ssl_clients[i] != ssl) {
      size_t message_length = strnlen(main_msg.data, sizeof(main_msg.data));
      CHECK_SSL_WRITE(ssl_clients[i], main_msg.data, message_length + 1);
    } else if (ssl_clients[i] == ssl) {
      CHECK_SSL_WRITE(ssl_clients[i], "NULL", 4);
    }
  }
}