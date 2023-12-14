#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <limits>

using namespace std;

uint choice_command(uint count)
{
    uint choice;
    string s;
    for (;;)
    {
        cin >> s;
        if (std::sscanf(s.c_str(), "%d", &choice) == 0 || choice > count)
        {
            cout << "Неверный выбор. Введите число [0," << count << "]: ";
        }
        else
        {
            if (choice == 0)
            {
                cout << "Выход";
                exit(0);
            }
            else
            {
                return choice;
            }
        }
    }
}
#define BUFFER_SIZE 1024
char buffer[BUFFER_SIZE];

void read_and_print_answer(int client_socket)
{
    int n = read(client_socket, buffer, BUFFER_SIZE - 1);
    if (n == 0)
    {
        cout << "Сокет закрылся";
        exit(-1);
    }
    buffer[n] = 0;
    cout << "\033[32m" << buffer << "\033[0m";
    fflush(stdout);
}
void skip_data(int client_socket)
{
    fd_set rfds;
    timeval timeout = {0, 100};
    FD_ZERO(&rfds);
    FD_SET(client_socket, &rfds);
    int r;
    while (0 != (r = select(client_socket + 1, &rfds, NULL, NULL, &timeout)))
    {
        read(client_socket, buffer, sizeof(buffer));
        timeout.tv_sec = 0;
        timeout.tv_usec = 100;
        FD_ZERO(&rfds);
        FD_SET(client_socket, &rfds);
    }
}
void subscribe(int client_socket)
{
    fd_set rfds;
    char stop = '!';
    int max_sock = max(STDIN_FILENO, client_socket) + 1;
    for (;;)
    {
        do
        {
            FD_ZERO(&rfds);
            FD_SET(STDIN_FILENO, &rfds);
            FD_SET(client_socket, &rfds);
        } while (0 == select(max_sock, &rfds, NULL, NULL, NULL));

        if (FD_ISSET(STDIN_FILENO, &rfds))
        {
            write(client_socket, &stop, 1);
            cout << "Отписка\n";
            // cin.ignore();
            cin.ignore();
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return;
        }
        else
        {
            read_and_print_answer(client_socket);
        }
    }
}
void talk_with_server_1(int client_socket)
{
    uint command;
    for (;;)
    {
        cout << "Введите запрос.\n"
             << "0 - выход\n"
             << "1 - получить ширину и высоту рамки окна один раз\n"
             << "2 - подписаться на получение ширины и рамки окна\n"
             << "3 - изменить название окна\n:";
        command = choice_command(3);

        skip_data(client_socket);
        buffer[0] = '#';
        buffer[1] = (char)command;

        switch (command)
        {
        case 1:
            write(client_socket, buffer, 2);
            read_and_print_answer(client_socket);
            cout << endl;
            break;
        case 2:
            write(client_socket, buffer, 2);
            subscribe(client_socket);
            break;
        case 3:
            cout << "Введите новое имя окна(до 30 символов)\n";
            cin >> buffer + 2;
            write(client_socket, buffer, strlen(buffer) + 1); // send with last 0
            read_and_print_answer(client_socket);
            cout << endl;
        }
    }
}
void talk_with_server_2(int client_socket)
{
    uint command;
    for (;;)
    {
        cout << "Введите запрос.\n"
             << "0 - выход\n"
             << "1 - получить размер файла подкачки в байтах\n"
             << "2 - подписаться на получение размер файла подкачки в байтах\n"
             << "3 - получить количество свободных байтов файла подкачки\n"
             << "4 - подписаться на получение количество свободных байтов файла подкачки\n:";
        command = choice_command(4);

        skip_data(client_socket);
        buffer[0] = '#';
        buffer[1] = (char)command;

        switch (command)
        {
        case 1:
        case 3:
            write(client_socket, buffer, 2);
            read_and_print_answer(client_socket);
            cout << endl;
            break;
        case 2:
        case 4:
            write(client_socket, buffer, 2);
            subscribe(client_socket);
            break;
        }
    }
}

sockaddr_in servers[] = {
    {AF_INET, htons(8001), {inet_addr("127.0.0.1")}},
    {AF_INET, htons(8002), {inet_addr("93.182.50.84")}}};

void update_server(char i)
{
    char port[] = "PORT_ ", host[] = "HOST_ ";
    port[5] = i;
    host[5] = i;

    size_t index = atoi(&i) - 1;
    if (index > 2)
    {
        exit(-1);
    }
    char *val = getenv(port);
    if (val != NULL)
    {
        cout << "Change port for server " << i << endl;
        servers[index].sin_port = htons(atoi(val));
    }
    val = getenv(host);
    if (val != NULL)
    {
        cout << "Change host for server " << i << endl;
        servers[index].sin_addr.s_addr = inet_addr(val);
    }
}

int main()
{
    uint server = 1;
    update_server('1');
    update_server('2');

    int client_socket = 0;
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        cerr << "Ошибка во время создания сокета:\n";
        cerr << strerror(errno);
        return -1;
    }

    cout << "Выберите сервер (1 - первый сервер, 2 - второй сервер, 0 - выход): ";
    server = choice_command(2);

    if (connect(client_socket, (struct sockaddr *)&servers[server - 1], sizeof(servers[server - 1])) < 0)
    {
        cerr << "Ошибка во время подключения к серверу:\n";
        cerr << strerror(errno);
        return -1;
    }

    if (server == 1)
    {
        talk_with_server_1(client_socket);
    }
    else
    {
        talk_with_server_2(client_socket);
    }

    return 0;
}