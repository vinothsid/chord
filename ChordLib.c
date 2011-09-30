#include "ChordLib.h"
#include <pthread.h>
extern struct Node* origin;
extern struct Node* finger[5];
extern int totalPeers;
extern int leaveFlag;
extern int finalStabilizeComplete;
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t tableMutex = PTHREAD_MUTEX_INITIALIZER;

int tempCount = -1;
int tempIDspace[10] = {1,2,3,4,5,150,300,1022};

int IDspace[50] = {1023,646,469,57,668,361,759,953,122,5,702,173,994,675,893,328,995,232,971,531,354,947,205,604,413,20,440,885,743,821,15,249,277,17,235,451,21,238,599,809,319,585,894,55,924,497,183,411,670,658};

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

//	printf("To check special case\n");
//	return finger[1];

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
	
//When all other peers are left and only peer zero is left the following condition applies

//	if(finger[0]->keyID==0 && finger[1]->keyID==0 ) {
//		return origin;
//	}

	for ( i=1;i<4;i++ ) {
		if (finger[i]->keyID==-1   ) {
			nilCounter++;
		}
		else {
			//printf("not -1 for index:%d\n",i);
		}
	}

	//printf("nilCounter : %d\n",nilCounter);
//The below statements execute only when all finger's keyID are not -1	
	if ( nilCounter==3 ) {
		return origin;
	}


	if (liesBetween(id, finger[0]->keyID , finger[1]->keyID)){
		return finger[1];
	} else {

		for (i=3; i>=1; i--) {
//If finger[i]->keyID is -1 ,continue the loop without returning . Finally the highest finger with proper value is returned in the if clause.(return finger[i])
			if (finger[i]->keyID==-1) {
				continue;
			}
			if (liesBetween(id,finger[i]->keyID,finger[i+1]->keyID )) {
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

//finger[4] should not be used .It is not part of finger table.Its only place holder for peer zero.
	finger[4]=origin;
//Initialise the self values
	strcpy(finger[0]->ipstr,ip);
	finger[0]->port = port;

}

/********************************************* JOIN **************************************************/

int join (char *ip,int port) {
	//struct Node n;
	struct Msg  *m2;
	int sock;
	char  *requestPkt;
	char  *responsePkt;
	initFingerTable(ip,port);
	if (debug ==1 ) {
		printf("join()\n");
	}	
	pred->keyID=-1;
	requestPkt= framePacket("JOIN",-1,finger[0],origin);
	
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
	finger[1]->keyID = temp->keyID;
	finger[1]->port = temp->port;
	strcpy(finger[1]->ipstr,temp->ipstr); 
	if ( finger[1]==NULL ) {
		perror("First finger is NULL\n");
		exit(1);
	}
//	printf("\n1111111111\n");	

	if (debug==1) {
		printf("join() : Actual Successor info, keyID : %d",finger[1]->keyID);
	}
	//Set of instructions that set the finger table
	//Each time a lookup is performed a check is performed to check if 
	//the current finger is also the next finger


	if (finger[1]->keyID < ((finger[0]->keyID + 2)) % 1024 && !(liesBetween((finger[0]->keyID + 2),finger[0]->keyID,finger[1]->keyID))) {
		printf("lookup(finger[2]): finger[1].keyID : finger[0]->keyID +2: %d %d \n", finger[1]->keyID, finger[0]->keyID + 2);
		temp = lookup((finger[0]->keyID + 2));
	        finger[2]->keyID = temp->keyID;
        	finger[2]->port = temp->port;
        	strcpy(finger[2]->ipstr,temp->ipstr);

	}
	else 
		finger[2] = finger[1];
//	printf("2222222\n");
        if (finger[2]->keyID < ((finger[0]->keyID + 4))%1024 && !(liesBetween((finger[0]->keyID + 4),finger[0]->keyID,finger[1]->keyID))) {
		printf("lookup(finger[3]): finger[2].keyID : finger[0]->keyID +4: %d %d \n", finger[1]->keyID, finger[0]->keyID + 4);
                temp = lookup((finger[0]->keyID + 4));

	        finger[3]->keyID = temp->keyID;
        	finger[3]->port = temp->port;
	        strcpy(finger[3]->ipstr,temp->ipstr);

        }
        else
                finger[3] = finger[2]; 


//When following free statements are executed , its seg faulting
	free(requestPkt);
	free(responsePkt);
	free(m2);
	free(temp);
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
	struct Msg  *m2;
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
	
	requestPkt=framePacket("GET",id, finger[0], n2);		
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
		requestPkt = framePacket("GET", id, finger[0], n3);
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
			printf("lookup() : Actual Successor info :  , keyId: %d\n\n",n->keyID);
		}

//When following free statements are executed seg fault occuring
		free(requestPkt);
		free(responsePkt);
		free(m2);
		free(n2);
		free(n3);
		return n;

	}

//When following free statements are executed seg fault occuring
//	free(requestPkt);
//	free(responsePkt);
//	free(m1);
//	free(m2);
	printf("lookup failed and returning NULL\n");
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

char* framePacket(char* method,short int keyID, struct  Node* hostNode, struct Node* contactNode ) {
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
		char *attr[15] = {"METHOD" , "ID" ,"HOST", "CONTACT" } ;
		char *tempID = itoa(keyID);
		char *val[15] = {method , tempID ,host,contact}; 
		//printf("after val\n");	
		pkt = (char *)malloc(BLEN*sizeof(char));
		utilFramePacket(attr,val,pkt);
		free(tempID);
		if (debug == 1) {
			//printf("framePacket(): returned from utilFramePacket()\n");
		}
		
	
	

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
		printTime();	
		printf("sendPkt() : Sending pkt:\n%s\n",buf);
	}
	int total = 0;        // how many bytes we've sent
   	int bytesleft = strlen(buf); // how many we have left to send
    	int n;
	char *tempBuf=buf;

    	while(total < strlen(buf)) {
        	n = send(sock, tempBuf+total, bytesleft, 0);
        	if (n == -1) { break; }
        		total += n;
        		bytesleft -= n;
    		}

   // *len = total; // return number actually sent here

    //return n==-1?-1:0; // return -1 on failure, 0 on success
	//free(tempBuf);
	//free(buf);
	return 1;

//	return send(sock, buf, strlen(buf) ,0);
}

