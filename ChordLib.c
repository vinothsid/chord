#include "ChordLib.h"
#include <pthread.h>
extern struct Node* origin;
extern struct Node* finger[4];
extern int totalPeers;
extern int leaveFlag;
extern int finalStabilizeComplete;
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t tableMutex = PTHREAD_MUTEX_INITIALIZER;

int IDspace[50] = {165,646,469,57,668,361,759,953,122,5,702,173,994,675,893,328,995,232,971,531,354,947,205,604,413,20,440,885,743,821,15,249,277,17,235,451,21,238,599,809,319,585,894,55,924,497,183,411,670,658};

char* RFCnames[50] = {"rfc4261","rfc3718","rfc1493","rfc3129","rfc2716","rfc4457","rfc2807","rfc1977","rfc2170","rfc1029","rfc3774","rfc1197","rfc4066","rfc4771","rfc2941","rfc1352","rfc4067","rfc5352","rfc4043","rfc3603","rfc5474","rfc5043","rfc5140","rfc3676","rfc2461","rfc3092","rfc1464","rfc1909","rfc2791","rfc2869","rfc3087","rfc4345","rfc3349","rfc3089","rfc5355","rfc5571","rfc3093","rfc2286","rfc3671","rfc3881","rfc5439","rfc1609","rfc1918","rfc4151","rfc1948","rfc4593","rfc1207","rfc4507","rfc4766","rfc4754"};
int count=0;
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
           printf("Connection could not be opened. Connection to port %d failed\n ",n->port );
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

/**************************************************************************************/


struct Node* findSuccessorClient(int id){

	id=id%1024;
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
			//printf("not -1 for index:%d\n",i);
		}
	}

	//printf("nilCounter : %d\n",nilCounter);
	if ( nilCounter==3 ) {
		return origin;
	}

