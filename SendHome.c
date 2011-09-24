#include "ChordLib.h"

int sendHome(int key, char *ipstr,int port ) {

	struct Node *n;
	char *requestPkt,*responsePkt;
	struct Msg *m2;
	int sock;

	n=(struct Node *)malloc(sizeof(struct Node));
        n->keyID=key;
        n->port=port;
        strcpy(n->ipstr,ipstr);

        char *attr[15] = {"METHOD" , "ID" ,"HOST", "CONTACT" } ;
        char *val[15] = {"GOHOME" ,"-1" , "0.0.0.0:0",nodeToString(n)};
        requestPkt = (char *)malloc(BLEN*sizeof(char));

        utilFramePacket(attr,val,requestPkt);
        printf("sendHome() : Packet : \n%s\n ",requestPkt);

        sock=tcpConnect(n);
        sendPkt(sock,requestPkt);

        responsePkt=recvPkt(sock);
        close(sock);
/*
        m2 = token(responsePkt);

        if (strcmp(m2->method,"210")!=0) {
                perror("Leave(): unrecognised status code in this module\n");
		return -1;
        }
*/
	free(n);
	free(requestPkt);
	return 0;
}

int main(int argc,char *argv[]) {

	if (argc!=4) {
		printf("Invalid number of arguments\n.Usage: ./sendHome <keyID> <IP address> <Port>");
		return -1;
	}
	if ( sendHome(argv[1],argv[2],atoi(argv[3]))==0  ) {
		printf("Sent Home successfully  \n ");
	} else {
		printf("Sending home failed.\n");	
	}

}
