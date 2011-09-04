#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#define PROTOCOL "CHORD";
#define VERSION "1.0";


struct Node {
	int id;
	char ipstr[INET6_ADDRSTRLEN];
	short int port;
	struct Node* next;
};

struct Msg {
	char method[15];
	char proto[6];
	char ver[4];
	short int keyID;
	char host[INET6_ADDRSTRLEN];
 	short int hostPort;
	char contact[INET6_ADDRSTRLEN];
	short int contactPort; 
	char* fileName;
	char* contentType;
	int contentLength;
	char* data;
};

int tcpConnect(struct Node* n);
struct Msg* initMsg();
char* framePacket(char* method, short int keyID, struct  Node* n, char* payload, struct Msg** m) ;

