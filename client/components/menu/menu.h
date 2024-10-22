#ifndef CLIENT_MENU_H_
#define CLIENT_MENU_H_

#include "../../struct/auth_struct.h"
#include "../base/base.h"

extern int balance;

/* Function to display the current menu and highlight the selected item */
void client_menu_display(const struct MenuNode *current_menu,
                         struct Client client, int selected_index);

/* Function to handle user input for navigating the menu */
void client_menu_handle_input(struct Client *client,
                              const struct MenuNode *current_menu,
                              int *selected_index);

/* Function to process user input with a timeout */
void client_menu_process_input(struct Client *client,
                               const struct MenuNode *current_menu,
                               int *selected_index);

/* Thread function to handle the client menu interaction */
void *client_menu_handle(void *data);

#endif  // CLIENT_MENU_H_
