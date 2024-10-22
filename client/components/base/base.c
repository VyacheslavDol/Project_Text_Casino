#include "base.h"

#include <pthread.h>
#include <stdio.h>
#include <string.h>

#include "../termio/termio.h"

int client_run(struct Client *client) {
  client_termio_set_nonblock();

  /* Check if the client pointer is valid */
  if (client == NULL) {
    fprintf(stderr, "Error: Client data is NULL.\n");
    return -1;
  }

  struct MenuNode *current_menu =
      menu_find_node_by_index(menu_tree, client->data.type);

  if (!current_menu) {
    fprintf(stderr, "Error: Current menu not found.\n");
    return -2;
  }

  /* Ensure the action function is valid before calling it */
  if (current_menu->action_data.action) {
    current_menu->action_data.action(client);
  } else {
    fprintf(stderr, "Error: No action defined for the current menu.\n");
    return -3;
  }

  return 0;
}