//The below statements execute only when all finger's keyID are not -1	
	if (liesBetween(id, finger[0]->keyID , finger[1]->keyID)){
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

void initFingerTable( char *ip,int port ) {
	int i;

	leaveFlag=0;
	finalStabilizeComplete=0;

	if (debug ==1 ){
		printf("initFingerTable()\n");
	}
	origin=(struct Node *)malloc(sizeof(struct Node));
	pred=(struct Node *)malloc(sizeof(struct Node));
	origin->keyID=0;
	origin->port=PEER_ZERO_PORT;
	pred->keyID=-1;
	strcpy(pred->ipstr,"0.0.0.0");
	pred->port=0;
	strcpy(origin->ipstr,PEER_ZERO_IP);

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
	requestPkt= framePacket("JOIN",-1,finger[0],origin,NULL,&m1);
	
	sock = tcpConnect(origin);
	printf("Socket num : %d\n",sock);
	if (sock < 0) {
		perror("Socket connection failed");
		exit(1);
	}
	sendPkt(sock,requestPkt);

	responsePkt=recvPkt(sock);


	//printf("Response pkt: %s\n",responsePkt);
	close(sock);	
	m2=token(responsePkt);
	if (debug==1) {
		printf("MSG struct values . method : %s , keyId : %d ,hostIp: %s,hostPort: %d,contactIp: %s,contactPort: %d\n",m2->method,m2->keyID,m2->hostIP,m2->hostPort,m2->contactIP,m2->contactPort);
	}
	finger[0]->keyID = m2->keyID;// m2->keyID;
	finger[0]->sblNo = 0; //m2->sblNoMsg;
	//Next instruction sets the successor of the current node

	struct Node *temp = lookup((finger[0]->keyID + 1));
	finger[1] = temp; 
	if ( finger[1]==NULL ) {
		perror("First finger is NULL\n");
		exit(1);
	}
//	printf("\n1111111111\n");	

	if (debug==1) {
		printf("join() : Actual Successor info: %s , keyID : %d",nodeToString(finger[1]),finger[1]->keyID);
	}
	//Set of instructions that set the finger table
	//Each time a lookup is performed a check is performed to check if 
	//the current finger is also the next finger

	if (finger[1]->keyID < (finger[0]->keyID + 2) && !(liesBetween((finger[0]->keyID + 2),finger[0]->keyID,finger[1]->keyID))) {
		printf("lookup(finger[2]): finger[1].keyID : finger[0]->keyID +2: %d %d \n", finger[1]->keyID, finger[0]->keyID + 2);
		finger[2] = lookup((finger[0]->keyID + 2));
	}
	else 
		finger[2] = finger[1];
//	printf("2222222\n");
        if (finger[2]->keyID < (finger[0]->keyID + 4) && !(liesBetween((finger[0]->keyID + 4),finger[0]->keyID,finger[1]->keyID))) {
		printf("lookup(finger[3]): finger[2].keyID : finger[0]->keyID +4: %d %d \n", finger[1]->keyID, finger[0]->keyID + 4);
                finger[3] = lookup((finger[0]->keyID + 4));
        }
        else
                finger[3] = finger[2]; 


//When following free statements are executed , its seg faulting
	//free(requestPkt);
	//free(responsePkt);
	//free(m1);
	//free(m2);

//	printf("3333333333\n");
	return 0;
}

/********************************************* LOOK UP ********************************************/

struct Node * lookup(short int id) {

	id=id%1024;
	//printf("4444444444\n");
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
	//printf("4444444444\n");
	n2 = findSuccessorClient(id);
	if (n2==NULL) {
		perror("Find Successor Client returned NULL");
		exit(1);
	}

	if (debug==1) {
		printf("lookup(): value returned from findSuccessorClient keyID: %d ip:%s port: %d\n",n2->keyID,n2->ipstr,n2->port);
	}
	
	requestPkt=framePacket("GET",id, finger[0], n2, NULL, &m1);		
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
		requestPkt = framePacket("GET", id, finger[0], n3,NULL, &m1);
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

//When following free statements are executed seg fault occuring
	//	free(requestPkt);
	//	free(responsePkt);
	//	free(m1);
	//	free(m2);
		return n;

	}

//When following free statements are executed seg fault occuring
//	free(requestPkt);
//	free(responsePkt);
//	free(m1);
//	free(m2);
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
	pkt[len+1]='\0';
	//if (debug == 1) {
	//	printf("utilFramePacket(): first line : %s\n", pkt);
	//}	
	int i=1;
	while(attr[i]!=NULL){
		strcat(pkt,attr[i]);
		strcat(pkt,":");
		//strcat(pkt," ");
		strcat(pkt,val[i]);
		len = strlen(pkt);
        //	pkt[len]=';';
        	pkt[len] = '\n';
		pkt[len+1]='\0';
	//	printf("The pkt until here is: %s\n", pkt);
		i++;
	}
	len = strlen(pkt);
       	pkt[len+1] = '\n';
       	pkt[len+2] = '\0';
		
	//if (debug ==1 ){
	//	printf("utilFramePacket() :Packet : \n%s%d\n", pkt,len);

	//}
	return 0;	
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
		strcpy(host,"anonymous:0");
	//strcpy(contact,nodeToString(contactNode));
        if (contactNode != NULL) {
                tempHost = (char *)nodeToString(contactNode);
                strcpy(contact,tempHost);
                free(tempHost);
        }
        else
                strcpy(contact,"anonymous:0");
        //strcpy(contact,nodeToString(contactNode));


	if (debug == 1 ) {
		printf("framePacket() : Host : %s\n ",host);
	}
	if (payload ==NULL) {
		char *attr[15] = {"METHOD" , "ID" ,"HOST", "CONTACT" } ;
		char *val[15] = {method , itoa(keyID),host,contact}; 
		//printf("after val\n");	
		pkt = (char *)malloc(BLEN*sizeof(char));
		utilFramePacket(attr,val,pkt);
		if (debug == 1) {
			//printf("framePacket(): returned from utilFramePacket()\n");
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
        //printf("framePacket() : Packet: %s\n",pkt );
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
	if (debug == 1) {
		printf("\nrecvPkt() : Received pkt:\n%s",recBuf);
	}	
	return recBuf;

}

struct Msg* token(char *str1)
{
	pthread_mutex_lock( &mutex1 );
        struct Msg* rcvMsg;
        char str[300];
	str[0]='\0';
        strcpy(str,str1);
//	if (debug==1) {
//	        printf("\nThis is the passed value :\n%s \n",str);
//	}
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
	if ( strcmp(rcvMsg->method,"LEAVE" ) == 0 ) {
	        p=strtok(NULL," /:;'\n'");
		rcvMsg->succID=atoi(p);
	} else {
		rcvMsg->succID=-1;
	}

        if ( strcmp(rcvMsg->method,"PUTKEY" ) == 0 ) {
                p=strtok(NULL," /:;'\n'");
                rcvMsg->predID=atoi(p);
        } else {
                rcvMsg->predID=-1;
        }

	if (debug==1) {
	       printf("\n SUCCESSFULLY TOKENIZED \n");
	}
	pthread_mutex_unlock( &mutex1 );
        return rcvMsg;
}
/*************************** LEAVE ************************************************/
/* frame work for LEAVE..sourabh & Vinoth please read and comment...... I have thinking of adding one more int field to header which would do the following
as soon as a node leaves it will send out messages to pred and succ with a leaveStbNo=0.......... 
Now, at the pred the leaveStbNo is checked and if it is 0 then, ....succ will update its pred.....pred will update its succ.
then they will pass on the message to their succ and preds respectively after incrementing the leaveStbNo by 1
Now their succ and pred will check the leaveStbNo. and if it is 1 then it will updates its finger[2] entries ...increment leaveStbNo by 1 and pass the msg.....
when the next nodes rcv that msg....it will check leaveStbNo. , if it is >=2 then it will drop the message....................
=======
this will prevent looping and infinite message sending loops. PlEASE COMMENT ANY SUGGESTIONS, I WILL WRITE A TEMP CODE FOR THIS ***********************/
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

int putKey() {
        struct Msg *m1 ;
        int sock;
        char  *requestPkt;
        char  *responsePkt;
        // pred should be defined global ??? l
        // leave response to be written ....
        // in leave response if leaveStbNo is 0 then it checks if the host in msg is its pred then it will update pred with its contact node
        // if the host in the msg is its succ then it updates its succ with the contact node

        char *attr[15] = {"METHOD" , "ID" ,"HOST", "CONTACT","PRED.ID" } ;
        char *val[15] = {"PUTKEY" , itoa(finger[0]->keyID),nodeToString(finger[0]),nodeToString(pred),itoa(pred->keyID)};
        requestPkt = (char *)malloc(BLEN*sizeof(char));

        utilFramePacket(attr,val,requestPkt);
        printf("putKey() : Packet : \n ");

        sock=tcpConnect(finger[1]);
        sendPkt(sock,requestPkt);
	
        responsePkt=recvPkt(sock);
        close(sock);

        m1 = token(responsePkt);

        if (strcmp(m1->method,"240") ) {
		free(requestPkt);
		free(m1);	
		free(responsePkt);
                printf("Acknowledgement for putKey is received\n");
		return 0;
        } else {
		free(requestPkt);
		free(m1);	
		free(responsePkt);
                perror("Error: Ack for putKey is not received\n");
		return -1;
        }


}

int leave() { // making int to check if leave is successfull 
	struct Msg *m1 ;
	int sock;
	char  *requestPkt;
	char  *responsePkt;
	// pred should be defined global ??? l
	// leave response to be written ....
	// in leave response if leaveStbNo is 0 then it checks if the host in msg is its pred then it will update pred with its contact node
	// if the host in the msg is its succ then it updates its succ with the contact node
	leaveFlag=1;

	while( finalStabilizeComplete==0 ) {
	// Wait till stabilize is completed fully	
	}
        char *attr[15] = {"METHOD" , "ID" ,"HOST", "CONTACT","SUCC.ID" } ;
        char *val[15] = {"LEAVE" , itoa(finger[0]->keyID),nodeToString(finger[0]),nodeToString(finger[1]),itoa(finger[1]->keyID)};
        requestPkt = (char *)malloc(BLEN*sizeof(char));

        utilFramePacket(attr,val,requestPkt);
        printf("leave() : Packet : \n ");

        sock=tcpConnect(pred);
        sendPkt(sock,requestPkt);

        responsePkt=recvPkt(sock);
        close(sock);
	free(requestPkt);

        m1 = token(responsePkt);

	if (strcmp(m1->method,"230") ) {
		printf("Acknowledgement for Leave is received\n");
	} else {
		printf("Warning: Ack for Leave is not received\n");
	}

	free(responsePkt);
	free(m1);
	return putKey();
/*	
	sock = tcpConnect(finger[1]);
	printf("Socket num : %d\n",sock);
	if (sock < 0) {
		perror("Socket connection failed");
		exit(1);
	}
        sendPkt(sock,requestPkt);
	close(sock);
	requestPkt1= framePacket("LEAVE",finger[0]->keyID,finger[0],finger[1],NULL,&m2);  // Leave not yet defined in switch cases at serverLib
	// pred should be defined global ??? l
	// leave response to be written ....
	// in leave response if leaveStbNo is 0 then it checks if the host in msg is its pred then it will update pred with its contact node
	// if the host in the msg is its succ then it updates its succ with the contact node	
	sock1 = tcpConnect(pred);
	printf("Socket num : %d\n",sock1);
	if (sock1 < 0) {
		perror("Socket connection failed");
		exit(1);
	}
	sendPkt(sock1,requestPkt1);
	close(sock1);
*/
}

void stabilize() {
	char *requestPkt,*responsePkt;
	struct Msg* m2;
	int sock;
//	if (finger[0]->keyID==finger[1]->keyID) {
//		printf("stabilize(): Successor and self are same \n");
//		return;
//	}

        char *attr[15] = {"METHOD" , "ID" ,"HOST", "CONTACT" } ;
        char *val[15] = {"STAB" , itoa(finger[0]->keyID),nodeToString(finger[0]),nodeToString(finger[0])};
	requestPkt = (char *)malloc(BLEN*sizeof(char));

        utilFramePacket(attr,val,requestPkt);
        printf("stabilize()  \n ");

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
///       			pthread_mutex_lock(&tableMutex);

			finger[1]->keyID=m2->keyID;
			strcpy(finger[1]->ipstr,m2->contactIP);
			finger[1]->port=m2->contactPort;

//       			pthread_mutex_unlock(&tableMutex);
			
		}
					
	} else {
		perror("stabilize(): unrecognised status code in this module\n");
	}

	free(requestPkt);
	free(responsePkt);
	free(m2);
	notify();

	printTable();	
}

