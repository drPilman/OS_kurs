#include "server.hpp"
using namespace std;

int recive_string(int client, char *buffer, size_t n)
{
    char c = ' ';
    size_t i = 0;
    while (1 == read(client, &c, 1) && c != 0)
    {
        buffer[i++] = c;
        if (i == n)
        {
            return false;
        }
    }
    buffer[i] = 0;
    return true;
}
void skip_while_char(int client, char stop)
{
    char c = ' ';
    while (1 == read(client, &c, 1) && c != stop)
        ;
    if (c != stop)
    {
        cout << "closed" << endl;
        exit(-1);
    }
}
void exit_if_err(int check)
{
    if (check == -1)
    {
        cerr << strerror(errno);
        exit(-1);
    }
}
void gen_result_and_send(int sock, gen_result f_gen, char *buf, size_t n)
{
    f_gen(buf, n);
    size_t l = strlen(buf);
    if (write(sock, buf, l) != l)
    {
        cout << "Сокет клиента закрылся\n";
        exit(-1);
    }
}
void gen_result_and_send_while(int sock, gen_result f_gen, char *buf, char *buf2, size_t n)
{
    strcpy(buf2, "$");
    size_t l, l2 = 1;
    timeval timeout;
    fd_set rfds;
    do
    {
        memset(buf, ' ', n);
        f_gen(buf, n);

        if (0 != strcmp(buf, buf2))
        {
            cout << "send\n";
            l = strlen(buf);
            buf[l] = ' ';
            l2 = max(l, l2);
            buf[l2] = 0;
            if (write(sock, buf, l2) != l2)
            {
                cout << "Сокет клиента закрылся\n";
                exit(-1);
            }
            strcpy(buf2, buf);
            l2 = l;
        };
        timeout.tv_sec = 0;
        timeout.tv_usec = 100;
        FD_ZERO(&rfds);
        FD_SET(sock, &rfds);
    } while (0 == select(sock + 1, &rfds, NULL, NULL, &timeout));
}

void run_server(uint16_t port, Fprocess_client f_process_client)
{
    int socket_wait_clients, socket_connected_client;
    struct sockaddr_in ipOfServer = {AF_INET, htons(port), {htonl(INADDR_ANY)}};
    socket_wait_clients = socket(AF_INET, SOCK_STREAM, 0);
    exit_if_err(socket_wait_clients);

    exit_if_err(bind(socket_wait_clients, (struct sockaddr *)&ipOfServer, sizeof(ipOfServer)));
    exit_if_err(listen(socket_wait_clients, 20));

    while (1)
    {
        socket_connected_client = accept(socket_wait_clients, (struct sockaddr *)NULL, NULL);
        int pid = fork();
        if (pid == 0)
        {
            cout << "New client" << endl;
            f_process_client(socket_connected_client);
        }
        close(socket_connected_client);
    }
}