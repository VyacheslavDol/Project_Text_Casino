#ifndef CLIENT_TERMIO_H_
#define CLIENT_TERMIO_H_

void disable_echo();
void enable_echo();
void get_password(char *password, int max_len);
/* Set terminal to non-blocking mode */
void client_termio_set_nonblock(void);
void client_termio_reset(void);

void restore_terminal_settings();
void save_terminal_settings();

#endif  // CLIENT_TERMIO_H_
