#ifndef CLIENT_BASE_H_
#define CLIENT_BASE_H_

#include <netinet/in.h>

#include "../../struct/auth_struct.h"
#include "../../struct/main_message_strcut.h"
#include "../../struct/menu_struct.h"

extern struct MenuNode *menu_tree; /* Pointer to the menu tree */

struct ClientMessage {
  char text[MAX_BUFFER]; /* Client message text buffer */
  int input_index;       /* Current input position in buffer */
};

struct Client {
  int socket; /* Client connection socket */
  struct User user;
  struct MainMessage data;      /* Client information (user data and submenu) */
  struct ClientMessage message; /* Structure for client messages */
};

/* Function to run the client */
int client_run(struct Client *client);

#endif  // CLIENT_BASE_H_