void notify() {
        char *requestPkt,*responsePkt;
        struct Msg* m2;
        int sock;

 //       if (finger[0]->keyID==finger[1]->keyID) {
 //               printf("notify(): Successor and self are same \n");
  //              return;
  //      }

        char *attr[15] = {"METHOD" , "ID" ,"HOST", "CONTACT" } ;
        char *val[15] = {"NOTIFY" , itoa(finger[0]->keyID),nodeToString(finger[0]),nodeToString(finger[0])};
        requestPkt = (char *)malloc(BLEN*sizeof(char));

        utilFramePacket(attr,val,requestPkt);
        printf("notify()  \n ");

        sock=tcpConnect(finger[1]);
        sendPkt(sock,requestPkt);

        responsePkt=recvPkt(sock);
        close(sock);
        m2 = token(responsePkt);

        if (strcmp(m2->method,"110")!=0) {
		perror("notify(): unrecognised status code in this module\n");
	}
	free(requestPkt);
	free(responsePkt);
	free(m2);
}

/****************************************rcv RFC******************************/
void rcvRFC(int sockfd, char *name ){
	FILE *wf;
        wf=fopen(name,"wa");
        
	//Receive loop          
        char *recBuf = (char *)malloc(300);
        char *recBptr; //pointer to recBuf
        int n;
        int buflen;
        recBptr=recBuf;
        buflen=300;

        printf("Received : \n");
        while ((n=recv(sockfd,recBuf,buflen,0))>0) {
		recBuf[n]='\0';
     		fputs(recBuf,wf);

        //	printf("%s",recBuf);
                	memset(recBuf,0,300);
	//	fflush(recBuf);
//		if (n<300)
//			close(sockfd);
        }	
        fclose(wf);
        printf("Fclse done\n");
	close(sockfd); 

	free(recBuf);
}

