#include "ChordLib.h"
//#include "serverLib.h"

extern struct Node* origin;
extern struct Node* finger[4];


 int debug=1;
/**************************** TCP CONNECT ****************************************/
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

/***************************** INIT Msg *****************************************/
/************** allocates memory to a Msg pointer ******************************/
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

/************************* GET KEY ******************************************/


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
	//m2 = tokenize(responsePkt);//to be used later	*/	
	//return m2;
}

/**********************************Tokenize also defined in serverLib.c ************************************/
struct Msg* tokenize(char* pkt) {

}
/**************************************************************************************/


struct Node* findSuccessorClient(int id){
/*	struct Node *n=(struct Node*)malloc(sizeof(struct Node));
        n->keyID = 1035;
        strcpy(n->ipstr,"127.0.0.1");
        n->port = 5000;
        n->next = NULL;
        if (debug == 1 ) {
                printf("findSuccessorClient()\n ");
        }
*/
/* TO DO : If all finger is nil , then return origin*/
	int i =0;
	int nilCounter=0;
//If all fingers are nil , then return origin

	for ( i=1;i<4;i++ ) {
		if (finger[i]->keyID==-1) {
			nilCounter++;
		}
		else {
			printf("not -1 for index:%d\n",i);
		}
	}

	printf("nilCounter : %d\n",nilCounter);
	if ( nilCounter==3 ) {
		return origin;
	}

//The below statements execute only when all finger's keyID are not -1	
	if (finger[0]->keyID < id &&  id <= finger[1]->keyID){
		return finger[1];
	} else {
		for (i=3; i>=1; i--) {
//If finger[i]->keyID is -1 ,continue the loop without returning . Finally the highest finger with proper value is returned in the if clause.(return finger[i])
			if (finger[i]->keyID==-1) {
				continue;
			}
			if (finger[i]->keyID < id ) {
//				finger[i]->port=5000;
//				strcpy(finger[i]->ipstr,"127.0.0.1");
				return finger[i];
			} 
		}
	}	
	
	return NULL;
}
/******************************************* INIT FINGER TABLE ***************************************/

void initFingerTable( char *ip,int port ){
	int i;
	if (debug ==1 ){
		printf("initFingerTable()\n");
	}
	origin=(struct Node *)malloc(sizeof(struct Node));
	pred=(struct Node *)malloc(sizeof(struct Node));
	origin->keyID=0;
	origin->port=5000;
	pred->keyID=-1;
	strcpy(pred->ipstr,"0.0.0.0");
	pred->port=0;
	strcpy(origin->ipstr,"127.0.0.1");

	for (i=0; i<4; i++) {
		finger[i]=(struct Node*)malloc(sizeof(struct Node));
		finger[i]->keyID=-1;
	}

//Initialise the self values
	strcpy(finger[0]->ipstr,ip);
	finger[0]->port = port;
}

/********************************************* JOIN **************************************************/

int join (char *ip,int port) {
	//struct Node n;
	struct Msg *m1, *m2;
	int sock;
	char  *requestPkt;
	char  *responsePkt;
	initFingerTable(ip,port);
	if (debug ==1 ) {
		printf("join()\n");
	}	
	pred->keyID=-1;
	requestPkt= framePacket("JOIN",-1,NULL,NULL,NULL,&m1);
	
	sock = tcpConnect(origin);
	printf("Socket num : %d\n",sock);
	if (sock < 0) {
		perror("Socket connection failed");
		exit(1);
	}
	sendPkt(sock,requestPkt);

	responsePkt=recvPkt(sock);


	printf("Response pkt: %s\n",responsePkt);
	close(sock);	
	m2=token(responsePkt);
	if (debug==1) {
		printf("MSG struct values . method : %s , keyId : %d ,hostIp: %s,hostPort: %d,contactIp: %s,contactPort: %d\n",m2->method,m2->keyID,m2->hostIP,m2->hostPort,m2->contactIP,m2->contactPort);
	}
	finger[0]->keyID = m2->keyID;// m2->keyID;
	finger[0]->sblNo = 0; //m2->sblNoMsg;
	//Next instruction sets the successor of the current node

	struct Node *temp = lookup(finger[0]->keyID + 1);
	finger[1] = temp; 
	if ( finger[1]==NULL ) {
		perror("First finger is NULL\n");
		exit(1);
	}
	printf("\n1111111111\n");	

	if (debug==1) {
		printf("join() : Actual Successor info: %s , keyID : %d",nodeToString(finger[1]),finger[1]->keyID);
	}
	//Set of instructions that set the finger table
	//Each time a lookup is performed a check is performed to check if 
	//the current finger is also the next finger
	if (finger[1]->keyID < (finger[0]->keyID + 2)) {
		printf("lookup(finger[2]): finger[1].keyID : finger[0]->keyID +2: %d %d \n", finger[1]->keyID, finger[0]->keyID + 2);
		finger[2] = lookup((finger[0]->keyID + 2));
	}
	else 
		finger[2] = finger[1];
	printf("2222222\n");
        if (finger[2]->keyID < (finger[0]->keyID + 4)) {
		printf("lookup(finger[3]): finger[2].keyID : finger[0]->keyID +4: %d %d \n", finger[1]->keyID, finger[0]->keyID + 4);
                finger[3] = lookup((finger[0]->keyID + 4));
        }
        else
                finger[3] = finger[2];
       /* if (finger[3]->keyID < (finger[0]->keyID + 8)) {
                finger[4] = lookup((finger[0]->keyID + 8));
        }
        else
                finger[4] = finger[3];*/

	printf("3333333333\n");
	return 0;
}

