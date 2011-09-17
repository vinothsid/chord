#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>
#include <netinet/in.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include "ChordLib.h"

struct msgToken {
	char *ptr;
	int sock;

};
void func(char *st);
int tcpServer();

//struct Msg* token(char *str);
//void (*abc)();