void printTable() {
	int i =0;
	printf("\nPredecesor : KeyID : %d , IP : %s , Port %d\n ",pred->keyID,pred->ipstr,pred->port);
	for (i=0;i<4;i++){
		printf("Finger : %d KeyID: %d IP : %s , Port %d\n",i,finger[i]->keyID,finger[i]->ipstr,finger[i]->port);
	}
}

int triggerSingleRFC(int id) {
	struct Node* rfcOwner;
	rfcOwner=lookup(id);
	getRFCrequest(id, rfcOwner);
	return 1;

}

int getRFCresponsible() {

	return getRFCBetween(pred->keyID,finger[0]->keyID,finger[1]);
}

int getRFCBetween(int start,int end,struct Node *n ) {
        int i=0;
        for(i=0;i<50;i++) {
                if (liesBetween(IDspace[i],start,end))
                {
                        getRFCrequest(IDspace[i], n );
                        //break;
                }
        }
        return 1;


}

int getRFCrequest(int id, struct Node* rfcOwner) {
	int sock;
	char* rfcDest;
	char* requestPkt;
	char* responsePkt;
	//struct Node* rfcOwner;
	//rfcOwner=lookup(id);
	//rfcOwner=(struct Node*)malloc(sizeof(struct Node));
	//strcpy(rfcOwner->ipstr,"127.0.0.1");
	//rfcOwner->port=5000;
	char *attr[15] = {"METHOD" , "ID" ,"HOST", "CONTACT" } ;
        char *val[15] = {"GETRFC" , itoa(id), nodeToString(finger[0]),nodeToString(rfcOwner)};
        requestPkt = (char *)malloc(BLEN*sizeof(char));

        utilFramePacket(attr,val,requestPkt);
	sock = tcpConnect(rfcOwner);
        if (sock==-1) {
                perror ("Socket is not established properly");
        }
	//free(rfcOwner);
	rfcDest=findRFCfromID(id);
	//requestPkt="hello\n";
        sendPkt(sock,requestPkt);
	printf("getRFCrequest(): A file of name %s will be created\n", rfcDest);
	rcvRFC(sock, rfcDest);
        close(sock);
	free(requestPkt);
	return 1;	

	
}

