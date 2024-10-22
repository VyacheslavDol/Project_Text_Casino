#include "../bl_j_func.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <termio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "../error.h"
#include "../bl_j_interface.h"
#include "../input_char.h"
/* Вспомогательные функции */

void init_player(player_t *plp, int pl_socket)
{   
    static int i = 0;
    plp->balance = 100;
    plp->id = i++;    
    plp->socket = pl_socket;
    strcpy(plp->name, "client_name");   // временное решение
}

void init_deck(card_t deck[])
{
    int index = 0;
    for (int suit = 0; suit < NUM_SUITS; suit++) 
    {
        for (int rank = 1; rank < NUM_RANKS; rank++) 
        {
            deck[index].suit = suit;
            deck[index].rank = rank;
            index++;
        }
    }
    shaffle_deck(deck);
}

void shaffle_deck(card_t deck[])
{   
    srand(time(NULL));
    for(int k = 0; k < 5; k++)
    {
        for (int i = NUM_CARDS - 1; i > 0; i--) 
        {
            int j = rand() % (i + 1);
            card_t temp = deck[i];
            deck[i] = deck[j];
            deck[j] = temp;
        }
    }
}

void get_bet(place_t *place_p, int bet_type)
{   
    char buff[MAX_SYMBOLS_BET];
    int bet = 0;

    // проверка на то, что ставка меньше или равна балансу
    if (bet_type == START_BET)
    {   
        do
        {   
            input_char(buff, sizeof(buff));     // заменится на получение из сообщения
            bet = atoi(buff);
        } while (bet <= 0 || bet > place_p->client.balance);
        place_p->pl_bet.start = bet;
        place_p->client.balance -= place_p->pl_bet.start;
    }
    else if (bet_type == INSURANCE_BET)
    {   
        place_p->pl_bet.insurance = place_p->pl_bet.start;
        place_p->client.balance -= place_p->pl_bet.insurance;
    }
}

void init_room(room_t *room, int id)
{
    room->id = id;        
    room->game_status = GAME_WAITING_START;
}

int card_to_place(card_t deck[], place_t *place_p, int hand, int card_account)
{   
    // счетчик карт
    static int k = 0;

    if (hand == FIRST_HAND)
    {
        place_p->hand_card.first_hand[card_account] = deck[k];
    }
    else if (hand == SECOND_HAND)
    {
        place_p->hand_card.second_hand[card_account] = deck[k];
    }
    
    k++;
    if (k == NUM_CARDS)
    {
        shaffle_deck(deck);
        k = 0;
    }

    return NUM_CARDS - k;
}

