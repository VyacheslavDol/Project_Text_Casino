#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "../header/error.h"
#include "../header/main_message_struct.h"
#include "../header/roul_basic_server.h"
#include "../header/input_char.h"
#include "../header/error.h"

int main(int argc, char *argv[]) {
  // ошибка в случае если мы не указали порт
  if (argc < 2) {
    perror("argv");
    return (EXIT_FAILURE);
  }

  int valread;
  int portno;                   // номер порта
  int sockfd, newsockfd, sock;  // дескрипторы сокетов
  socklen_t clilen;  // размер адреса клиента типа socklen_t
  struct sockaddr_in serv_addr, cli_addr;  // структура сокета сервера и клиента
  struct hostent *hst;
  struct MainMessage buff = {0};
  fd_set master;  // набор всех имеющих дескрипторов
  fd_set read_fds;  // дескрипторы на чтение(что-то куда-то пришло)

  // Шаг 1 - создание сокета
  CHECK_ERROR((sockfd = socket(AF_INET, SOCK_STREAM, 0)), -1);

  // Шаг 2 - связывание сокета с локальным адресом
  bzero((char *)&serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr =
      INADDR_ANY;  // сервер принимает подключения на всех доступных IP, (сервер
                   // имеет множество IP адресов и один конкретный порт)
  portno = atoi(argv[1]);
  serv_addr.sin_port = htons(portno);  // порт

  CHECK_ERROR(bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)),
              -1);

  listen(sockfd, 5);

  clilen = sizeof(cli_addr);

  FD_ZERO(&master);
  FD_ZERO(&read_fds);

  FD_SET(sockfd, &master);  // добавление слушающего сокета в набор master
  int FD_MAX = sockfd;  // максимальное значение дескриптора

  room_roul_t room[NUM_ROOM_ROUL] = {0};

  for (int i = 0; i < NUM_ROOM_ROUL; i++) {
    init_room(&room[i], i);
    printf("Room id = %d, room_status %d\n", room[i].id, room->status);
  }

  int room_id = 0;  // id комнаты с которой будем рабатать

  msg_place_roul_t msg_rcv = {0};

  while (1) {
    read_fds = master;

    CHECK_ERROR(select(FD_MAX + 1, &read_fds, NULL, NULL, NULL), -1);

    for (int i = 0; i <= FD_MAX; i++) {
      if (FD_ISSET(i, &read_fds)) {
        if (i == sockfd) /* Присоединение нового клиента */
        {
          CHECK_ERROR((newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr,
                                          &clilen)), -1);

          hst = gethostbyaddr((char *)&cli_addr.sin_addr, 4, AF_INET);
          printf("+%s [%s] new connect! With socket = %d\n",
                 (hst) ? hst->h_name : "Unknown host",
                 inet_ntoa(cli_addr.sin_addr), newsockfd);
          FD_SET(newsockfd,
                 &master);  // добавление нового дескриптора в общий набор
          if (newsockfd > FD_MAX) FD_MAX = newsockfd;
        } else  // изменение на клиентском сокете
        {
          /* Получение сообщения */
          memset(&buff, 0, sizeof(struct MainMessage));
          sock = i;

          CHECK_ERROR((valread = recv(sock, &buff, sizeof(buff), 0)),
                      -1);
          if (valread == 0) {

          /* Поиск откуда отключился */
            int j = 0;
            int flag = 1;
            do {
              if (j == NUM_ROOM_ROUL) break;
              for (int i = 0; i < MAX_PLACE_ROUL; i++) {
                if (room[j].place[i].player.socket == sock) {
                    ((msg_place_roul_t*)buff.data)->msg_code = DISCONNECTED_ROUL;
                    ((msg_place_roul_t*)buff.data)->place_id = i;
                    ((msg_place_roul_t*)buff.data)->room_id = j;
                  flag = 0;
                }
              }
              j++;
            } while (flag);

            printf("Клиент отключен\n");
            FD_CLR(sock, &master);
            close(sock);
          }

          msg_rcv = *(msg_place_roul_t *)buff.data;

          /* ПРИНЯТИЕ СООБЩЕНИЯ  АНАЛИЗ ТОГО ЧТО ПРИШЛО */

          printf("Recv: Code mes %d, socket = %d\n", msg_rcv.msg_code, sock);

          if (msg_rcv.msg_code == DISCONNECT_PLAYER_ROUL) {
            FD_CLR(sock, &master);
            close(sock);
          }

          if (msg_rcv.msg_code == REPLY_CHOICE_ROOM_ROUL) {
            room_id = ((msg_info_room_roul_t *)&msg_rcv)
                          ->choice_room;  // Выбор комнаты
          } else {
            room_id = msg_rcv.room_id;
          }

          loop_server_roulette(&room[room_id], &msg_rcv, sock);
        }
      }
    }
  }

  return 0;
}