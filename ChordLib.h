#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#define PROTOCOL "CHORD"
#define VERSION "1.0"
#define BLEN 300
//int debug=1;
//#define log(funcName,str,variable) if (debug ==1) { printf("
//#define log(a,b,c) if(debug) printf("%s ... %s ... %s", a, b, c);

struct Node {
	int keyID;
	char ipstr[INET6_ADDRSTRLEN];
	int port;
	struct Node* next;
	int sblNo;
	
} *finger[4], *pred, *origin;




struct Msg {
	char method[15];
	char proto[6];
	char ver[4];
	int keyID;
	char hostIP[INET6_ADDRSTRLEN];
 	int hostPort;
	char contactIP[INET6_ADDRSTRLEN];
	int contactPort; 
	struct metaFile* fileInfo ;
	int sblNoMsg;
};

struct metaFile {
	char* fileName;
        char* contentType;
        int contentLength;
        char* data;
};

int tcpConnect(struct Node* n);

struct Msg* initMsg(); //Allocates space for struct Msg ,initialises it and returns the struct Msg* .

int utilFramePacket(char** attr, char** val,char *pkt); //Takes attribute value pairs to form packet 

char* framePacket(char* method, short int keyID, struct  Node* hostNode,struct Node* contactNode, struct metaFile* payload, struct Msg** m) ; //Returns a char * which is the packet to be sent.

struct Node * lookup(short int id); // Looks for the node responsible for the specific by executing chord algorithm for lookup .

int sendPkt(int sock,char *buf);

struct Msg* getKey(short int id); // Looks up for the key,finds the node responsible for the key,frames pkt,sends pkt ,receives response and return response in a struct.

struct Msg * tokenize(char *pkt); // Tokenizes the pkt and converts to struct Msg.

char *recvPkt(int sock); // Receives the pkt and returns it as a string.

int action(struct Msg *m); //Takes action based on the values of struct Msg *m ,and returns 0 for pass and non-zero for failure.

int join(char *ip,int port); //Called when a new peer is joining the Chord system

struct Node* findSuccessorClient(int id);

int leave();//Yet to think about it.

void initFingerTable( char *ip,int port );
struct Msg* token(char *str);