/***************************************** RECEIVE PACKET ***************************************/

char *recvPkt(int sock) {
	char *recBuf = (char *)calloc(BLEN,sizeof(char));
	
        char *recBptr; //pointer to recBuf
        int n;
        int buflen;
	recBptr=recBuf;
        buflen=BLEN;
	
//	pthread_mutex_lock(&tableMutex);
	while ((n=recv(sock,recBptr,buflen,0))>0) {
                buflen -=n;
	/*	if (debug == 1) {
			recBptr[n]='\0';
			printf("Received pkt:%s Num of bytes received: %d\n",recBptr,n);
        	}       */
                recBptr +=n;
	//	recBptr++;

//In case we get erroneous packet of small size
		if (recBuf[10]!='\0' )
			break;

        }

//	pthread_mutex_unlock(&tableMutex);
	recBuf[recBptr-recBuf+1] = '\0';
	if (debug == 1) {
		printTime();
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

	if (start==end)
		return 0;  
	id=id%1024;
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
	char *tempID = itoa(finger[0]->keyID);
	char *tempStr1 = nodeToString(finger[0]);
	char *tempStr2 = nodeToString(pred);
        char *val[15] = {"PUTKEY" , tempID ,tempStr1,tempStr2,itoa(pred->keyID)};
        requestPkt = (char *)malloc(BLEN*sizeof(char));

        utilFramePacket(attr,val,requestPkt);
        printf("putKey() : Packet : \n ");
	free(tempID);
	free(tempStr1);
	free(tempStr2);
        sock=tcpConnect(finger[1]);
        sendPkt(sock,requestPkt);
	
        responsePkt=recvPkt(sock);
        close(sock);

        m1 = token(responsePkt);

        if (strcmp(m1->method,"240")==0 ) {
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
	char *tempID =  itoa(finger[0]->keyID);
	char *tempStr1 = nodeToString(finger[0]);
	char *tempStr2 = nodeToString(finger[1]);
        char *val[15] = {"LEAVE" , tempID,tempStr1,tempStr2,itoa(finger[1]->keyID)};
        requestPkt = (char *)malloc(BLEN*sizeof(char));

        utilFramePacket(attr,val,requestPkt);
        printf("leave() : Packet : \n ");
	free(tempID);
	free(tempStr1);
	free(tempStr2);
        sock=tcpConnect(pred);
        sendPkt(sock,requestPkt);

        responsePkt=recvPkt(sock);
        close(sock);

        m1 = token(responsePkt);

	if (strcmp(m1->method,"230") ) {
		printf("Acknowledgement for Leave is received\n");
	} else {
		printf("Warning: Ack for Leave is not received\n");
	}

	free(requestPkt);
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
	char *tempID = itoa(finger[0]->keyID);
        char *tempStr1 = nodeToString(finger[0]);
        

        char *val[15] = {"STAB" , tempID ,tempStr1,tempStr1};
	requestPkt = (char *)malloc(BLEN*sizeof(char));

        utilFramePacket(attr,val,requestPkt);
        printf("stabilize(): sending pkt to %d  \n ",finger[1]->keyID);

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
	free(tempID);
	free(tempStr1);
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
	char *tempID =  itoa(finger[0]->keyID);
        char *tempStr1 = nodeToString(finger[0]);


        char *val[15] = {"NOTIFY" , tempID ,tempStr1,tempStr1};
        requestPkt = (char *)malloc(BLEN*sizeof(char));

        utilFramePacket(attr,val,requestPkt);
        printf("notify()  \n ");
	free(tempID);
	free(tempStr1);
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
	return;
}

/****************************************rcv RFC******************************/
void rcvRFC(int sockfd, char *name ){
	FILE *wf;
        wf=fopen(name,"wa");
        
	//Receive loop          
        char *recBuf = (char *)malloc(301);
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
	return;
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
	free(rfcOwner);
	return 1;

}

int getRFCresponsible() {

	return getRFCBetween(pred->keyID,finger[0]->keyID,finger[1]);
}

int getRFCBetween(int start,int end,struct Node *n ) {
        int i=0;
	if ( debug==1) {
		printf("Getting RFC between %d and %d from node %d\n",start,end,n->keyID);
	}
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
	char *tempID = itoa(id);
        char *tempStr1 = nodeToString(finger[0]);
        char *tempStr2 = nodeToString(rfcOwner);

        char *val[15] = {"GETRFC" , tempID , tempStr1,tempStr2};
        requestPkt = (char *)malloc(BLEN*sizeof(char));

        utilFramePacket(attr,val,requestPkt);
	free(tempID);
	free(tempStr1);
	free(tempStr2);
	sock = tcpConnect(rfcOwner);
        if (sock==-1) {
                perror ("Socket is not established properly");
        }
	//free(rfcOwner);
	rfcDest=findRFCfromID(id);
	//requestPkt="hello\n";
        sendPkt(sock,requestPkt);
	printf("getRFCrequest(): Receiving file %s corresponding to id %d from node %d \n", rfcDest,id,rfcOwner->keyID);
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

//For testing purpose uncomment following lines . tempIDspace and tempCount are already defined globally

//	tempCount++;
//	return tempIDspace[tempCount];
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
	char *tempID =  itoa(str->keyID) ;
        char *tempStr1 = nodeToString(finger[0]);

        char *val[15] = {"200" , tempID ,tempStr1,"NULL:NULL"};
	m1=(char *)malloc(BLEN*sizeof(char));
        utilFramePacket(attr,val,m1);
	printf("joinResponse() \n ");
	free(tempID);
	free(tempStr1);
	sendPkt(sock,m1);
	close(sock);
	totalPeers++;
	printf("joinResponse(): totalPeers : %d\n",totalPeers);
	free(m1);
	free(str);
	return;
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
	char *tempID=NULL;
	char *tempStr1=NULL;
	char *tempStr2=NULL;
	if (finger[0]->keyID == finger[1]->keyID) {
//For the first peer alone this condition will be true 
                char *attr[15] = {"METHOD" , "ID" ,"HOST", "CONTACT" } ;
		tempID = itoa(finger[1]->keyID);
	        tempStr1 = nodeToString(finger[0]);
        	tempStr2 = nodeToString(finger[1]);

                char *val[15] = {"200" , tempID ,tempStr1,tempStr2};
                utilFramePacket(attr,val,m1);
                //printf("getResponse() : Packet : \n%s\n ",m1);
                sendPkt(sock,m1);
		close(sock);
		free(m1);
		free(str);
		free(tempStr1);
		free(tempStr2);
		free(tempID);
                return;
	} else if (finger[1]->keyID ==0 && str->keyID > finger[0]->keyID  ) {
//The node lies between last peer and peer 0
                char *attr[15] = {"METHOD" , "ID" ,"HOST", "CONTACT" } ;
		tempID = itoa(finger[1]->keyID);
                tempStr1 = nodeToString(finger[0]);
                tempStr2 = nodeToString(finger[1]);
                char *val[15] = {"200" , tempID ,tempStr1,tempStr2};
                utilFramePacket(attr,val,m1);
                //printf("getResponse() : Packet : \n%s\n ",m1);
                sendPkt(sock,m1);
		close(sock);
		free(m1);
		free(str);
		free(tempStr1);
		free(tempStr2);
		free(tempID);
                return;
	} else if (liesBetween(str->keyID,finger[0]->keyID,finger[1]->keyID) ) {
//Found the actual successor . 
	        char *attr[15] = {"METHOD" , "ID" ,"HOST", "CONTACT" } ;
		tempID = itoa(finger[1]->keyID);
                tempStr1 = nodeToString(finger[0]);
                tempStr2 = nodeToString(finger[1]);
	        char *val[15] = {"200" , tempID ,tempStr1,tempStr2};
	        utilFramePacket(attr,val,m1);
        	//printf("getResponse() : Packet : \n%s\n ",m1);
	        sendPkt(sock,m1);
		close(sock);
		free(m1);
		free(str);
		free(tempStr1);
		free(tempStr2);
		free(tempID);
		return;
        } else {

/*major change
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

*/
		if ( liesBetween(str->keyID,finger[3]->keyID,0)) {
		        char *attr[15] = {"METHOD" , "ID" ,"HOST", "CONTACT" } ;
			tempID = itoa(finger[3]->keyID);
	                tempStr1 = nodeToString(finger[0]);
        	        tempStr2 = nodeToString(finger[3]);

                	char *val[15] = {"305" , tempID,tempStr1,tempStr2};
                        utilFramePacket(attr,val,m1);
                  //              printf("getResponse() : Packet : \n%s\n ",m1);
                        sendPkt(sock,m1);
                        close(sock);
			free(m1);
			free(str);
			free(tempStr1);
			free(tempStr2);
			free(tempID);
                        return;
                }

                if ( liesBetween(str->keyID,finger[2]->keyID,finger[3]->keyID)) {
                        char *attr[15] = {"METHOD" , "ID" ,"HOST", "CONTACT" } ;
			tempID = itoa(finger[2]->keyID);
	                tempStr1 = nodeToString(finger[0]);
        	        tempStr2 = nodeToString(finger[2]);

                        char *val[15] = {"305" , tempID ,tempStr1,tempStr2};
                        utilFramePacket(attr,val,m1);
                  //              printf("getResponse() : Packet : \n%s\n ",m1);
                        sendPkt(sock,m1);
                        close(sock);
			free(m1);
			free(str);
			free(tempStr1);
			free(tempStr2);
			free(tempID);
                        return;
                }


                if ( liesBetween(str->keyID,finger[1]->keyID,finger[2]->keyID)) {
                        char *attr[15] = {"METHOD" , "ID" ,"HOST", "CONTACT" } ;
			tempID = itoa(finger[1]->keyID);
	                tempStr1 = nodeToString(finger[0]);
        	        tempStr2 = nodeToString(finger[1]);

                        char *val[15] = {"305" , tempID , tempStr1,tempStr2};
                        utilFramePacket(attr,val,m1);
                  //              printf("getResponse() : Packet : \n%s\n ",m1);
                        sendPkt(sock,m1);
                        close(sock);
			free(m1);
			free(str);
			free(tempStr1);
			free(tempStr2);
			free(tempID);
                        return;
                }


	}

	char *attr[15] = {"METHOD" , "ID" ,"HOST", "CONTACT" } ;
	tempID = itoa(finger[0]->keyID);
        tempStr1 = nodeToString(finger[0]);

	char *val[15] = {"200" , tempID , tempStr1,tempStr1};
        utilFramePacket(attr,val,m1);
        //printf("getResponse() : Packet : \n%s\n ",m1);
        sendPkt(sock,m1);
	close(sock);
	free(m1);
	free(str);
	free(tempStr1);
	free(tempID);
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
	char *m1,*tempID;
	char *tempStr1=NULL;
	char *tempStr2=NULL;
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
		tempID = itoa(finger[0]->keyID);
                tempStr1 = nodeToString(finger[0]);
                tempStr2 = nodeToString(finger[0]);

        	char *val[15] = {"100" , tempID,tempStr1,tempStr2};//
        	utilFramePacket(attr,val,m1);
	} else {
		tempID = itoa(pred->keyID);
		if (!leaveFlag ) {
	                tempStr1 = nodeToString(finger[0]);
        	        tempStr2 = nodeToString(pred);

		        char *val[15] = {"100" ,tempID , tempStr1,tempStr2};
	        	utilFramePacket(attr,val,m1);
		} else {
	                tempStr1 = nodeToString(finger[0]);
        	        tempStr2 = nodeToString(finger[1]);

                        char *val[15] = {"100" , tempID ,tempStr1, tempStr2};
                        utilFramePacket(attr,val,m1);
		}
	}
        printf("stabResponse() \n");
        sendPkt(sock,m1);
	close(sock);

	free(m1);
	free(str);
	free(tempStr1);
	free(tempStr2);
	free(tempID);
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
	char *tempID = itoa(finger[0]->keyID);
        char *tempStr1 = nodeToString(finger[0]);

        char *val[15] = {"110" , tempID , tempStr1 , tempStr1};
        utilFramePacket(attr,val,m1);
        printf("notifyResponse() \n ");
        sendPkt(sock,m1);
	close(sock);
	printTable();
	free(m1);
	free(str);
	free(tempStr1);
	free(tempID);
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



	free(gotMsg);
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

	char *tempPort = itoa(n->port);
	if ((rv = getaddrinfo(NULL,tempPort, &hints, &servinfo)) != 0) {

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
	free(tempPort);
    
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
	//msg1=malloc(BLEN);
	msg1=recvPkt(sockDesc);
	msgsock =(struct msgToken*)malloc(sizeof(struct msgToken));
        msgsock->ptr=msg1;
        msgsock->sock=sockDesc;
	//printf("The message msg1 is : %s\n", msg1);
	Action(msgsock);
	close(sockDesc);
	free(a);
	free(msgsock->ptr);
	free(msgsock);
	return;
			
}

void fixFingers(){

	struct Node *temp=NULL;
	  
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
        if (finger[1]->keyID < ((finger[0]->keyID + 2))%1024  && !(liesBetween((finger[0]->keyID + 2),finger[0]->keyID,finger[1]->keyID)) ) {
          	temp=lookup((finger[0]->keyID + 2));
                printf("lookup(finger[2]): finger[1].keyID : finger[0]->keyID +2: %d %d \n", finger[1]->keyID, finger[0]->keyID + 2);
	        finger[2]->keyID = temp->keyID;
	        finger[2]->port = temp->port;
        	strcpy(finger[2]->ipstr,temp->ipstr);
        }
        else
                finger[2] = finger[1];

        if (finger[2]->keyID < ((finger[0]->keyID + 4))%1024  && !(liesBetween((finger[0]->keyID + 4),finger[0]->keyID,finger[1]->keyID)) ) {
                printf("lookup(finger[3]): finger[2].keyID : finger[0]->keyID +4: %d %d \n", finger[1]->keyID, finger[0]->keyID + 4);
                temp = lookup((finger[0]->keyID + 4));
	        finger[3]->keyID = temp->keyID;
        	finger[3]->port = temp->port;
	        strcpy(finger[3]->ipstr,temp->ipstr);
        }
        else
                finger[3] = finger[2];

       /* if (finger[3]->keyID < (finger[0]->keyID + 8)) {
                finger[4] = lookup((finger[0]->keyID + 8));
        }
        else
                finger[4] = finger[3];*/

//       pthread_mutex_unlock(&tableMutex);

	if ( temp!=NULL) {
		free(temp);
	}
       //return 0;

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

	char *tempID = itoa(finger[0]->keyID) ;
	char *tempStr1 = nodeToString(finger[0]) ;
        char *val[15] = {"230" , tempID , tempStr1,"0.0.0.0:0" };
        utilFramePacket(attr,val,m1);
        printf("leaveResponse() \n");
        sendPkt(sock,m1);
        close(sock);

	free(m1);
	free(str);
	free(tempID);
	free(tempStr1);
	return 0;
}

int putKeyResponse (struct msgToken *msgsock) {
        int sock;
        struct Msg* str;
        char *m1;
        m1=(char *)malloc(BLEN *sizeof (char));
        str=token(msgsock->ptr);
        sock=msgsock->sock;

        printf("\nIt is in putKeyResponse thread now..\n");

//Host ip and host port are used to create to tempNode since stabilization thread may change the pred before getRFCBetween completes
	struct Node *tempNode;
	tempNode=(struct Node *)malloc(sizeof(struct Node));
	tempNode->keyID=str->keyID;
	tempNode->port=str->hostPort;
	strcpy(tempNode->ipstr,str->hostIP);


//        pthread_mutex_lock(&tableMutex);

	pred->keyID=str->predID;
        strcpy(pred->ipstr,str->contactIP);
        pred->port=str->contactPort;

//        pthread_mutex_unlock(&tableMutex);

        

	printTable();
        char *attr[15] = {"METHOD" , "ID" ,"HOST", "CONTACT" } ;

	char *tempID = itoa(finger[0]->keyID) ;
	char *tempStr1 = nodeToString(finger[0]);
        char *val[15] = {"240" , tempID , tempStr1 ,"0.0.0.0:0" };
        utilFramePacket(attr,val,m1);
        printf("putKeyResponse()\n");
        sendPkt(sock,m1);
        close(sock);
//Get all the RFC's for which the leaving node is responsible

	getRFCBetween(str->predID,str->keyID,tempNode);
        free(m1);
        free(str);
	free(tempNode);
	free(tempID);
	free(tempStr1);
        return 0;
}

int printTime() {
	char buffer[30];
	struct timeval tv;

	time_t curtime;



	gettimeofday(&tv, NULL);
	curtime=tv.tv_sec;

	strftime(buffer,30,"%m-%d-%Y  %T.",localtime(&curtime));
	printf("%s%ld\n",buffer,tv.tv_usec);

	return 0;

}