/********************************************* LOOK UP ********************************************/

struct Node * lookup(short int id) {
printf("4444444444\n");
	struct Node *n;
	struct Node *n2;
	struct Node *n3;
	n=(struct Node *)malloc(sizeof(struct Node));
	n3=(struct Node *)malloc(sizeof(struct Node));
	struct Msg *m1, *m2;
	int sock;
	//int sock1;
	char respCode[15];
	char *requestPkt, *responsePkt;
	//old test code
	/*n=(struct Node*)malloc(sizeof(struct Node));
	n->keyID = 1011;
	strcpy(n->ipstr,"127.0.0.1");
	n->port = 5000;
	n->next = NULL;
	if (debug == 1 ) {
		printf("lookup() ");
	}
	return n;*/
	//end of old test code
printf("4444444444\n");
	n2 = findSuccessorClient(id);
	if (n2==NULL) {
		perror("Find Successor Client returned NULL");
		exit(1);
	}

	if (debug==1) {
		printf("lookup(): value returned from findSuccessorClient keyID: %d ip:%s port: %d\n",n2->keyID,n2->ipstr,n2->port);
	}
	
	requestPkt=framePacket("GET",id, n2, finger[0], NULL, &m1);		
//sleep(2);
	sock = tcpConnect(n2);
	sendPkt(sock, requestPkt);
	responsePkt=recvPkt(sock);
	close(sock);
	m2=token(responsePkt);
	strcpy(respCode,m2->method);
	if (debug == 1) {
	        printf("lookup() 1 : MSG struct values . method : %s , keyId : %d ,hostIp: %s,hostPort: %d,contactIp: %s,contactPort: %d\n",m2->method,m2->keyID,m2->hostIP,m2->hostPort,m2->contactIP,m2->contactPort);
	}
	while (strcmp(m2->method,"305")==0) {
		strcpy(n3->ipstr, m2->contactIP);
		n3->port = m2->contactPort;
		requestPkt = framePacket("GET", id, n3, finger[0],NULL, &m1);
		sock = tcpConnect(n3);
		sendPkt(sock, requestPkt);
		responsePkt=recvPkt(sock);
		close(sock);	
		m2 = token(responsePkt);
		strcpy(respCode,m2->method);
		if (debug == 1) {
	        	printf("lookup() redirection : MSG struct values . method : %s , keyId : %d ,hostIp: %s,hostPort: %d,contactIp: %s,contactPort: %d\n",m2->method,m2->keyID,m2->hostIP,m2->hostPort,m2->contactIP,m2->contactPort);
		}
			
	}	
	
	if (strcmp(m2->method,"200")==0) {
		strcpy(n->ipstr,m2->contactIP);
		n->port = m2->contactPort;
		n->keyID=m2->keyID;
		if (debug==1) {
			printf("lookup() : Actual Successor info : %s , keyId: %d\n\n",nodeToString(n),n->keyID);
		}
		return n;

	}
	return NULL;
}

/************************************ Util Frame Packet ********************************************/