void sigchld_handler(int s)
{
	while(waitpid(-1, NULL, WNOHANG) > 0);
}

int randn() {
	int num;
	srand(time(NULL));
	num = rand()%1024;
	printf("\nthe number generated(%d)\n",num);
	return num;
}

/***************RESPONSES WRITTEN HERE**********************************/

void joinResponse (struct msgToken *msgsock){
	int sock;	
	struct Msg* str;
	//printf("printing here.......%s\n...",msgsock->ptr);
	str=token(msgsock->ptr);
	sock=msgsock->sock;
	
	printf("\nIt is in JOINRESPONSE thread now...congo...1...\n");

	int respSock;
	struct Msg *m;
	char *m1,*m2;
	str->keyID = randn();
        char *attr[15] = {"METHOD" , "ID" ,"HOST", "CONTACT" } ;
        char *val[15] = {"200" , itoa(str->keyID),nodeToString(finger[0]),"NULL:NULL"};
	m1=(char *)malloc(BLEN*sizeof(char));
        utilFramePacket(attr,val,m1);
	printf("joinResponse() \n ");
	sendPkt(sock,m1);
	close(sock);
	totalPeers++;
	printf("joinResponse(): totalPeers : %d\n",totalPeers);
	free(m1);
}

struct Node *findSuccessorServer(int id) {
}

void getResponse (struct msgToken *msgsock){
	int sock;
	int i =0;
	struct Msg* str;
	struct Node *toContact;
	char *m1;
        m1=(char *)malloc(BLEN*sizeof(char));
	str=token(msgsock->ptr);
	sock=msgsock->sock;
	printf("\nIt is in GETRESPONSE thread now...congo...2...\n");

	if (finger[0]->keyID == finger[1]->keyID) {
//For the first peer alone this condition will be true 
                char *attr[15] = {"METHOD" , "ID" ,"HOST", "CONTACT" } ;
                char *val[15] = {"200" , itoa(finger[1]->keyID),nodeToString(finger[0]),nodeToString(finger[1])};
                utilFramePacket(attr,val,m1);
                //printf("getResponse() : Packet : \n%s\n ",m1);
                sendPkt(sock,m1);
		close(sock);
                return;
	} else if (finger[1]->keyID ==0 && str->keyID > finger[0]->keyID  ) {
//The node lies between last peer and peer 0
                char *attr[15] = {"METHOD" , "ID" ,"HOST", "CONTACT" } ;
                char *val[15] = {"200" , itoa(finger[1]->keyID),nodeToString(finger[0]),nodeToString(finger[1])};
                utilFramePacket(attr,val,m1);
                //printf("getResponse() : Packet : \n%s\n ",m1);
                sendPkt(sock,m1);
		close(sock);
                return;
	} else if (finger[0]->keyID < str->keyID &&  str->keyID <= finger[1]->keyID){
//Found the actual successor . 
	        char *attr[15] = {"METHOD" , "ID" ,"HOST", "CONTACT" } ;
	        char *val[15] = {"200" , itoa(finger[1]->keyID),nodeToString(finger[0]),nodeToString(finger[1])};
	        utilFramePacket(attr,val,m1);
        	//printf("getResponse() : Packet : \n%s\n ",m1);
	        sendPkt(sock,m1);
		close(sock);
		return;
        } else {
             for (i=3; i>1; i--) {
                        if (finger[i]->keyID < str->keyID ) {
//Send shortcut contact node to the client.
                    char *attr[15] = {"METHOD" , "ID" ,"HOST", "CONTACT" } ;
                		char *val[15] = {"305" , itoa(finger[i]->keyID),nodeToString(finger[0]),nodeToString(finger[i])};
                		utilFramePacket(attr,val,m1);
		  //              printf("getResponse() : Packet : \n%s\n ",m1);
                		sendPkt(sock,m1);
				close(sock);
				return;
                        }
                }
	}

	char *attr[15] = {"METHOD" , "ID" ,"HOST", "CONTACT" } ;
	char *val[15] = {"200" , itoa(finger[0]->keyID),nodeToString(finger[0]),nodeToString(finger[0])};
        utilFramePacket(attr,val,m1);
        //printf("getResponse() : Packet : \n%s\n ",m1);
        sendPkt(sock,m1);
	close(sock);
	free(m1);
	free(str);
        return;


}


void getrfcResponse (struct msgToken *msgsock){
	int sock;
	char* id;
	char path[50]= RFC_PATH ;
	struct Msg* str;
	str=token(msgsock->ptr);
	sock=msgsock->sock;
	id=findRFCfromID(str->keyID);
	strcat(path,id);
	sendRFC(sock,path);
	printf("\nIt is in GETRFC thread now...congo...3...\n");
	free(str);
}

