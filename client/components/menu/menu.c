#include "menu.h"

#include <stdio.h>
#include <unistd.h>

#include "../termio/termio.h"

#define TIMEOUT_SEC 1  /* Timeout duration in seconds for input */
#define TIMEOUT_USEC 0 /* Timeout duration in microseconds for input */

void client_menu_display(const struct MenuNode *current_menu,
                         struct Client client, int selected_index) {
  printf("\033[2J\033[H");
  if (!strcmp(current_menu->name, "Main Menu"))
    printf(
        "\U0001F525 WELCOME CASINO\n\U0001F47B USERNAME: %s\n\U0001F4B0 "
        "BALANCE: %d\n\n",
        client.user.login, balance);
  printf("%s\n", current_menu->name);

  /* Iterate through each submenu item */
  for (int i = 0; i < current_menu->submenu_count; i++) {
    /* Highlight the selected item */
    if (i == selected_index) {
      printf("\033[1;32m> %s <\033[0m\n", current_menu->submenu[i]->name);
    } else {
      printf("  %s\n", current_menu->submenu[i]->name);
    }
  }
}

void client_menu_handle_input(struct Client *client,
                              const struct MenuNode *current_menu,
                              int *selected_index) {
  char ch = getchar();

  if (ch == '\033') {
    /* Handle escape sequence for navigation */
    ch = getchar();
    if (ch == '\n') {
      struct MenuNode *node =
          menu_find_parent_by_index(menu_tree, client->data.type);
      keep_running = node != NULL;
      client->data.type = node ? node->index : client->data.type;
    } else {
      switch (getchar()) {
        case 'A': /* Arrow up */
          if (*selected_index > 0) {
            (*selected_index)--;
          }
          break;
        case 'B': /* Arrow down */
          if (*selected_index + 1 < current_menu->submenu_count) {
            (*selected_index)++;
          }
          break;
      }
    }
  } else if (ch == '\n') { /* Enter key */
    if (*selected_index >= 0 && *selected_index < current_menu->submenu_count) {
      client->data.type = current_menu->submenu[*selected_index]->index;
    }
  }
}

void client_menu_process_input(struct Client *client,
                               const struct MenuNode *current_menu,
                               int *selected_index) {
  fd_set read_fds;
  struct timeval timeout = {TIMEOUT_SEC, TIMEOUT_USEC};

  FD_ZERO(&read_fds);
  FD_SET(STDIN_FILENO, &read_fds);

  /* Wait for input with a timeout */
  if (select(STDIN_FILENO + 1, &read_fds, NULL, NULL, &timeout) > 0) {
    client_menu_handle_input(client, current_menu, selected_index);
    client_menu_display(current_menu, *client, *selected_index);
  }
}

void *client_menu_handle(void *data) {
  struct Client *client = (struct Client *)data;

  /* Find the initial menu node */
  const struct MenuNode *current_menu =
      menu_find_node_by_index(menu_tree, client->data.type);
  if (current_menu == NULL) {
    fprintf(stderr, "Error: Initial menu node not found.\n");
    return NULL; /* Exit if the initial node is invalid */
  }

  while (keep_running) {
    if (client->message.input_index == -1) {
      client->message.input_index = 0;
    }

    // Display the current menu
    client_menu_display(current_menu, *client, client->message.input_index);

    // Process user input
    client_menu_process_input(client, current_menu,
                              &client->message.input_index);

    /* Check if the selected submenu has changed */
    if (client->data.type != current_menu->index) {
      int submenu_found = 0;  // Flag to track found submenu

      for (int i = 0; i < current_menu->submenu_count; i++) {
        if (current_menu->submenu[i]->index == client->data.type) {
          submenu_found = 1;  // Set the found submenu flag

          if (current_menu->action_data.action) {
            client->message.input_index =
                -1;  // Reset the input index for the next cycle
            menu_find_node_by_index(menu_tree, client->data.type)
                ->action_data.action(client);  // Execute action
          } else {
            fprintf(
                stderr,
                "Warning: No action associated with the selected menu node.\n");
          }
          break;  // Exit the loop after executing the action
        }
      }

      if (!submenu_found) {
        fprintf(stderr,
                "Warning: Selected submenu not found in current menu.\n");
      }

      client->message.input_index =
          -1;  // Reset the input index for the next cycle
      current_menu = menu_find_node_by_index(
          menu_tree, client->data.type);  // Update the current menu

      client_termio_set_nonblock();
    }
  }

  return NULL;
}
