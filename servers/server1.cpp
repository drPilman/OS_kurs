#include "server.hpp"

#define COMAND_GET_ONCE_WH 1
#define COMAND_GET_WHILE_WH 2
#define COMAND_SET_TITLE 3

using namespace std;

void getWindowSize(char *buf, size_t n)
{
    struct winsize size;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);
    snprintf(buf, n, "\rSize of Window is (%dx%d)", size.ws_row, size.ws_col);
}

void process_client(int client)
{
    char command = ' ';
    char c;
    char buf[32], buf2[32];
    int w, h;

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
        case COMAND_GET_ONCE_WH:
            gen_result_and_send(client, getWindowSize, buf, sizeof(buf));
            break;
        case COMAND_GET_WHILE_WH:
            gen_result_and_send_while(client, getWindowSize, buf, buf2, sizeof(buf));
            break;
        case COMAND_SET_TITLE:
            if (recive_string(client, buf, sizeof(buf)))
            {
                printf("%c]0;%s%c\r\n", '\033', buf, '\007');
                snprintf(buf, sizeof(buf), "\rУдачно");
            }
            else
            {
                snprintf(buf, sizeof(buf), "\rНеудачно");
            }
            write(client, buf, strlen(buf));
            break;

        default:
            exit(-1);
        }
    }

    int t = close(client);
    printf("PID %d close socket with code: %d\n", getpid(), t);
    exit(t);
}

int main(int argc, char *argv[])
{
    uint16_t port = 8001;
    if (argc == 2)
    {
        port = atoi(argv[1]);
    };
    run_server(port, process_client);
    return 0;
}