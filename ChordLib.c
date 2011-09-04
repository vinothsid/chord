#include "ChordLib.h"

int tcpConnect(struct Node* n) {
	int sock;
	struct hostent* host;
	struct sockaddr_in server_addr;
	
	host = gethostbyname(n->ipstr);
	
	if((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket cannot be opened");
		exit(1);
	}
	
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(n->port);
	server_addr.sin_addr = *((struct in_addr*)host->h_addr);
	memset(&(server_addr.sin_zero), 0, sizeof server_addr.sin_zero);

	if (connect(sock, (struct sockaddr *)&server_addr,sizeof(struct sockaddr)) == -1) {
           perror("Connection could not be opened");
           exit(1);
         }
	
	return sock;
}
struct Msg* initMsg(){
	struct Msg* newMsg;
	newMsg = (struct Msg*)malloc(sizeof(struct Msg));
        newMsg->hostPort = -1;
	return newMsg;
}



/*char* framePacket(char* method, short int keyID, struct  Node* n, char* payload, struct Msg** m) {
        char *pkt = "Inside Frame Packet";
	*m = initMsg();
        return pkt;
}*/

struct Msg* getKey(short int id) {
	struct Node *n;
	struct Msg  *m1,*m2;
        char *requestPkt,*responsePkt;
	int sock;
	n = lookup(id);	
	requestPkt = framePacket("Get",id,n,NULL,&m1 );
	sock = tcpConnect(n);
	sendPkt(sock,requestPkt);	
	responsePkt = recvPkt(sock);
	close(sock);
	//m2 = tokenizePkt(responsePkt);//to be used later		
	return m2;
}


struct Node * lookup(short int id) {
	struct Node *n;
	n=(struct Node*)malloc(sizeof(struct Node));
	n->keyID = 1011;
	strcpy(n->ipstr,"127.0.0.1");
	n->port = 5000;
	n->next = NULL;
	return n;
}

char* framePacket(char* method,short int keyID, struct  Node* n, char* payload, struct Msg** m) {
	char *pkt ;
	if (strcmp(method,"GET") == 0) {
		pkt = "GET 1011 Chord/1.1\nHost: 127.0.0.1:5000\nContact: 127.0.0.1:3490\n\n";
	}
	return pkt;
}

int sendPkt(int sock,char *buf) {
	/* send request */
	
	return send(sock, buf, strlen(buf),0);
}

char *recvPkt(int sock) {
	char recBuf[BLEN];
        char *recBptr; //pointer to recBuf
        int n;
        int buflen;
	recBptr=recBuf;
        buflen=BLEN;


	while ((n=recv(sock,recBptr,buflen,0))>0) {
                recBptr +=n;
                buflen -=n;
        }
	
	return recBuf;

}