void stabResponse (struct msgToken *msgsock){
	int sock;
	struct Msg* str;
	char *m1;
	m1=(char *)malloc(BLEN *sizeof (char));
	str=token(msgsock->ptr);
	sock=msgsock->sock;

	printf("\nIt is in STAB thread now...congo...4...\n");
	
        char *attr[15] = {"METHOD" , "ID" ,"HOST", "CONTACT" } ;

	if (pred->keyID==-1) {
		if(finger[0]->keyID==0 && count==0){
		count++;
		finger[1]->keyID=str->keyID;
	                strcpy(finger[1]->ipstr,str->contactIP);
        	        finger[1]->port=str->contactPort;

		}
        	char *val[15] = {"100" , itoa(finger[0]->keyID),nodeToString(finger[0]),nodeToString(finger[1])};//
        	utilFramePacket(attr,val,m1);
	} else {
	        char *val[15] = {"100" , itoa(pred->keyID),nodeToString(finger[0]),nodeToString(pred)};
        	utilFramePacket(attr,val,m1);
	}
        printf("stabResponse() \n");
        sendPkt(sock,m1);
	close(sock);

	free(m1);
	free(str);
        return;

}

void notifyResponse (struct msgToken *msgsock){
        int sock;
        struct Msg* str;
        char *m1;
        m1=(char *)malloc(BLEN *sizeof (char));
        str=token(msgsock->ptr);
        sock=msgsock->sock;

        printf("\nIt is in Notify thread now...congo...4...\n");

//        pthread_mutex_lock(&tableMutex);

	if (pred->keyID==-1) {
		pred->keyID=str->keyID;
		strcpy(pred->ipstr,str->contactIP);
       	        pred->port=str->contactPort;
	} else {
		if(liesBetween(str->keyID,pred->keyID,finger[0]->keyID)==1) {
				printf("Pred lies between its own pred and self\n");
	                pred->keyID=str->keyID;
	                strcpy(pred->ipstr,str->contactIP);
        	        pred->port=str->contactPort;
	
		}
	}

//        pthread_mutex_unlock(&tableMutex);

        char *attr[15] = {"METHOD" , "ID" ,"HOST", "CONTACT" } ;

        char *val[15] = {"110" , itoa(finger[0]->keyID),nodeToString(finger[0]),nodeToString(finger[0])};
        utilFramePacket(attr,val,m1);
        printf("notifyResponse() \n ");
        sendPkt(sock,m1);
	close(sock);
	printTable();
	free(m1);
	free(str);
        return;

}

/*
struct Msg* token(char *str1)
{

        struct Msg* rcvMsg;
        char str[300];
        strcpy(str,str1);
        printf("\nThis is the printed PPpassed value :\n%s \n",str);
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
       printf("\n SUCCESSFULLY TOKENIZED \n");
        return rcvMsg;
}

*/


int Action(struct msgToken* msgsock){
	struct Msg* gotMsg;
        gotMsg=token(msgsock->ptr);
	printf("Action(): msgsock->ptr %s\n", msgsock->ptr);
        int n,ret;

        char *msg1, *join,*get,*getrfc,*stablize,*notify,*leaveMethod,*putKey,*goHome;
        join="JOIN";
        get="GET";
        getrfc="GETRFC";
        stablize="STAB";
	notify = "NOTIFY" ;
	leaveMethod = "LEAVE" ;
	goHome="GOHOME";
	putKey="PUTKEY";

        /*DECISION SECTION*/
            		if(strcmp(gotMsg->method,join)==0){
                                printf("A new node is joining......\n");
                                joinResponse(msgsock);
                        }
                        else if(strcmp(gotMsg->method,get)==0){
                                printf("GET request received......\n");
                                getResponse(msgsock);
                        }
                        else if(strcmp(gotMsg->method,getrfc)==0){
                                printf("Sending RFC......\n");
                                getrfcResponse(msgsock);
                        }
                        else if(strcmp(gotMsg->method,stablize)==0){
                                printf("STABILIZING......\n");
                                stabResponse(msgsock);
                        }
                        else if(strcmp(gotMsg->method,notify)==0){
                                printf("Got Notify......\n");
                                notifyResponse(msgsock);
                        }
                        else if(strcmp(gotMsg->method,leaveMethod )==0){
                                printf("Got leave......\n");
                                leaveResponse(msgsock);
                        }
                        else if(strcmp(gotMsg->method,putKey)==0){
                                printf("Got putkey......\n");
                                putKeyResponse(msgsock);
                        }
                        else if(strcmp(gotMsg->method,goHome)==0){
                                printf("Got gohome......\n");
				leave();
                        }




	return 1;

}


