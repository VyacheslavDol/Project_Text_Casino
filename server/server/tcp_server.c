#include "tcp_server.h"

SSL* ssl_clients[MAX_CLIENTS] = {0};

void init_client_socket(int* client_socket) {
  for (int i = 0; i < MAX_CLIENTS; i++) {
    client_socket[i] = 0;
    ssl_clients[i] = NULL;
  }
}

void create_server_socket(int* server_fd) {
  if ((*server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
    perror("Socket failed");
    exit(EXIT_FAILURE);
  }
}

void set_opt_socket(int* server_fd, const int* opt) {
  if (setsockopt(*server_fd, SOL_SOCKET, SO_REUSEADDR, opt, sizeof(*opt))) {
    perror("Setsockopt failed");
    close(*server_fd);
    exit(EXIT_FAILURE);
  }
}

void bind_socket(int* server_fd, struct sockaddr_in* address) {
  if (bind(*server_fd, (struct sockaddr*)address, sizeof(*address)) < 0) {
    perror("Bind failed");
    close(*server_fd);
    exit(EXIT_FAILURE);
  }
}

void listen_socket(int* server_fd) {
  if (listen(*server_fd, 3) < 0) {
    perror("Listen failed");
    close(*server_fd);
    exit(EXIT_FAILURE);
  }
}

void add_client_in_set(int* sd, const int* client_socket, int* max_sd,
                       fd_set* readfds) {
  for (int i = 0; i < MAX_CLIENTS; i++) {
    *sd = client_socket[i];

    if (*sd > 0) FD_SET(*sd, readfds);

    if (*sd > *max_sd) *max_sd = *sd;
  }
}

void add_client_in_arr(int* client_socket, const int* new_socket) {
  for (int i = 0; i < MAX_CLIENTS; i++) {
    if (client_socket[i] == 0) {
      client_socket[i] = *new_socket;
      printf("Adding to list of sockets as %d\n", i);
      break;
    }
  }
}

void check_active_client(int* sd, int* client_socket, fd_set* readfds,
                         struct sockaddr_in* address, socklen_t* addrlen,
                         room_t* room, room_roul_t* room_roul) {
  for (int i = 0; i < MAX_CLIENTS; i++) {
    *sd = client_socket[i];

    if (FD_ISSET(*sd, readfds)) {
      SSL* ssl = ssl_clients[i];
      if (!ssl) {
        continue;
      }

      struct MainMessage main_msg = {0};
      printf("READ START\n");
      int read_bytes = SSL_read(ssl, &main_msg, sizeof(struct MainMessage));
      int check_leave_game = 0;
      if (read_bytes <= 0) {
        int check_in_game = 0;
        // roul
        int a = 0;
        int flag_roul = 1;
        do {
          if (a == NUM_ROOM_ROUL) break;
          for (int u = 0; u < MAX_PLACE_ROUL; u++) {
            if (room_roul[a].place[u].player.socket == ssl) {
              ((msg_place_roul_t*)main_msg.data)->msg_code = DISCONNECTED_ROUL;
              ((msg_place_roul_t*)main_msg.data)->place_id = u;
              ((msg_place_roul_t*)main_msg.data)->room_id = a;
              flag_roul = 0;
              check_in_game = 1;
            }
          }
          a++;
        } while (flag_roul);

        if (a < NUM_ROOM_ROUL && check_in_game) {
          check_leave_game = 1;
          main_msg.type = ROUL;
        }

        // bl
        int j = 0;
        if (!check_in_game) {
          int flag = 1;
          do {
            for (int k = 0; k < NUM_PLACE; k++) {
              if (room[j].place[k].client.socket == ssl) {
                if (room[j].game_status == GAME_WAITING_START)
                  ((msg_place*)main_msg.data)->msg_code = LEAVE_ROOM;
                else
                  ((msg_place*)main_msg.data)->msg_code = DISCONNECT_PLAYER;
                ((msg_place*)main_msg.data)->id_place = k;
                ((msg_place*)main_msg.data)->id_room = j;
                update_balance(room[j].place[k].client.uuid,
                               room[j].place[k].client.balance);
                flag = 0;
              }
            }

            j++;
            if (j == NUM_ROOM) break;
          } while (flag);
        }

        if (j < NUM_ROOM && !check_in_game) {
          check_leave_game = 1;
          main_msg.type = BLASK_JACK;
        }

        int ssl_error = SSL_get_error(ssl, read_bytes);
        if (ssl_error == SSL_ERROR_ZERO_RETURN ||
            ssl_error == SSL_ERROR_SYSCALL) {
          getpeername(*sd, (struct sockaddr*)address, addrlen);
          printf("Host disconnected, ip %s, port %d\n",
                 inet_ntoa(address->sin_addr), ntohs(address->sin_port));
        } else {
          printf("SSL error occurred for client %d: %d\n", i, ssl_error);
        }

        SSL_shutdown(ssl);
        SSL_free(ssl);
        ssl_clients[i] = NULL;

        close(*sd);
        client_socket[i] = 0;
      } else {
        printf("Received %d bytes from client %d\n", read_bytes, i);
      }

      struct TokenPayload payload = {0};
      if (main_msg.type >= USER_IS_AUTH && !check_leave_game) {
        int status = 0;

        if (validate_token((const unsigned char*)(main_msg.access_token),
                           &payload, ACCESS_KEY)) {
          ++status;
          CHECK_SSL_WRITE(ssl, &status, sizeof(status));
          printf("\033[0;33m\tACCESS IS DEAD\033[0m\n");
          return;
        } else {
          printf("\033[0;32m\tACCESS IS VALID\033[0m\n");
          CHECK_SSL_WRITE(ssl, &status, sizeof(status));
        }
      }
      router(ssl, &main_msg, room, room_roul, &payload);
      printf("READ END\n-------------------------------------------------\n");
    }
  }
}

void start_server(int* sd, int* server_fd, fd_set* readfds,
                  struct sockaddr_in* address, socklen_t* addrlen,
                  SSL_CTX* ctx) {
  int max_sd, new_socket, client_socket[MAX_CLIENTS];

  init_client_socket(client_socket);

  room_t room[NUM_ROOM] = {0};
  room_roul_t room_roul[NUM_ROOM_ROUL] = {0};

  for (int i = 0; i < NUM_ROOM; i++) {
    init_room(&room[i], i);
    init_room_roul(&room_roul[i], i);
  }

  while (1) {
    FD_ZERO(readfds);

    FD_SET(*server_fd, readfds);
    max_sd = *server_fd;

    add_client_in_set(sd, client_socket, &max_sd, readfds);

    int activity = select(max_sd + 1, readfds, NULL, NULL, NULL);

    if ((activity < 0) && (errno != EINTR)) {
      perror("Select error");
    }

    if (FD_ISSET(*server_fd, readfds)) {
      if ((new_socket = accept(*server_fd, (struct sockaddr*)address,
                               (socklen_t*)addrlen)) < 0) {
        perror("Accept failed");
        exit(EXIT_FAILURE);
      }

      SSL* ssl = SSL_new(ctx);
      SSL_set_fd(ssl, new_socket);

      if (SSL_accept(ssl) <= 0) {
        ERR_print_errors_fp(stderr);
        close(new_socket);
        SSL_free(ssl);
      } else {
        printf("New connection, socket fd is %d, ip is : %s, port : %d\n",
               new_socket, inet_ntoa(address->sin_addr),
               ntohs(address->sin_port));

        add_client_in_arr(client_socket, &new_socket);

        for (int i = 0; i < MAX_CLIENTS; i++) {
          if (client_socket[i] == new_socket) {
            ssl_clients[i] = ssl;
            break;
          }
        }
      }
    }

    check_active_client(sd, client_socket, readfds, address, addrlen, room,
                        room_roul);
  }
}

int tcp_server(SSL_CTX* ctx) {
  int server_fd, sd;
  int opt = 1;
  socklen_t addrlen;
  struct sockaddr_in address;
  fd_set readfds;

  create_server_socket(&server_fd);

  set_opt_socket(&server_fd, &opt);

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(TCP_PORT);

  bind_socket(&server_fd, &address);

  listen_socket(&server_fd);

  addrlen = sizeof(address);
  printf("TCP server is running on port: %d\n", TCP_PORT);
  printf("\033[0;32m\tSERVER STARTED SUCCESS\033[0m\n");
  start_server(&sd, &server_fd, &readfds, &address, &addrlen, ctx);

  return 0;
}