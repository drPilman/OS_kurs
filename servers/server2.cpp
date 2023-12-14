#include "server.hpp"
#include <sys/sysinfo.h>

#define COMAND_GET_ONCE_SWAP_ALL 1
#define COMAND_GET_WHILE_SWAP_ALL 2

#define COMAND_GET_ONCE_SWAP_FREE 3
#define COMAND_GET_WHILE_SWAP_FREE 4

using namespace std;

struct sysinfo info;

void getSwapAll(char *buf, size_t n)
{
    if (sysinfo(&info))
    {
        std::cerr << "Ошибка при получении информации о системе" << std::endl;
        exit(-1);
    }
    snprintf(buf, n, "\rSwap all = %lu ", info.totalswap * info.mem_unit);
}

void getSwapFree(char *buf, size_t n)
{
    if (sysinfo(&info))
    {
        std::cerr << "Ошибка при получении информации о системе" << std::endl;
        exit(-1);
    }
    snprintf(buf, n, "\rSwap free = %lu ", info.freeswap * info.mem_unit);
}

void process_client(int client)
{
    char command = ' ';
    char c;
    char buf[32], buf2[32];
    int w, h;

    timeval timeout;
    fd_set rfds;
    while (1)
    {
        cout << "wait command" << endl;
        skip_while_char(client, '#');
        if (0 == read(client, &command, 1))
        {
            exit(-1);
        }
        cout << "process comand " << (int)command << endl;
        switch (command)
        {
        case COMAND_GET_ONCE_SWAP_ALL:
            gen_result_and_send(client, getSwapAll, buf, sizeof(buf));
            break;
        case COMAND_GET_ONCE_SWAP_FREE:
            gen_result_and_send(client, getSwapFree, buf, sizeof(buf));
            break;
        case COMAND_GET_WHILE_SWAP_ALL:
            gen_result_and_send_while(client, getSwapAll, buf, buf2, sizeof(buf));
            break;
        case COMAND_GET_WHILE_SWAP_FREE:
            gen_result_and_send_while(client, getSwapFree, buf, buf2, sizeof(buf));
            break;
        default:
            exit(-1);
        }
    }

    int t = close(client);
    printf("PID %d close socket", getpid());
    exit(t);
}

int main(int argc, char *argv[])
{
    uint16_t port = 8002;
    if (argc == 2)
    {
        port = atoi(argv[1]);
    };
    run_server(port, process_client);
    return 0;
}