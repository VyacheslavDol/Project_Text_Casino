#ifndef MENU_CLIENT_H
#define MENU_CLIENT_H

#include <netinet/in.h>

#define MAX_SUBMENUS 10
#define MAX_BUFFER 1024

struct MenuAction {
  char name[MAX_BUFFER];   /* Action name */
  void *(*action)(void *); /* Action function pointer */
};

struct MenuNode {
  int index;                     /* Current submenu index */
  char name[MAX_BUFFER];         /* Menu item name */
  struct MenuAction action_data; /* Menu action data */
  struct MenuNode **submenu;     /* Array of submenu pointers */
  int submenu_count;             /* Number of submenu items */
};

extern struct MenuNode *menu_tree;
extern volatile __sig_atomic_t keep_running;

struct MenuNode *menu_load_from_file(const struct MenuAction *action_list);

/* Find a node by index in the menu tree */
struct MenuNode *menu_find_node_by_index(struct MenuNode *root, int index);

/* Find a parent node by index in the menu tree */
struct MenuNode *menu_find_parent_by_index(struct MenuNode *root,
                                           int child_index);

/* Free memory for a menu node and its submenus */
int menu_free(struct MenuNode *node);

#endif