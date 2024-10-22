#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <signal.h>
#include <strings.h>
#include <fcntl.h>
#include <sys/time.h>
#include <errno.h>
#include <termios.h>
#include "../bl_j_interface.h"
#include "../error.h"
#include "../bl_j_basic_client.h"
#include "../bl_j_struct.h"
#include "../bl_j_func.h"

#define h_addr  h_addr_list[0]  /* для совместимости с предыдущими версиями */
#define FILENAME "file_client"
#define END_FILE "end of file"
#define BUFF_FILE 64
#define EXIT_MSG "400"
#define SIZE_BUFF 100
#define IP_ADDRESS "127.0.0.1"

volatile int exit_flag = 1;

void exit_func()
{
    exit_flag = 0;
}

struct termios oldattr;

int main(int argc, char *argv[])
{
    int my_sock, portno;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buff[sizeof(msg_room)];
    signal(SIGINT, exit_func);

    /* printf("print <start> to start\n"); */
    
    if (argc < 2) 
    { 
        fprintf(stderr, "usage %s port\n",
        argv[0]);
        exit(0);
    }

    // извлечение порта
    portno = atoi(argv[1]);

    // Шаг 1 - создание сокета
    CHECK_ERROR((my_sock = socket(AF_INET, SOCK_STREAM, 0)), -1);

    // извлечение хоста
    CHECK_ERROR((server = gethostbyname(IP_ADDRESS)), NULL);

    // заполнение структуры serv_addr
    bzero((char*) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char*)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    
    // установка порта
    serv_addr.sin_port = htons(portno);

    // Шаг 2 - установка соединения
    CHECK_ERROR(connect(my_sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)), -1);


    // Шаг 3 - чтение и передача сообщений

    int selected;
    struct termios oldattr;
    msg_place msg_send = {0};
    init_menu(&oldattr);

    place_t place = {0};

    selected = choice_menu(MENU_3_SIZE, MENU_3_CODE, NULL, 0);

    if(selected == 0)
    {
        msg_send.msg_code = BLACK_JACK_GAME;
        CHECK_ERROR(send(my_sock, (void*)&msg_send, sizeof(msg_place), 0), -1);
    }
    else if(selected == 1)
    {
        printf("закрытие сессии\n");
        close(my_sock);
        close_menu(&oldattr);
        exit(EXIT_SUCCESS);
    }
    
    int val_read;

    while(exit_flag)
    {
        memset(buff, 0, sizeof(buff));

        CHECK_ERROR((val_read = recv(my_sock, (void*)buff, sizeof(buff), 0)), -1);
        if(val_read == 0)
        {
            clean_tty();
            printf("Server shutdow\n");
            break;
        }
/*            printf("VAL_READ = %d, RCV code %d, go loop\n", val_read, ((msg_room*)buff)->msg_code);
        sleep(5); */

        loop_client_black_jack((msg_room*)buff, &place, my_sock);

    }

    send_msg_place(my_sock, &((msg_room*)buff)->room.place[((msg_room*)buff)->id_place], ((msg_room*)buff)->id_place, ((msg_room*)buff)->room.id, DISCONNECT_PLAYER);

    printf("закрытие сессии\n");
    conio_mode(1);
    set_cursor_mode(1);
    set_echo_mode(1);
    close_menu(&oldattr);
    close(my_sock);
    exit(EXIT_SUCCESS);
}