int tcpServer(struct Node *n)
{	pthread_t threadID[20];
//	struct serverParm *parmPtr;
	int *serverFd;
	int threadCount=0;	
	char *msg1;
	struct msgToken* msgsock;
	int sockfd, new_fd, ret;  // listen on sock_fd, new connection on new_fd
	struct addrinfo hints, *servinfo, *p;/* hints are hints for get  addr info servinfo is server information and p is just used as a pointer*/
	struct sockaddr_storage their_addr; // connector's address information
	socklen_t sin_size;
	struct sigaction sa;
	int yes=1;
	char s[INET6_ADDRSTRLEN];
	int rv;
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE; // use my IP
	if ((rv = getaddrinfo(NULL,itoa(n->port), &hints, &servinfo)) != 0) {

		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	sockfd = socket(servinfo->ai_family,servinfo->ai_socktype,servinfo->ai_protocol);


	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,sizeof(int)) == -1) {
		perror("setsockopt");
		exit(1);
	}

	if (bind(sockfd, servinfo->ai_addr, servinfo->ai_addrlen) == -1) {
		close(sockfd);
		perror("server: bind");
		
	}

	if (servinfo == NULL)  {
		fprintf(stderr, "server: failed to bind\n");
		return 2;
	}

	freeaddrinfo(servinfo); // all done with this structure

	if (listen(sockfd,LISTENQUEUE) == -1) {
		perror("listen");
		exit(1);
	}

	sa.sa_handler = sigchld_handler; // reap all dead processes
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, NULL) == -1) {
		perror("sigaction");
		exit(1);
	}

	printf("server: waiting for connections...\n");

    
	while(1) {  // main accept() loop
		sin_size = sizeof their_addr;
		new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
		if (new_fd == -1) {
			perror("accept");
			continue;
		}

	//	parmPtr = (struct serverParm *)malloc(sizeof(struct serverParm));
	//	parmPtr->sockDes = new_fd;
		serverFd = (int *)malloc(sizeof(int));
		*serverFd = new_fd;
		if(pthread_create(&threadID[threadCount],NULL,serverThread,(void *)serverFd)!=0){
			printf("cannot create thread\n");
		//close(new_fd);		
		}
/*
		if (!fork()) { // this is the child process
			close(sockfd); // child doesn't need the listener
			msg1=malloc(BLEN);
			msg1=recvPkt(new_fd);
			msgsock =(struct msgToken*)malloc(sizeof(struct msgToken));
        		msgsock->ptr=msg1;
        		msgsock->sock=new_fd;
			printf("The message msg1 is : %s\n", msg1);
			Action(msgsock);
			close(new_fd);
			exit(0);
		}*/
		//close(new_fd);  // parent doesn't need this
		threadCount = (threadCount+1)%20;

	}
	printf("IT IS GETTING OUT OF TCP SERVER\n");
	return 0;
}


char* findRFCfromID (int id) {
	int i=0;
	while (id != IDspace[i]){
		i++;
	}
	return RFCnames[i];
}

/********************************send RFC*****************************************/
void sendRFC(int new_fd,char *name) {
//	pthread_mutex_lock( &mutex2 );
	FILE *rf;
        int rlen;
//	char *send_data='\0';
        rf=fopen(name,"r");
 //       send_data=(char *)malloc(300);
        int i=2;
        char seek[300];
        rlen=300;     
       	do {
                memset(seek,0,300);
                rlen=fread(seek,1,300,rf);
		seek[300]='\0';
             	//strcat(send_data,seek);
                //send_data=(char *)realloc(send_data, i*600*sizeof(char));
		send(new_fd,seek,strlen(seek), 0);
               //	i++;
                printf("\nThe value of rlen is %d\n",rlen);
        } while( rlen==300);
//	rlen=strlen(send_data);
	printf("\nThe value of rlen is %d\n",rlen);
        //send_data[rlen]='\0';
//	printf(send_data);
	//send(new_fd,send_data,strlen(send_data), 0);
        sleep(2);
	memset(seek,0,300);
//	memset(send_data,0,strlen(send_data));
//	free(send_data);
      	close(new_fd);	
//	pthread_mutex_unlock( &mutex2 ); 
	
}
	
/********************************send RFC*****************************************/

void *serverThread (void *a){
//	struct serverParm *parmPtr;
//	parmPtr=a;
	char *msg1;
	struct msgToken* msgsock;
	int *temp = (int *)a;
	int sockDesc = *temp;
	printf("Socket descriptor : %d\n",sockDesc);
	//close(sockfd); // child doesn't need the listener
	msg1=malloc(BLEN);
	msg1=recvPkt(sockDesc);
	msgsock =(struct msgToken*)malloc(sizeof(struct msgToken));
        msgsock->ptr=msg1;
        msgsock->sock=sockDesc;
	//printf("The message msg1 is : %s\n", msg1);
	Action(msgsock);
	close(sockDesc);
	free(a);
			
}

