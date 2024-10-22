#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/select.h>
#include <strings.h>
#include <string.h>

#include "../input_char.h"
#include "../error.h"
#include "../bl_j_func.h"
#include "../bl_j_basic_server.h"
#include "../bl_j_struct.h"

int main(int argc, char *argv[])
{
        // ошибка в случае если мы не указали порт
    if (argc < 2) 
    {
        perror("argv");
        return(EXIT_FAILURE);
    }

    int valread;
    int portno; // номер порта
    int sockfd, newsockfd, sock; // дескрипторы сокетов
    socklen_t clilen; // размер адреса клиента типа socklen_t
    struct sockaddr_in serv_addr, cli_addr; // структура сокета сервера и клиента
    struct hostent *hst;    
    char buff[1024];

    fd_set master; //набор всех имеющих дескрипторов
    fd_set read_fds; //дескрипторы на чтение(что-то куда-то пришло) 


    // Шаг 1 - создание сокета
    CHECK_ERROR((sockfd = socket(AF_INET, SOCK_STREAM, 0)) , -1);

    // Шаг 2 - связывание сокета с локальным адресом
    bzero((char*) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY; // сервер принимает подключения на всех доступных IP, (сервер имеет множество IP адресов и один конкретный порт)
    portno = atoi(argv[1]);
    serv_addr.sin_port = htons(portno); //порт

    CHECK_ERROR(bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)), -1);

    listen(sockfd, 5);

    clilen = sizeof(cli_addr);

    FD_ZERO(&master);
    FD_ZERO(&read_fds);

    FD_SET(sockfd, &master); //добавление слушающего сокета в набор master
    int FD_MAX = sockfd;    //максимальное значение дескриптора

    room_t room[NUM_ROOM] = {0};   // + 1 -место диллера 

    for(int i = 0; i < NUM_ROOM; i++)
    {
        init_room(&room[i], i);
    }
    
    int room_id = 0; // id комнаты с которой будем рабатать

    msg_place msg_rcv = {0};

    while(1)
    {   
        read_fds = master;

        CHECK_ERROR(select(FD_MAX + 1, &read_fds, NULL, NULL, NULL), -1);

        for(int i = 0; i <= FD_MAX; i++)
        {
            if(FD_ISSET(i, &read_fds))
            {

                if(i == sockfd)     /* Присоединение нового клиента */
                {
                    CHECK_ERROR((newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen )), -1);

                    hst = gethostbyaddr((char *)&cli_addr.sin_addr, 4, AF_INET);
                    printf("+%s [%s] new connect! With socket = %d\n", (hst) ? hst->h_name : "Unknown host", inet_ntoa(cli_addr.sin_addr), newsockfd);
                    FD_SET(newsockfd, &master); //добавление нового дескриптора в общий набор
                    if(newsockfd > FD_MAX) FD_MAX = newsockfd;
                }
                else    //изменение на клиентском сокете
                {
                    /* Получение сообщения */
                    memset(buff, 0, sizeof(buff));
                    sock = i;                    

                    CHECK_ERROR((valread = recv(sock, (void*)buff, sizeof(buff), 0)), -1);
                    if (valread <= 0)
                    {   
                        int j = 0;
                        int flag = 1;
                        do
                        {   
                            for(int i = 0; i < NUM_PLACE; i++)
                            {
                                if(room[j].place[i].client.socket == sock)
                                {   
                                    if(room[j].game_status == GAME_WAITING_START)
                                        ((msg_place *)buff)->msg_code = LEAVE_ROOM;
                                    else
                                        ((msg_place *)buff)->msg_code = DISCONNECT_PLAYER;
                                    ((msg_place *)buff)->id_place = i;
                                    ((msg_place *)buff)->id_room = j;
                                    flag = 0;
                                }
                                j++;
                                if(j == NUM_ROOM)
                                    break;
                            }
                        }while (flag);

                        printf("Клиент отключен\n");
                        FD_CLR(sock, &master);
                        close(sock);
                    }

                    msg_rcv = *((msg_place *)buff);

                    printf("Recv: Code mes %d, socket = %d, place id [%d]\n", ((msg_place *)buff)->msg_code, sock, ((msg_place *)buff)->id_place);
    

                    if(msg_rcv.msg_code == REPLY_CHOICE_ROOM)
                    {
                        room_id = ((msg_info_room*)buff)->choice_room;
                        printf("CHOICE ROOM = %d\n", ((msg_info_room*)buff)->choice_room);
                    }
                    else
                    {
                        room_id = msg_rcv.id_room;
                    }

                    loop_server_black_jack(&room[room_id], &msg_rcv, sock);
                }
            }
        }
    }

    return 0;
}