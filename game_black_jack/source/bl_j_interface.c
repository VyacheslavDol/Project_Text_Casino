#include "../bl_j_interface.h"
#include "../bl_j_func.h"
#include <termios.h>
#include <stdio.h>
#include <unistd.h>

static const char *MENU_[5][4] = {{"Continue", "Insurance", "Give up", "\0"}, 
                                    {"More", "Enough", "Double", "Split"},
                                    {"Black Jack", "Exit", "\0", "\0"},
                                    {"Stay in room", "Leave room"},
                                    {"Ready", "Leave room"}};


char* get_char_status(place_t *place, char *status)
{
    status = (place->status == PLAEYR_PLAY) ? GREEN"in game"RESET :
                (place->status == END_GAME) ? YELLOW"finished"RESET : RED"disconnected"RESET;
    return status; 
}

void print_main_menu(place_t *place, int index, int my_place_id)
{   
    char status[30];

    if(index < DILER_PLACE)
    {
        printf("\nPlace #%d %s socket%d            %s\n First hand: ", index, place->client.name, place->client.socket, (index == my_place_id) ? GREEN"<--ME"RESET : "\0");
        //Вывод карт 1ой руки
        for (int i = 0; place->hand_card.first_hand[i].rank != 0; i++)
        {
            printf("[%s] ", card_to_string(place->hand_card.first_hand[i]));
        }
        //Вывод карт 2ой руки
        if(place->hand_card.second_hand[0].rank != 0)
        {
            printf("\n Second hand: ");
            for (int i = 0; place->hand_card.second_hand[i].rank != 0; i++)
            {
                printf("[%s] ", card_to_string(place->hand_card.second_hand[i]));
            };
        }
        //Ставка
        printf("\n Bet: %d", place->pl_bet.start);
        if (place->next_insur_giveup == INSURANCE)
        {
            printf(" Insurance bet: %d %s", place->pl_bet.insurance, (place->pl_bet.insurance == 0) ? YELLOW"bet lost"RESET : "\0");
        };

        if(index < DILER_PLACE)
        printf("\n Status: %s %d\n", get_char_status(place, status), place->status);
        printf("balance = %d\n", place->client.balance);
    }
    else if(index >= DILER_PLACE)
    {   
        printf("\n%s\n Cards: ", place->client.name);
        int dil_cards = get_num_cards(place->hand_card.first_hand);
        if(dil_cards < 3 && dil_cards > 0)    //есть только две карты
        {
            printf("[%s] [closed]", card_to_string(place->hand_card.first_hand[0]));
        }
        else    //у дилера 21 или конец игры
        {
            for (int i = 0; place->hand_card.first_hand[i].rank != 0; i++)
            {
                printf("%s, ", card_to_string(place->hand_card.first_hand[i]));
            }
        }
        printf("\n");
    }
}

void set_conio_mode(struct termios *oldattr) 
{
    struct termios newattr;
    tcgetattr(STDIN_FILENO, &newattr);
    *oldattr = newattr; // Сохраняем текущие атрибуты терминала
    newattr.c_lflag &= ~(ICANON | ECHO); // Отключаем канонический режим и эхо
    tcsetattr(STDIN_FILENO, TCSANOW, &newattr);
}

void reset_conio_mode(struct termios *oldattr) 
{
    tcsetattr(STDIN_FILENO, TCSANOW, oldattr);
}

void conio_mode(int mode)
{   
    struct termios attr;
    tcgetattr(STDIN_FILENO, &attr);
    if(mode == 1)
        attr.c_lflag |= (ICANON); // Включаем канонический режим
    else if(mode == 0)
        attr.c_lflag &= (-ICANON);
    tcsetattr(STDIN_FILENO, TCSANOW, &attr);
}

void display_menu(int selected, int size_menu, int code_menu)
{   
    printf("Выбор:\n");
    for (int i = 0; i < size_menu; i++) {
        if (i == selected) {
            printf("> %s <\n", MENU_[code_menu][i]); // Выделяем выбранный элемент
        } else {
            printf("  %s\n", MENU_[code_menu][i]);
        }
    }
}

void print_interface_bl_j(room_t *room, int num_player, int my_place_id)
{
    printf("Room #%d. Players: %d\n", room->id, num_player);
    for (int i = 0; i < num_player; i++)
    {
        print_main_menu(&room->place[i], i, my_place_id);
    }
    print_main_menu(&room->place[DILER_PLACE], DILER_PLACE, 0);
}


int choice_menu(int size_menu, int code_menu, room_t *room, int my_place_id)
{   
    int running = 1;
    int selected = 0;
    while (running) 
    {   
        if(room != NULL)
        {
            print_interface_bl_j(room, get_num_player(room->place), my_place_id);
        }
            display_menu(selected, size_menu, code_menu);
        // Получаем ввод
        char c = getchar();
        if (c == 27) 
        { // ESC означает начало последовательности
            getchar(); // Пропускаем '['
            switch (getchar()) 
            { // Читаем направление стрелки
                case 'A': // Стрелка вверх
                    selected = (selected == 0) ? size_menu - 1 : selected - 1;
                    break;
                case 'B': // Стрелка вниз
                    selected = (selected == size_menu - 1) ? 0 : selected + 1;
                    break;
            }
        } else if (c == '\n') 
        { // Enter
            running = 0;
        }

        printf("\033[H\033[J"); // Очистка экрана
    }

    return selected;
}

void init_menu(struct termios *oldattr)
{
    printf("\033[H\033[J"); // Очистка экрана
    printf("\033[?25l"); // Скрываем курсор
    set_conio_mode(oldattr); // Устанавливаем новый режим терминала
}

void close_menu(struct termios *oldattr)
{
    set_echo_mode(1);
    printf("\033[?25h"); // Показываем курсор перед выходом
    reset_conio_mode(oldattr); // Возвращаем исходные настройки терминала
}

void set_cursor_mode(int mode)
{
    if (mode)
    {
        printf("\033[?25h");
    }
    else{
        printf("\033[?25l");
    }
}

void set_echo_mode(int mode)
{
    struct termios tty;
    tcgetattr(STDIN_FILENO, &tty); // Получаем текущие настройки терминала.

    if (mode) {
        tty.c_lflag |= ECHO; // Включаем echo.
    } else {
        tty.c_lflag &= ~ECHO; // Отключаем echo.
    }

    tcsetattr(STDIN_FILENO, TCSANOW, &tty); // Применяем изменения.
}

void clean_tty(void)
{
    printf("\033[H\033[J"); // Очистка экрана
}
