#include "client/client.h"
#include "components/chat/chat.h"
#include "components/game_component.h"
#include "components/menu/menu.h"
#include "struct/menu_struct.h"

#define NAME_OF(name) #name

volatile sig_atomic_t keep_running = 1;
struct MenuNode *menu_tree = NULL;

int main() {
  save_terminal_settings();
  printf("\033[H\033[J");
  const struct MenuAction action_list[] = {
      {NAME_OF(client_menu_handle), client_menu_handle},
      {NAME_OF(slot_machine), slot_machine},
      {NAME_OF(bl_j_run), bl_j_run},
      {NAME_OF(roulette), roulette},
      {NAME_OF(client_chat_handle), client_chat_handle},
      {NAME_OF(logout_service), logout_service},
      {NAME_OF(exit_game), exit_game},
      {"\0", NULL}};

  /* Initialize the menu */
  menu_tree = menu_load_from_file(action_list);

  /* Check if menu loading was successful */
  if (menu_tree == NULL) {
    fprintf(stderr, "Error: Failed to load menu from JSON file\n");
    return EXIT_FAILURE;
  }

  client();
  menu_free(menu_tree);
  return 0;
}