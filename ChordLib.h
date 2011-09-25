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

#define PROTOCOL "CHORD"
#define VERSION "1.0"
#define BLEN 300
#define LISTENQUEUE 10
#define PEER_ZERO_IP "192.168.15.4"
#define PEER_ZERO_PORT 5000
#define RFC_PATH ""
//int debug=1;
//#define log(funcName,str,variable) if (debug ==1) { printf("
//#define log(a,b,c) if(debug) printf("%s ... %s ... %s", a, b, c);
int leaveFlag;
int totalPeers;
int finalStabilizeComplete;
struct Node {
	int keyID;
	char ipstr[INET6_ADDRSTRLEN];
	int port;
	struct Node* next;
	int sblNo;
	
} *finger[4], *pred, *origin;

struct msgToken {
        char *ptr;
        int sock;

};

//int IDspace[50] = {165,646,469,57,668,361,759,953,122,5,702,173,994,675,893,328,995,232,971,531,354,947,20,604,413,20,440,885,743,821,15,249,277,17,235,451,21,238,599,809,319,585,894,55,924,497,183,411,670,658};

//char* RFCnames[50] = {"rfc4261","rfc3718","rfc1493","rfc3129","rfc2716","rfc4457","rfc2807","rfc1977","rfc2170","rfc1029","rfc3774","rfc1197","rfc4066","rfc4771","rfc2941","rfc1352","rfc4067","rfc5352","rfc4043","rfc3603","rfc5474","rfc5043","rfc5140","rfc3676","rfc2461","rfc3092","rfc1464","rfc1909","rfc2791","rfc2869","rfc3087","rfc4345","rfc3349","rfc3089","rfc5355","rfc5571","rfc3093","rfc2286","rfc3671","rfc3881","rfc5439","rfc1609","rfc1918","rfc4151","rfc1948","rfc4593","rfc1207","rfc4507","rfc4766","rfc4754"};




struct Msg {
	char method[15];
	char proto[6];
	char ver[4];
	int keyID;
	char hostIP[INET6_ADDRSTRLEN];
 	int hostPort;
	char contactIP[INET6_ADDRSTRLEN];
	int contactPort;
	int succID;
	int predID; 
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

int leave();//just added , modifications...suggest... 9/20/2011, 1:07pm

void initFingerTable( char *ip,int port );
struct Msg* token(char *str);
void stabilize() ;
void notify() ;
void printTable() ;

void func(char *st);
int tcpServer();

void sendRFC(int new_fd,char *name);
char* findRFCfromID(int );//finds the RFC corresponding to an ID
void *serverThread (void *parmPtr);

char *itoa(int num); //converts integer to a character string, make sure to hold the return in a place holder and free it after use

char *nodeToString(struct Node *n); //converts ip and port information of a host into a single contiguous string where ip and port are separated by a ":"
