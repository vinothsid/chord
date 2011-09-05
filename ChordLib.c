#include "ChordLib.h"

int debug = 1;
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
        //newMsg->hostPort = -1;
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
	requestPkt = framePacket("GET",id,n,NULL,NULL,&m1 );
	sock = tcpConnect(n);
	if (sock==-1) {
		perror ("Socket is not established properly");
        }
	sendPkt(sock,requestPkt);	
	responsePkt = recvPkt(sock);
	if (debug == 1) {
		printf("getKey() : Response Pkt : %s\n",responsePkt);
	}
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
	if (debug == 1 ) {
		printf("lookup() ");
	}
	return n;
}



int utilFramePacket(char** attr, char** val,char *pkt) {
	strcpy(pkt,val[0]);
	strcat(pkt," ");
	strcat(pkt,PROTOCOL);
	strcat(pkt,"/");
	strcat(pkt,VERSION);
	int len;
	len = strlen(pkt);
	pkt[len]='\n';
	pkt[len+1] = '\0';
	if (debug == 1) {
		printf("The first line is %s\n", pkt);
	}	
	int i=1;
	while(attr[i]!=NULL){
		strcat(pkt,attr[i]);
		strcat(pkt,":");
		strcat(pkt," ");
		strcat(pkt,val[i]);
		len = strlen(pkt);
        	pkt[len]='\n';
        	pkt[len+1] = '\0';
		i++;
	}
	
	if (debug ==1 ){
		printf("utilFramePacket() :Packet : \n%s\n", pkt);
	}
	return 0;	
}

char* framePacket(char* method,short int keyID, struct  Node* hostNode, struct Node* contactNode, struct metaFile* payload, struct Msg** m) {
	char *pkt;
	char host[INET6_ADDRSTRLEN + 10];
	char contact[INET6_ADDRSTRLEN + 10];
	char *tempHost;
	tempHost = (char *)nodeToString(hostNode);
	strcpy(host,tempHost);
	free(tempHost);
	//strcpy(contact,nodeToString(contactNode));
	if (debug == 1 ) {
		printf("framePacket() : Host : %s\n ",host);
	}
	if (payload ==NULL) {
		char *attr[15] = {"METHOD" , "ID" ,"HOST", "CONTACT" } ;
		char *val[15] = {method , itoa(keyID),host,"contact"}; 	
		pkt = (char *)malloc(BLEN);

		utilFramePacket(attr,val,pkt);
	} else {
		pkt = (char *)malloc(BLEN + payload->contentLength );
		
		
	}
	

	/*Initialising values of struct Msg*/
	struct Msg* temp=initMsg();
	
		
	strcpy(temp->method,method);
	strcpy(temp->proto, PROTOCOL);
	strcpy(temp->ver, VERSION);
	temp->keyID=keyID;
	strcpy(temp->hostIP,hostNode->ipstr);
	temp->hostPort=hostNode->port;
	if (contactNode!=NULL) {
		strcpy(temp->contactIP,contactNode->ipstr);
		temp->contactPort = contactNode->port; 
	}
	if (payload!=NULL) {
		temp->fileInfo=payload;
	} else {
	}
	
	*m=temp;
	/*End of initialisation*/
	
/*	if (strcmp(temp->method,"GET") == 0) {
		pkt= "GET 1011 Chord/1.1\nHost: 127.0.0.1:5000\nContact: 127.0.0.1:3490\n\n";
	
		strcpy(pkt,temp->method);
		strcat(pkt," ");
		strcat(pkt,temp->keyID);	
	
	}
*/
        printf("framePacket() : Packet: %s",pkt );
	return pkt;
}

int sendPkt(int sock,char *buf) {
	/* send request */
	if (debug == 1) {
		printf("sendPkt() : Sending pkt:\n%s",buf);
	}	
	return send(sock, buf, strlen(buf),0);
}

char *recvPkt(int sock) {
	char *recBuf = (char *)malloc(BLEN);
        char *recBptr; //pointer to recBuf
        int n;
        int buflen=5;
	recBptr=recBuf;
        buflen=BLEN;

	while ((n=recv(sock,recBptr,buflen,0))>0) {
                buflen -=n;
	/*	if (debug == 1) {
			recBptr[n]='\0';
			printf("Received pkt:%s\n",recBptr);
        } */      
                recBptr +=n;
	//	recBptr++;

        }

	recBuf[recBptr-recBuf+2] = '\0';
	if (debug == 1) {
		printf("recvPkt() : Received pkt:%s",recBuf);
	}	
	return recBuf;

}
