#include "../bl_j_basic_client.h"
#include "../bl_j_func.h"
#include "../bl_j_interface.h"
#include "../error.h"
#include "../input_char.h"
#include <netinet/in.h>
#include <unistd.h>

void loop_client_black_jack(msg_room *rcv_msg, place_t *place, int socket)
{   
    int selected = 0;
    int msg_code = 0;

    int flag = 1;
    
    msg_place snd_msg = {0};
    msg_place *snd_msg_p = &snd_msg;
    msg_info_room info_room = {0};
    //int room_id = rcv_msg->room.id;

    struct termios oldattr;
    init_menu(&oldattr);
    
    switch (rcv_msg->msg_code)
    {
        case CHOICE_ROOM:
            set_echo_mode(1);
            set_cursor_mode(1);
            conio_mode(1);

            for(int i = 0; i < NUM_PLACE; i++)
            {
                printf("%d. Room #%d (%d/%d) [%s]\n", i + 1, i, ((msg_info_room *)rcv_msg)->num[i], NUM_PLACE,
                       (((msg_info_room *)rcv_msg)->num[i] == NUM_PLACE) ? RED "full" RESET : (((msg_info_room *)rcv_msg)->access[i] == GAME_IN_PROCCESS) ? RED "in game" RESET\
                                                                                                                                                          : GREEN "open" RESET);
            }
                printf("%d. Refresh\n", NUM_PLACE + 1);

            if(((msg_info_room*)rcv_msg)->flag == 1)
                printf(YELLOW"Room is unreacheable\n"RESET);

            int ans = 0;
            
            do
            {
                printf("Choose room: ");
                ans = get_ans_int();
            } while(ans <= 0 || ans > NUM_PLACE + 1);
            conio_mode(0);
            if(ans == NUM_PLACE + 1)
            {   
                clean_tty();
                snd_msg_p->msg_code = BLACK_JACK_GAME;
                CHECK_ERROR(send(socket, (void*)snd_msg_p, sizeof(msg_place), 0), -1);
                break;
            }
            
            info_room.choice_room = ans - 1;
            info_room.code_msg = REPLY_CHOICE_ROOM;
            CHECK_ERROR(send(socket, (void*)&info_room, sizeof(msg_info_room), 0), -1);
            
            break;
        
        case PLACE_ID_MSG:
            
            clean_tty();

            selected = choice_menu(MENU_5_SIZE, MENU_5_CODE, NULL, 0);

            if(selected == 0)
            {
                msg_code = READY;
                place->status = GAME_IN_PROCCESS;
            }
            else if(selected == 1)
            {   
                msg_code = LEAVE_ROOM;
            }
            send_msg_place(socket, place, rcv_msg->id_place, rcv_msg->room.id, msg_code);
            clean_tty();
            printf("Room #%d\nMy place is #%d\nWaiting for start..\n", rcv_msg->room.id, rcv_msg->id_place);
            break;
            
        case NEW_PLACE_ID_MSG_BL:
            clean_tty();
            printf("Room #%d\nMy place is #%d\nWaiting for start..\n",
                   rcv_msg->room.id, rcv_msg->id_place);
            
            break;

        case WAIT_ALL:
            clean_tty();
            printf("Room #%d\nMy place is #%d\nWaiting for start..\n",
                   rcv_msg->room.id, rcv_msg->id_place);
            break;

        case END_ROUND:
            
            print_interface_bl_j(&rcv_msg->room, get_num_player(rcv_msg->room.place), rcv_msg->id_place);
            printf("\nRound is over\n Result %s, Your balance = %d\n<Press any key to continue>\n", \
                                    (place->win_status == 1) ? GREEN"win"RESET : (place->win_status == 2) \
                                            ? YELLOW"draw"RESET : RED"lose"RESET, place->client.balance);
            getchar();
            //init_menu(&oldattr);
            clean_tty();
            selected = choice_menu(MENU_4_SIZE, MENU_4_CODE, NULL, 0);  //leave/stay

            if(selected == 0)
            {   
                info_room.choice_room = rcv_msg->room.id; /* ROOM_ID */
                info_room.flag = 0;
                info_room.code_msg = REPLY_CHOICE_ROOM;
                CHECK_ERROR(send(socket, (void*)&info_room, sizeof(msg_info_room), 0), -1); 
            }
            else if(selected == 1)
            {
                snd_msg_p->msg_code = BLACK_JACK_GAME;
                CHECK_ERROR(send(socket, (void*)snd_msg_p, sizeof(msg_place), 0), -1);
            }

            break;
        
        default:

            init_menu(&oldattr);

            int place_id = rcv_msg->id_place;
            //int room_id;
            printf("code rcv = %d\n, ", rcv_msg->msg_code);
            //sleep(2);
            *place = rcv_msg->room.place[place_id]; // Обноваление поля своего place
    
            if (place_id == rcv_msg->id_place)  // запрос к нам от сервера
            {
                /* Рекция на запросы */
                switch (rcv_msg->msg_code)
                {  
                    // Стартовая ставка
                case REQUEST_START_BET: 

                    reply_start_bet(&rcv_msg->room, place, place_id);
                    send_msg_place(socket, place, place_id, rcv_msg->room.id, REPLY_START_BET);
                    printf("\033[H\033[J"); // Очистка экрана
                    print_interface_bl_j(&rcv_msg->room, get_num_player(rcv_msg->room.place), place_id);
                    break;

                case REQUEST_SELECT_1:
                    printf("\033[H\033[J"); // Очистка экрана
                    selected = choice_menu(MENU_1_SIZE, MENU_1_CODE, &rcv_msg->room, place_id);
                    place->next_insur_giveup = selected + 1;

                    if (place->next_insur_giveup == END_GAME)    // Сдача
                    {
                        give_up(place);
                    }
                    else if (place->next_insur_giveup == INSURANCE)   // Страховка
                    {
                        get_bet(place, INSURANCE_BET);
                    }
                    send_msg_place(socket, place, place_id, rcv_msg->room.id, REPLY_SELECT_1);
                    break;

                case REQUEST_SELECT_2:
                    do{
                        printf("\033[H\033[J"); // Очистка экрана
                        selected = choice_menu(MENU_2_SIZE, MENU_2_CODE, &rcv_msg->room, place_id);
                        if(selected == 2 && place->client.balance < place->pl_bet.start)
                        {
                            printf("Too low balance\n");
                            flag = 1;
                        }
                        else if(selected == 3)
                        {
                            printf("This action is not supported\n");
                            flag = 1;
                        }
                        else
                        {
                            flag = 0;
                        }
                    } while(flag);
                    flag = 1;              
                    if (selected == 0)
                    {
                        msg_code = MORE;
                    }
                    else if (selected == 1)
                    {
                        msg_code = ENOUGH;
                    }
                    else if (selected == 2)
                    {   
                        msg_code = DOUBLE;
                    }
                    else if (selected == 3)
                    {
                        msg_code = SPLIT;
                    }

                    send_msg_place(socket, place, place_id, rcv_msg->room.id, msg_code);
                        break;

                    default:
                        printf("\033[H\033[J"); // Очистка экрана
                        print_interface_bl_j(&rcv_msg->room, get_num_player(rcv_msg->room.place), place_id);
                        break;
                    }
            }

    

            break; // Главного switch
    }
        close_menu(&oldattr);
    return;
}