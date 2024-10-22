#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>

#include "../header/roul_basic_client.h"
#include "../header/roul_interface.h"
#include "../header/roul_struct.h"
#include "../header/main_message_struct.h"
#include "../header/roul_func.h"
#include "../header/error.h"

#define h_addr h_addr_list[0] /* для совместимости с предыдущими версиями */
#define BUFF_FILE 64
#define SIZE_BUFF 100
#define IP_ADDRESS "127.0.0.1"

struct termios old_attr;

volatile int exit_flag = 1;

void exit_func() { exit_flag = 0; }

static int my_sock, portno;

struct termios oldattr;

int main(int argc, char *argv[]) {
  struct sockaddr_in serv_addr;
  struct hostent *server;

  signal(SIGINT, exit_func);

  if (argc < 2) {
    fprintf(stderr, "usage %s port\n", argv[0]);
    exit(1);
  }

  portno = atoi(argv[1]);

  CHECK_ERROR((my_sock = socket(AF_INET, SOCK_STREAM, 0)), -1);

  // извлечение хоста
  CHECK_ERROR((server = gethostbyname(IP_ADDRESS)), NULL);

  // заполнение структуры serv_addr
  bzero((char *)&serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr,
        server->h_length);

  // установка порта
  serv_addr.sin_port = htons(portno);

  // Шаг 2 - установка соединения
  CHECK_ERROR(
      connect(my_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)), -1);

  // Шаг 3 - чтение и передача сообщений

  int selected;

  struct MainMessage msg_send = {0};

  init_menu(&oldattr);

  place_roul_t place = {0}; // Мое место

  selected = choice_menu(MENU_3_SIZE, MENU_3_CODE, NULL, 0);

  if (selected == 0) {
    
    ((msg_place_roul_t*)msg_send.data)->msg_code = ROULETTE;
    send(my_sock, (void*)&msg_send, sizeof(struct MainMessage), 0);
  } else if (selected == 1) {
    printf("закрытие сессии\n");
    close(my_sock);
    close_menu(&oldattr);
    exit(EXIT_SUCCESS);
  }

  int val_read;

  struct MainMessage rcv_msg = {0};
  while (exit_flag) {
    memset(&rcv_msg, 0, sizeof(struct MainMessage));

    CHECK_ERROR((val_read = recv(my_sock, &rcv_msg, sizeof(struct MainMessage), 0)), -1);
    if (val_read == 0) {
      clean_tty();
      reset_conio_mode(&old_attr);
      printf("Server shutdown\n");
      break;
    }
/*                 printf("VAL_READ = %d, RCV code %d, go loop\n", val_read,
       ((msg_room_roul_t*)rcv_msg.data)->code_msg);
        sleep(3);  */

    loop_client_black_jack((msg_room_roul_t*)rcv_msg.data, &place, my_sock);

  }

  printf("закрытие сессии\n");
  conio_mode(1);
  set_cursor_mode(1);
  set_echo_mode(1);
  close_menu(&oldattr);
  close(my_sock);
  exit(EXIT_SUCCESS);
}
