#define _XOPEN_SOURCE_EXTENDED 1
#define _OPEN_MSGQ_EXT
#include <stdio.h>      // standard input and output library
#include <stdlib.h>     // this includes functions regarding memory allocation
#include <string.h>     // contains string functions
#include <errno.h>      //It defines macros for reporting and retrieving error conditions through error codes
#include <time.h>       //contains various functions for manipulating date and time
#include <unistd.h>     //contains various constants
#include <sys/types.h>  //contains a number of basic derived types that should be used whenever appropriate
#include <arpa/inet.h>  // defines in_addr structure
#include <sys/socket.h> // for socket creation
#include <netinet/in.h> //contains constants and structures needed for internet domain addresses
#include <iostream>
#include <iostream>
#include <unistd.h>
#include <sys/ioctl.h>

typedef void Fprocess_client(int);
typedef void gen_result(char *, size_t);

int recive_string(int client, char *buffer, size_t n);
void skip_while_char(int client, char stop);
void exit_if_err(int check);
void gen_result_and_send(int socket, gen_result f_gen, char *buf, size_t n);
void gen_result_and_send_while(int sock, gen_result f_gen, char *buf, char *buf2, size_t n);
void run_server(uint16_t port, Fprocess_client f_process_client);