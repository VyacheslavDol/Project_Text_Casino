Описание

Реализована игра Black Jack.

Клиентская и серверная части.

Созданы основные функции loop, которые прогоняются и в зависимости от кодов сообщений, которые записаны в bl_j_struct.h.

Для запуска программ необходимо ввести для сервера и клиента номер порта на котором будет слушать сервер. (Например ./client_bj_test 50004).

Основные функции игры находятся в bl_j_func.c, реализация самомого процесса(хода) игры - bl_j_basic_server(client).client

Клиент имеет возможность выбирать комнату, в случае если комната недоступна клиенту придет соответсвующее сообщение.

Программы подготовлены под интеграцию в основной проект.
Архитекура создавалась с целью создания библиотеки которая легко может интегрирваться в проект.

Сервер работает на TCP сокетах с использованием мультиплексирования для обработки входящих сообщений.