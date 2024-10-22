#include <cjson/cJSON.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../struct/menu_struct.h"

struct MenuNode *menu_deserialize(cJSON *json,
                                  const struct MenuAction *action_list) {
  if (!json) return NULL;

  struct MenuNode *node = (struct MenuNode *)malloc(sizeof(struct MenuNode));
  if (!node) return NULL;

  /* Fill in the fields of the structure */
  const cJSON *index_item = cJSON_GetObjectItem(json, "index");
  if (index_item) {
    node->index = index_item->valueint;
  } else {
    fprintf(stderr, "Error: 'index' not found in JSON\n");
    free(node);
    return NULL;
  }

  const cJSON *name_item = cJSON_GetObjectItem(json, "name");
  if (name_item && name_item->valuestring) {
    strncpy(node->name, name_item->valuestring, sizeof(node->name) - 1);
    node->name[sizeof(node->name) - 1] = '\0';  // Ensure null termination
  } else {
    fprintf(stderr, "Error: 'name' not found in JSON\n");
    free(node);
    return NULL;
  }

  const cJSON *action_name_item = cJSON_GetObjectItem(json, "action_name");
  if (action_name_item && action_name_item->valuestring) {
    strncpy(node->action_data.name, action_name_item->valuestring,
            sizeof(node->action_data.name) - 1);
    node->action_data.name[sizeof(node->action_data.name) - 1] =
        '\0'; /* Ensure null termination of the string */

    /* Find the corresponding function pointer from action_list */
    for (size_t i = 0; strnlen(action_list[i].name, MAX_BUFFER) != 0; i++) {
      if (strncmp(node->action_data.name, action_list[i].name, MAX_BUFFER) ==
          0) { /* Compare action names */
        node->action_data.action =
            action_list[i]
                .action; /* Assign the corresponding function pointer */
        break;
      }
    }
  } else {
    fprintf(stderr, "Error: 'action_name' not found in JSON\n");
    free(node);
    return NULL;
  }

  const cJSON *submenu_count_item = cJSON_GetObjectItem(json, "submenu_count");
  if (submenu_count_item) {
    node->submenu_count = submenu_count_item->valueint;
  } else {
    fprintf(stderr, "Error: 'submenu_count' not found in JSON\n");
    free(node);
    return NULL;
  }

  /* Allocate memory for the array of submenus */
  node->submenu = (struct MenuNode **)malloc(node->submenu_count *
                                             sizeof(struct MenuNode *));
  if (!node->submenu) {
    fprintf(stderr, "Error: Failed to allocate memory for submenus\n");
    free(node);
    return NULL;
  }

  /* Fill in submenus recursively */
  cJSON *submenu_json = cJSON_GetObjectItem(json, "submenu");
  if (!cJSON_IsArray(submenu_json)) {
    fprintf(stderr, "'submenu' is not an array or not found\n");
    free(node->submenu);
    free(node);
    return NULL;
  }

  for (int i = 0; i < node->submenu_count; i++) {
    cJSON *submenu_item = cJSON_GetArrayItem(submenu_json, i);
    if (submenu_item) {
      node->submenu[i] = menu_deserialize(submenu_item, action_list);
      if (!node->submenu[i]) {
        fprintf(stderr, "Error deserializing submenu item at index %d\n", i);
        /* Free previously allocated submenus */
        for (int j = 0; j < i; j++) {
          free(node->submenu[j]);
        }
        free(node->submenu);
        free(node);
        return NULL;
      }
    } else {
      fprintf(stderr, "'submenu' item at index %d not found\n", i);
      free(node->submenu);
      free(node);
      return NULL;
    }
  }

  return node;
}

struct MenuNode *menu_load_from_file(const struct MenuAction *action_list) {
  char *data =
      "{"
      "\"index\": 0,"
      "\"name\": \"Main Menu\","
      "\"action_name\": \"client_menu_handle\","
      "\"submenu_count\": 6,"
      "\"submenu\": ["
      "{"
      "\"index\": 4,"
      "\"name\": \"\U0001F0CF Blackjack Game\","
      "\"action_name\": \"bl_j_run\","
      "\"submenu_count\": 0,"
      "\"submenu\": []"
      "},"
      "{"
      "\"index\": 5,"
      "\"name\": \"\U0001F534 Roulette Game\","
      "\"action_name\": \"roulette\","
      "\"submenu_count\": 0,"
      "\"submenu\": []"
      "},"
      "{"
      "\"index\": 12,"
      "\"name\": \"\U0001F3B0 Slot Machine\","
      "\"action_name\": \"slot_machine\","
      "\"submenu_count\": 0,"
      "\"submenu\": []"
      "},"
      "{"
      "\"index\": 20,"
      "\"name\": \"\U0001F4AC Chat\","
      "\"action_name\": \"client_chat_handle\","
      "\"submenu_count\": 0,"
      "\"submenu\": []"
      "},"
      "{"
      "\"index\": 7,"
      "\"name\": \"\u274C Logout\","
      "\"action_name\": \"logout_service\","
      "\"submenu_count\": 0,"
      "\"submenu\": []"
      "},"
      "{"
      "\"index\": 8,"
      "\"name\": \"\U0001F6AA Exit\","
      "\"action_name\": \"exit_game\","
      "\"submenu_count\": 0,"
      "\"submenu\": []"
      "}"
      "]"
      "}";

  cJSON *json = cJSON_Parse(data);

  if (!json) {
    fprintf(stderr, "Error parsing JSON: %s\n", cJSON_GetErrorPtr());
    return NULL;
  }

  /* Deserialize the menu structure from the JSON object. */
  struct MenuNode *menu = menu_deserialize(json, action_list);
  cJSON_Delete(json); /* Free the JSON object */

  if (!menu) {
    fprintf(stderr, "Error deserializing menu\n");
    return NULL;
  }

  return menu;
}

struct MenuNode *menu_find_node_by_index(struct MenuNode *root, int index) {
  /* Check if the root node is NUstruct LL */
  if (!root) {
    fprintf(stderr, "Error: Rootstruct MenuNode is NULL\n");
    return NULL;
  }

  /* Check if the current node matches the index */
  if (root->index == index) {
    return root;
  }

  /* Recursively search through submenus */
  for (int i = 0; i < root->submenu_count; i++) {
    struct MenuNode *found = menu_find_node_by_index(root->submenu[i], index);
    if (found) {
      return found;
    }
  }

  return NULL;
}

struct MenuNode *menu_find_parent_by_index(struct MenuNode *root,
                                           int child_index) {
  /* Check for invalid parameters */
  if (!root || child_index < 0) {
    fprintf(stderr,
            "Error: Invalid parameters - root is NULL or child_index "
            "is negative\n");
    return NULL;
  }

  /* Search for the parent node of the child node */
  for (int i = 0; i < root->submenu_count; i++) {
    if (root->submenu[i]->index == child_index) {
      return root;
    }

    /* Recursively check submenus for the parent */
    struct MenuNode *parent =
        menu_find_parent_by_index(root->submenu[i], child_index);
    if (parent) {
      return parent;
    }
  }

  return NULL;
}

int menu_free(struct MenuNode *node) {
  /* Check fostruct r invalid node */
  if (!node) {
    fprintf(stderr, "Error: Cannot free NULL node\n");
    return -1;
  }

  /* Recursively free all submenus */
  for (int i = 0; i < node->submenu_count; i++) {
    menu_free(node->submenu[i]);
  }

  /* Free allocated memory for submenu array and the node itself */
  free(node->submenu);
  free(node);

  return 0;
}
