#ifndef CLIENT_CHAT_H_
#define CLIENT_CHAT_H_

#include <stdbool.h>

#include "../../struct/main_message_strcut.h"
#include "../base/base.h"

extern struct MainMessage send_msg;

void client_chat_clear_screen_and_prompt(void);
void client_chat_handle_escape_sequence(struct Client *client);
void client_chat_handle_message_sending(struct Client *client);
void client_chat_append_character_to_message(struct Client *client, char ch);
void *client_chat_handle(void *data);

#endif  // CLIENT_CHAT_H_