const char* card_to_string(card_t card) {
    const char* ranks[] = {"0", "A", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K"};
    const char* suits[] = {"Hearts", "Diamonds", "Clubs", "Spades"};
    
    static char buffer[20];
    snprintf(buffer, sizeof(buffer), "%s %s", ranks[card.rank], suits[card.suit]);
    return buffer;
}


int calc_sum(place_t* place)
{
    int sum = 0;
    int aces_count = 0;
    int rank = 0;

    // Подсчет стоимости карт
    for (int i = 0; (rank = place->hand_card.first_hand[i].rank) != 0; i++) 
    {   
        if ((rank >= 2) && (rank <= 10))
        {
            sum += rank;
        }
        else if (rank > 10)
        {
            sum += 10;
        }
        else
        {
            aces_count++;
        }
    }

    // Добавление стоимости тузов
    for(int i = 0; i < aces_count; i++)
    {
        if((sum + 11) <= 21)
        {
            sum += 11;
        }
        else
        {
            sum += 1;
        }
    }

    return sum;
}

int more(place_t* place, card_t *deck, int hand)
{   
    int card_account = 0;
    if (hand == FIRST_HAND)
    {
        card_account = get_card_account(place->hand_card.first_hand);
    }
    else if(hand == SECOND_HAND)
    {
        card_account = get_card_account(place->hand_card.second_hand);
    }
    
    card_to_place(deck, place, hand, card_account);
    place->sum[FIRST_HAND] = calc_sum(place);
    if (place->sum[FIRST_HAND] > 21)
    {
        place->pl_bet.start = 0;
        place->win_status = 3;
        place->status = END_GAME;
        return 1;
    }
    else
    {   
        return 0;
    }
}

int get_card_account(card_t *card_hand)
{
    int count = 0;
    while(card_hand[count].rank != 0)
    {
        count++;
    }
    return count;
}

int double_bet(place_t *place, card_t *deck, int hand)
{   
    int card_account = 0;
    if (hand == FIRST_HAND)
    {
        card_account = get_card_account(place->hand_card.first_hand);
    }
    else if(hand == SECOND_HAND)
    {
        card_account = get_card_account(place->hand_card.second_hand);
    }

    if (place->pl_bet.start <= place->client.balance)
    {
        place->client.balance -= place->pl_bet.start;
        place->pl_bet.start *= 2;
    }
    else
    {
        return -1;
    }

    card_to_place(deck, place, hand, card_account);
    place->sum[FIRST_HAND] = calc_sum(place);

    place->status = END_GAME;

    if (place->sum[hand] > 21)
    {
        place->pl_bet.start = 0;
        return 1;
    }
    else
    {   
        return 0;
    }
}

void give_up(place_t *place)
{
    place->client.balance += place->pl_bet.start / 2;
    place->pl_bet.start = 0;
    place->win_status = 4;
    place->status = END_GAME;
}

void check_21(place_t *place_player, place_t *place_diler)
{      
    int val;

    if ((place_diler->sum[FIRST_HAND] == 21))         
    {                                                   //      ничья                     поражение
        val = (place_player->sum[FIRST_HAND] == 21) ? place_player->pl_bet.start : place_player->pl_bet.insurance * 2;
        place_player->client.balance += val;
        place_player->pl_bet.start = 0;
        place_player->win_status = (val == place_player->pl_bet.start) ? 2 : 3;
        place_player->status = END_GAME;
    }
    else if (place_diler->sum[0] < 21 && place_player->sum[0] == 21)     // победа
    {
        place_player->client.balance += place_player->pl_bet.start * 3;   // НА 2.5 по правилам, но int
        place_player->pl_bet.start = 0;
        place_player->win_status = 1;
        place_player->status = END_GAME;
    }
    else if (place_diler->sum[0] < 21 && place_player->sum[0] < 21)
    {
        place_player->pl_bet.insurance = 0;  // сгорание страховки
    }
}

void enough(place_t *place)
{
    place->status = END_GAME;
}

int diler_end_game(place_t *place_diler, int stop_number, card_t *deck, int card_account)
{
    if (place_diler->sum[0] >= stop_number)
    {
        place_diler->status = END_GAME;
        return 1;
    }
    else
    {
        card_to_place(deck, place_diler, FIRST_HAND, card_account);
        place_diler->sum[FIRST_HAND] = calc_sum(place_diler);
        return 0;
    }    
}

int get_num_player(place_t *place)
{
    int count = 0;

    for (int i = 0; i < NUM_PLACE; i++)
    {
        if (place[i].client.socket != 0 || place[i].status == DISCONNECTED)
        {
            count++;
        }
    }

    return count;
}

void give_cards(room_t *room, int hand, int card_account)
{
    for (int i = 0; i < room->num_player; i++)
    {
        card_to_place(room->deck, &(room->place[i]), hand, card_account);
    }   
        card_to_place(room->deck, &room->place[DILER_PLACE], hand, card_account);
    
}

int get_num_act_pl(room_t *room)
{   
    int count = 0;
    for (int i = 0; i < room->num_player; i++)
    {
        if (room->place[i].status == GAME_IN_PROCCESS)
        {
            count++;
        }
    }
    return count;
}

int get_num_cards(card_t *cards)
{
    int count = 0;
    while(cards[count].rank != 0)
    {
        count++;
    }
    return count;
}

void reply_start_bet(room_t *room, place_t *place, int my_place_id)
{
    printf("\033[H\033[J"); // Очистка экрана
    print_interface_bl_j(room, get_num_player(room->place), my_place_id);
    set_cursor_mode(1);
    set_echo_mode(1);
    conio_mode(1);
    printf("Start bet: ");

    get_bet(place, START_BET);

    conio_mode(0);
    set_cursor_mode(0);
    set_echo_mode(0);

}

void send_msg_place(int my_sock, place_t *place, int place_id, int room_id, int code_msg)
{
    msg_place msg = {0};
    msg.place = *place;
    msg.id_place = place_id;
    msg.msg_code = code_msg;
    msg.id_room = room_id;
    CHECK_ERROR(send(my_sock, (void*)&msg, sizeof(msg_place), 0), -1);
}

void send_msg_room(int socket, room_t *room, int place_id, int code_msg)
{   
    int bytes;
    msg_room msg;
    msg.room = *room;
    msg.id_place = place_id;
    msg.msg_code = code_msg;
    if(socket != 0) 
        bytes = send(socket, (void*)&msg, sizeof(msg_room), 0);
    if(bytes < 0)
    {   
        fprintf(stderr, "Error sending message: %s  %d \n", strerror(errno), errno);
    }
    printf("SEND bytes %d,  Code = %d, socket = %d\n", bytes, code_msg, socket);
}