int utilFramePacket(char** attr, char** val,char *pkt) {
	strcpy(pkt,val[0]);
	strcat(pkt," ");
	strcat(pkt,PROTOCOL);
	strcat(pkt,"/");
	strcat(pkt,VERSION);
	int len;
	len = strlen(pkt);
	//pkt[len]=';';
pkt[len] = '\n';
	if (debug == 1) {
		printf("utilFramePacket(): first line : %s\n", pkt);
	}	
	int i=1;
	while(attr[i]!=NULL){
		strcat(pkt,attr[i]);
		strcat(pkt,":");
		//strcat(pkt," ");
		strcat(pkt,val[i]);
		len = strlen(pkt);
        //	pkt[len]=';';
        	pkt[len] = '\n';
		i++;
	}
	len = strlen(pkt);
        	pkt[len+1] = '\n';
		
	if (debug ==1 ){
		printf("utilFramePacket() :Packet : \n%s%d\n", pkt,len);

	}
	return 4;	
}

/************************************ Frame Packet *********************************************/

char* framePacket(char* method,short int keyID, struct  Node* hostNode, struct Node* contactNode, struct metaFile* payload, struct Msg** m) {
	char *pkt;
	char host[INET6_ADDRSTRLEN + 10];
	char contact[INET6_ADDRSTRLEN + 10];
	char *tempHost;
	if (hostNode != NULL) {
		tempHost = (char *)nodeToString(hostNode);
		strcpy(host,tempHost);
		free(tempHost);
	} 
	else 
		strcpy(host,"anonymous:6060");
	//strcpy(contact,nodeToString(contactNode));
	if (debug == 1 ) {
		printf("framePacket() : Host : %s\n ",host);
	}
	if (payload ==NULL) {
		char *attr[15] = {"METHOD" , "ID" ,"HOST", "CONTACT" } ;
		char *val[15] = {method , itoa(keyID),host,"contact:8080"}; 
		printf("after val\n");	
		pkt = (char *)malloc(BLEN*sizeof(char));
		utilFramePacket(attr,val,pkt);
		if (debug == 1) {
			printf("framePacket(): returned from utilFramePacket()\n");
		}
	} 
	else 
		pkt = (char *)malloc(BLEN + payload->contentLength );
		
	
	

	/*Initialising values of struct Msg*/
	struct Msg* temp=initMsg();
	
		
	strcpy(temp->method,method);
	strcpy(temp->proto, PROTOCOL);
	strcpy(temp->ver, VERSION);
	temp->keyID=keyID;

	if (hostNode != NULL) {
		strcpy(temp->hostIP,hostNode->ipstr);
		temp->hostPort=hostNode->port;
	}
/*
	if (contactNode!=NULL) {
		strcpy(temp->contactIP,contactNode->ipstr);
		temp->contactPort = contactNode->port; 
	}
	if (payload!=NULL) {
		temp->fileInfo=payload;
	} else {
	}
	
	*m=temp;
*/
	/*End of initialisation*/
	
/*	if (strcmp(temp->method,"GET") == 0) {
		pkt= "GET 1011 Chord/1.1\nHost: 127.0.0.1:5000\nContact: 127.0.0.1:3490\n\n";
	
		strcpy(pkt,temp->method);
		strcat(pkt," ");
		strcat(pkt,temp->keyID);	
	
	}
*/ 
        printf("framePacket() : Packet: %s\n",pkt );
	return pkt;
}

/******************************************************* SEND PACKET *****************************/

int sendPkt(int sock,char *buf) {
	/* send request */
	if (debug == 1) {
		printf("sendPkt() : Sending pkt:\n%s\n",buf);
	}	
	return send(sock, buf, strlen(buf) ,0);
}

/***************************************** RECEIVE PACKET ***************************************/

char *recvPkt(int sock) {
	char *recBuf = (char *)malloc(BLEN);
        char *recBptr; //pointer to recBuf
        int n;
        int buflen;
	recBptr=recBuf;
        buflen=BLEN;
	
	while ((n=recv(sock,recBptr,buflen,0))>0) {
                buflen -=n;
	/*	if (debug == 1) {
			recBptr[n]='\0';
			printf("Received pkt:%s Num of bytes received: %d\n",recBptr,n);
        	}       */
                recBptr +=n;
	//	recBptr++;
		break;

        }

	recBuf[recBptr-recBuf+1] = '\0';
	//if (debug == 1) {
	//	printf("\nrecvPkt() : Received pkt:\n%s",recBuf);
	//}	
	return recBuf;

}

struct Msg* token(char *str1)
{