void fixFingers(){

//        pthread_mutex_lock(&tableMutex);

//        struct Node *temp = lookup(finger[0]->keyID + 1);
//        finger[1] = temp;
//        if ( finger[1]==NULL ) {
//                perror("First finger is NULL\n");
//                exit(1);
 //       }

  //      if (debug==1) {
  //              printf("fixFingers() : Actual Successor info: %s , keyID : %d",nodeToString(finger[1]),finger[1]->keyID);
  //      }
        //Set of instructions that set the finger table
        //Each time a lookup is performed a check is performed to check if 
        //the current finger is also the next finger
        if (finger[1]->keyID < (finger[0]->keyID + 2)  && !(liesBetween((finger[0]->keyID + 2),finger[0]->keyID,finger[1]->keyID)) ) {
                printf("lookup(finger[2]): finger[1].keyID : finger[0]->keyID +2: %d %d \n", finger[1]->keyID, finger[0]->keyID + 2);
                finger[2] = lookup((finger[0]->keyID + 2));
        }
        else
                finger[2] = finger[1];

        if (finger[2]->keyID < (finger[0]->keyID + 4)  && !(liesBetween((finger[0]->keyID + 4),finger[0]->keyID,finger[1]->keyID)) ) {
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

//       pthread_mutex_unlock(&tableMutex);
       return 0;

}

char *itoa(int num) {
        char *str;
        str = (char *)malloc(5);
        sprintf(str,"%d",num);
        return str;
}
char *nodeToString(struct Node *n) {
        char *ipPortStr = (char *)malloc(INET6_ADDRSTRLEN + 10);
        char *portStr;
        strcpy(ipPortStr,n->ipstr);
        strcat(ipPortStr,":");

        portStr = itoa(n->port);
//        if (debug == 1) {
//                printf("nodeToString() :From itoa IP String : %s ipPortStr : %s\n",portStr,ipPortStr);
//        }
        strcat(ipPortStr,portStr);
        free(portStr);
//        if (debug == 1) {
//                printf("nodeToString(): IP String : %s\n",ipPortStr);
//        }
        return ipPortStr;
}

int leaveResponse (struct msgToken *msgsock) {
        int sock;
        struct Msg* str;
        char *m1;
        m1=(char *)malloc(BLEN *sizeof (char));
        str=token(msgsock->ptr);
        sock=msgsock->sock;

        printf("\nIt is in leave thread now..\n");

//        pthread_mutex_lock(&tableMutex);

        finger[1]->keyID=str->succID;
        strcpy(finger[1]->ipstr,str->contactIP);
        finger[1]->port=str->contactPort;


//        pthread_mutex_unlock(&tableMutex);

	printTable();
        char *attr[15] = {"METHOD" , "ID" ,"HOST", "CONTACT" } ;

        char *val[15] = {"230" , itoa(finger[0]->keyID),nodeToString(finger[0]),"0.0.0.0:0" };
        utilFramePacket(attr,val,m1);
        printf("leaveResponse() \n");
        sendPkt(sock,m1);
        close(sock);
	free(m1);

	return 0;
}

int putKeyResponse (struct msgToken *msgsock) {
        int sock;
        struct Msg* str;
        char *m1;
        m1=(char *)malloc(BLEN *sizeof (char));
        str=token(msgsock->ptr);
        sock=msgsock->sock;

        printf("\nIt is in putKey thread now..\n");

//pred is copied to tempNode since stabilization thread may change the pred before getRFCBetween completes
	struct Node *tempNode;
	tempNode=(struct Node *)malloc(sizeof(struct Node));
	tempNode->keyID=pred->keyID;
	tempNode->port=pred->port;
	strcpy(tempNode->ipstr,pred->ipstr);


//Get all the RFC's for which the leaving node is responsible

	getRFCBetween(str->keyID,finger[0]->keyID,tempNode);
        
//        pthread_mutex_lock(&tableMutex);

	pred->keyID=str->predID;
        strcpy(pred->ipstr,str->contactIP);
        pred->port=str->contactPort;

//        pthread_mutex_unlock(&tableMutex);

	printTable();
        char *attr[15] = {"METHOD" , "ID" ,"HOST", "CONTACT" } ;

        char *val[15] = {"240" , itoa(finger[0]->keyID),nodeToString(finger[0]),"0.0.0.0:0" };
        utilFramePacket(attr,val,m1);
        printf("putKeyResponse()\n");
        sendPkt(sock,m1);
        close(sock);
        free(m1);
	free(tempNode);
        return 0;
}

