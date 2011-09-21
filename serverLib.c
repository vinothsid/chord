#include "serverLib.h"
#include <stdlib.h>
#define port "6000"
#define back 10
#define BLEN 300

//int IDspace[50] = {165,646,469,57,668,361,759,953,122,5,702,173,994,675,893,328,995,232,971,531,354,947,20,604,413,20,440,885,743,821,15,249,277,17,235,451,21,238,599,809,319,585,894,55,924,497,183,411,670,658};

//char* RFCnames[50] = {"rfc4261","rfc3718","rfc1493","rfc3129","rfc2716","rfc4457","rfc2807","rfc1977","rfc2170","rfc1029","rfc3774","rfc1197","rfc4066","rfc4771","rfc2941","rfc1352","rfc4067","rfc5352","rfc4043","rfc3603","rfc5474","rfc5043","rfc5140","rfc3676","rfc2461","rfc3092","rfc1464","rfc1909","rfc2791","rfc2869","rfc3087","rfc4345","rfc3349","rfc3089","rfc5355","rfc5571","rfc3093","rfc2286","rfc3671","rfc3881","rfc5439","rfc1609","rfc1918","rfc4151","rfc1948","rfc4593","rfc1207","rfc4507","rfc4766","rfc4754"};

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
	printf("printing here.......%s\n...",msgsock->ptr);
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
	printf("joinResponse() : Packet : \n%s\n ",m1);
	sendPkt(sock,m1);
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
                printf("getResponse() : Packet : \n%s\n ",m1);
                sendPkt(sock,m1);
                return;
	} else if (finger[1]->keyID ==0 && str->keyID > finger[0]->keyID  ) {
//The node lies between last peer and peer 0
                char *attr[15] = {"METHOD" , "ID" ,"HOST", "CONTACT" } ;
                char *val[15] = {"200" , itoa(finger[1]->keyID),nodeToString(finger[0]),nodeToString(finger[1])};
                utilFramePacket(attr,val,m1);
                printf("getResponse() : Packet : \n%s\n ",m1);
                sendPkt(sock,m1);
                return;
	} else if (finger[0]->keyID < str->keyID &&  str->keyID <= finger[1]->keyID){
//Found the actual successor . 
	        char *attr[15] = {"METHOD" , "ID" ,"HOST", "CONTACT" } ;
	        char *val[15] = {"200" , itoa(finger[1]->keyID),nodeToString(finger[0]),nodeToString(finger[1])};
	        utilFramePacket(attr,val,m1);
        	printf("getResponse() : Packet : \n%s\n ",m1);
	        sendPkt(sock,m1);
		return;
        } else {
             for (i=3; i>1; i--) {
                        if (finger[i]->keyID < str->keyID ) {
//Send shortcut contact node to the client.
                    char *attr[15] = {"METHOD" , "ID" ,"HOST", "CONTACT" } ;
                		char *val[15] = {"305" , itoa(finger[i]->keyID),nodeToString(finger[0]),nodeToString(finger[i])};
                		utilFramePacket(attr,val,m1);
		                printf("getResponse() : Packet : \n%s\n ",m1);
                		sendPkt(sock,m1);
				return;
                        }
                }
	}

//Send error msg . Server couldnt find successor or shortcut in its finger table
	char *attr[15] = {"METHOD" , "ID" ,"HOST", "CONTACT" } ;
	char *val[15] = {"500" , itoa(finger[0]->keyID),nodeToString(finger[0]),"0:0"};
        utilFramePacket(attr,val,m1);
        printf("getResponse() : Packet : \n%s\n ",m1);
        sendPkt(sock,m1);
        return;


}


void getrfcResponse (struct msgToken *msgsock){
	int sock;
	struct Msg* str;
	char* rfcToSend;
	char path[50]="/home/hurricane/rfcDB/";
	//char* path="/home/hurricane/rfcDB/";
	str=token(msgsock->ptr);
	sock=msgsock->sock;
	rfcToSend=findRFCfromID(str->keyID);
	strcat(path,rfcToSend);
	printf("Beginning to send rfc: %s\n", path);
	sendRFC(sock, path);

	printf("\nIt is in GETRFC thread now...congo...3...\n");
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
        	char *val[15] = {"100" , itoa(pred->keyID),nodeToString(finger[0]),nodeToString(finger[0])};
        	utilFramePacket(attr,val,m1);
	} else {
	        char *val[15] = {"100" , itoa(pred->keyID),nodeToString(finger[0]),nodeToString(pred)};
        	utilFramePacket(attr,val,m1);
	}
        printf("stabResponse() : Packet : \n%s\n ",m1);
        sendPkt(sock,m1);
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

	if (pred->keyID==-1) {
		pred->keyID=str->keyID;
		strcpy(pred->ipstr,str->contactIP);
		printf("\nThe contact port throwing error is %d\n\n",str->contactPort);
       	       // pred->port=str->contactPort;
	} else {
		if(liesBetween(str->keyID,pred->keyID,finger[0]->keyID)==1) {
				printf("Pred lies between its own pred and self\n");
	                pred->keyID=str->keyID;
	                strcpy(pred->ipstr,str->contactIP);
        	 //       pred->port=str->contactPort;
	
		}
	}
        char *attr[15] = {"METHOD" , "ID" ,"HOST", "CONTACT" } ;

        char *val[15] = {"110" , itoa(finger[0]->keyID),nodeToString(finger[0]),nodeToString(finger[0])};
        utilFramePacket(attr,val,m1);
        printf("notifyResponse() : Packet : \n%s\n ",m1);
        sendPkt(sock,m1);
	printTable();
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

        char *msg1, *join,*get,*getrfc,*stablize,*notify;
        join="JOIN";
        get="GET";
        getrfc="GETRFC";
        stablize="STAB";
	notify = "NOTIFY" ;

        /*DECISION SECTION*/
            		if(strcmp(gotMsg->method,join)==0){
                                printf("A new node is joining......\n");
                                joinResponse(msgsock);
                        }
                        if(strcmp(gotMsg->method,get)==0){
                                printf("GET request received......\n");
                                getResponse(msgsock);
                        }

                        if(strcmp(gotMsg->method,getrfc)==0){
                                printf("Sending RFC......\n");
                                getrfcResponse(msgsock);
                        }

                        if(strcmp(gotMsg->method,stablize)==0){
                                printf("STABILIZING......\n");
                                stabResponse(msgsock);
                        }
                        if(strcmp(gotMsg->method,notify)==0){
                                printf("Got Notify......\n");
                                notifyResponse(msgsock);
                        }
			
			

	return 1;

}


int tcpServer(void)
{
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
	if ((rv = getaddrinfo(NULL,"5000", &hints, &servinfo)) != 0) {

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

	if (listen(sockfd,back) == -1) {
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
		}
		//close(new_fd);  // parent doesn't need this
	}

	return 0;
}

