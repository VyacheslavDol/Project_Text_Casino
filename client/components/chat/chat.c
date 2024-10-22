#include "chat.h"

#include <fcntl.h>
#include <openssl/ssl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <unistd.h>

#include "../../ssl/ssl.h"
#include "../../struct/menu_struct.h"

#define TIMEOUT_SEC 5
#define TIMEOUT_USEC 0

static int client_thread_create(pthread_t *thread,
                                void *(*start_routine)(void *),
                                struct Client *client) {
  if (pthread_create(thread, NULL, start_routine, client) != 0) {
    fprintf(stderr, "Error: Failed to create thread\n");
    return -1;
  }
  return 0;
}

void client_chat_clear_screen_and_prompt(void) {
  printf("\033[2J\033[HCurrent input: ");
  fflush(stdout);
}

void client_chat_handle_escape_sequence(struct Client *client) {
  if (getchar() == '\n') {
    struct MenuNode *node =
        menu_find_parent_by_index(menu_tree, client->data.type);
    keep_running = node != NULL;
    client->data.type = node ? node->index : client->data.type;
  }
}

void *client_chat_message_receiving(void *data) {
  const struct Client *client = (struct Client *)data;

  while (client->message.input_index >= 0) {
    char server_message[MAX_BUFFER];
    bzero(server_message, sizeof(server_message));

    int bufsize = SSL_read(ssl, server_message, sizeof(server_message));

    if (bufsize == 4 &&
        strncmp(server_message, "NULL", sizeof(server_message)) == 0)
      continue;
    printf("\033[2K\r%s", server_message);
    printf("\n\033[2K\rCurrent input: %s", client->message.text);
    fflush(stdout);
  }

  return NULL;
}

void client_chat_handle_message_sending(struct Client *client) {
  printf("\033[2K\rYou entered: %s", client->message.text);

  send_msg.type = CHAT_TYPE;

  snprintf(send_msg.data, sizeof(send_msg.data), "Client %s: %s",
           client->user.login, client->message.text);

  CHECK_ERROR(SSL_write(ssl, &send_msg, sizeof(send_msg)), -1);

  memset(client->message.text, 0, sizeof(client->message.text));
  client->message.input_index = 0;

  printf("\n\033[2K\rCurrent input: ");
  fflush(stdout);
}

void client_chat_append_character_to_message(struct Client *client, char ch) {
  if (client->message.input_index < MAX_BUFFER - 1) {
    client->message.text[client->message.input_index++] = ch;
    client->message.text[client->message.input_index] = '\0';

    // Вывод текущего ввода
    printf("\033[2K\rCurrent input: %s", client->message.text);
    fflush(stdout);
  } else {
    fprintf(stderr, "Input is too long. Maximum length is %d characters.\n",
            MAX_BUFFER - 1);
  }
}

void *client_chat_handle(void *data) {
  struct Client *client = (struct Client *)data;
  pthread_t receive_thread;

  client_chat_clear_screen_and_prompt();

  const struct MenuNode *initial_menu =
      menu_find_node_by_index(menu_tree, client->data.type);
  if (!initial_menu) {
    fprintf(stderr, "Error: Initial menu node not found.\n");
    return NULL;
  }

  memset(client->message.text, 0, sizeof(client->message.text));
  client->message.input_index = 0;

  snprintf(send_msg.data, sizeof(send_msg.data), "Client %s: is in chat",
           client->user.login);

  CHECK_ERROR(SSL_write(ssl, &send_msg, sizeof(send_msg)), -1);

  if (client_thread_create(&receive_thread, client_chat_message_receiving,
                           client) != 0) {
    fprintf(stderr, "Error creating receive thread.\n");
    return NULL;
  }

  while (keep_running) {
    fd_set read_fds;
    struct timeval timeout = {TIMEOUT_SEC, TIMEOUT_USEC};

    FD_ZERO(&read_fds);
    FD_SET(STDIN_FILENO, &read_fds);

    if (select(STDIN_FILENO + 1, &read_fds, NULL, NULL, &timeout) > 0) {
      int ch = getchar();
      switch (ch) {
        case '\033':
          client_chat_handle_escape_sequence(client);
          break;
        case '\n':
          client_chat_handle_message_sending(client);
          break;
        default:
          if (ch != EOF) client_chat_append_character_to_message(client, ch);
          break;
      }
    }

    const struct MenuNode *new_menu =
        menu_find_node_by_index(menu_tree, client->data.type);

    if (new_menu != initial_menu) {
      if (!initial_menu->action_data.action) {
        fprintf(stderr,
                "Warning: No action associated with the selected menu "
                "node or node not found.\n");
      }
      break;
    }
  }

  client->message.input_index = -1;

  memset(client->message.text, 0, sizeof(client->message.text));

  snprintf(send_msg.data, sizeof(send_msg.data), "Client %s: diconnected",
           client->user.login);

  CHECK_ERROR(SSL_write(ssl, &send_msg, sizeof(send_msg)), -1);
  pthread_join(receive_thread, NULL);
  client->message.input_index = 0;

  return NULL;
}