        struct Msg* rcvMsg;
        char str[300];
        strcpy(str,str1);
	if (debug==1) {
	        printf("\nThis is the passed value :\n%s \n",str);
	}
        char *p ;
        rcvMsg=(struct Msg *)malloc(sizeof(struct Msg ));
        p=strtok(str," /:;'\n'");
        strcpy(rcvMsg->method,p);
        printf("....%s...\n",rcvMsg->method);
        p=strtok(NULL," /:;'\n'");
        strcpy(rcvMsg->proto,p);
        printf("....%s...\n",rcvMsg->proto);
        p=strtok(NULL," /:;'\n'");
        strcpy(rcvMsg->ver,p);
        printf("....%s...\n",rcvMsg->ver);
        p=strtok(NULL," /:;'\n'");
        p=strtok(NULL," /:;'\n'");
        rcvMsg->keyID =atoi(p);
        printf("....%d...\n",rcvMsg->keyID);
        p=strtok(NULL," /:;'\n'");
        p=strtok(NULL," /:;'\n'");
        strcpy(rcvMsg->hostIP,p);
        printf("....%s...\n",rcvMsg->hostIP);
        p=strtok(NULL," /:;'\n'");
        rcvMsg->hostPort=atoi(p);
        printf("....%d...\n",rcvMsg->hostPort);
        p=strtok(NULL," /:;'\n'");
        p=strtok(NULL," /:;'\n'");
        strcpy(rcvMsg->contactIP,p);
        printf("....%s...\n",rcvMsg->contactIP);
	p=strtok(NULL," /:;");
	rcvMsg->contactPort=atoi(p);
	printf("....%d...\n",rcvMsg->contactPort);
        rcvMsg->fileInfo= NULL;
        rcvMsg->sblNoMsg=0;
//rcvMsg->fileInfo = str->fileInfo;
//rcvMsg->sblNoMsg = str->sblNoMsg;
	if (debug==1) {
	       printf("\n SUCCESSFULLY TOKENIZED \n");
	}
        return rcvMsg;
}

int liesBetween(id,start,end) {
	if (start<end) {
		if( id > start && id <=end){
			return 1;
		} else {
			return 0;
		}
	} else {
		if ( id>start ) {
			return 1;
		} else {
			return 0;
		}
	}
}

void stabilize() {
	char *requestPkt,*responsePkt;
	struct Msg* m2;
	int sock;
        char *attr[15] = {"METHOD" , "ID" ,"HOST", "CONTACT" } ;
        char *val[15] = {"STAB" , itoa(finger[0]->keyID),nodeToString(finger[0]),nodeToString(finger[1])};
	requestPkt = (char *)malloc(BLEN*sizeof(char));

        utilFramePacket(attr,val,requestPkt);
        printf("stabilize() : Packet : \n%s\n ",requestPkt);

	sock=tcpConnect(finger[1]);
        sendPkt(sock,requestPkt);
	
        responsePkt=recvPkt(sock);
        close(sock);
        m2 = token(responsePkt);

	if (strcmp(m2->method,"100")==0) {
		if(debug==1) {
			printf("Inside 100 response\n");
		}
		if ( liesBetween(m2->keyID,finger[0]->keyID,finger[1]->keyID)==1) {
			if(debug==1) {
				printf("Inside liesBetween\n");
			}
			finger[1]->keyID=m2->keyID;
			strcpy(finger[1]->ipstr,m2->contactIP);
			finger[1]->port=m2->contactPort;
		}
					
	} else {
		perror("stabilize(): unrecognised status code in this module\n");
	}

	notify();
	
}

void notify() {
        char *requestPkt,*responsePkt;
        struct Msg* m2;
        int sock;
        char *attr[15] = {"METHOD" , "ID" ,"HOST", "CONTACT" } ;
        char *val[15] = {"NOTIFY" , itoa(finger[0]->keyID),nodeToString(finger[0]),nodeToString(finger[0])};
        requestPkt = (char *)malloc(BLEN*sizeof(char));

        utilFramePacket(attr,val,requestPkt);
        printf("notify() : Packet : \n%s\n ",requestPkt);

        sock=tcpConnect(finger[1]);
        sendPkt(sock,requestPkt);

        responsePkt=recvPkt(sock);
        close(sock);
        m2 = token(responsePkt);

        if (strcmp(m2->method,"110")!=0) {
		perror("notify(): unrecognised status code in this module\n");
	}
}


void printTable() {
	int i =0;
	printf("Predecesor : KeyID : %d , IP : %s , Port %d\n ",pred->keyID,pred->ipstr,pred->port);
	for (i=0;i<4;i++){
		printf("Finger : %d KeyID: %d IP : %s , Port %d\n",i,finger[i]->keyID,finger[i]->ipstr,finger[i]->port);
	}
}
