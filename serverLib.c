#include "serverLib.h"
#include <stdlib.h>
#define port "6000"
#define back 10
#define BLEN 300


void sigchld_handler(int s)
{
	while(waitpid(-1, NULL, WNOHANG) > 0);
}

/***************RESPONSES WRITTEN HERE**********************************/

void joinResponse (struct msgToken *msgsock){
	int sock;	
	struct Msg* str;
printf("printing here.......%s\n...",msgsock->ptr);
	str=token(msgsock->ptr);
	sock=msgsock->sock;

	printf("\nIt is in JOINRESPONSE thread now...congo...1...\n");

	int num,respSock;
	struct Msg *m;
	char *m1,*m2;

srand(time(NULL));
num = rand()%1024;
str->keyID = num;
//char *attr[15] = {"METHOD" , "ID" ,"HOST", "CONTACT" } ;
//char *val[15] = {method , itoa(keyID),host,"contact"}; 
//printf("after val\n");	
//m1 = (char *)malloc(BLEN*sizeof(char));
printf("\nthe number generated(%d)\n",num);
m1=framePacket("200",str->keyID,NULL,NULL,NULL,&m);
//initFingerTable(); // Initialise temp . but basically it should be done in client when it joins
//char *attr[15] = {"METHOD" , "ID" ,"HOST", "CONTACT" } ;
//char *val[15] = {"200" , itoa(str->keyID),nodeToString(finger[0] ) ,"contact"};
//sendPkt(sock,m1);
pthread_exit(NULL);
}

void getResponse (struct msgToken *msgsock){
int sock;
struct Msg* str;
str=token(msgsock->ptr);
sock=msgsock->sock;

printf("\nIt is in GETRESPONSE thread now...congo...2...\n");
}

void getrfcResponse (struct msgToken *msgsock){
int sock;
struct Msg* str;
str=token(msgsock->ptr);
sock=msgsock->sock;

printf("\nIt is in GETRFC thread now...congo...3...\n");
}

void stabResponse (struct msgToken *msgsock){
int sock;
struct Msg* str;
str=token(msgsock->ptr);
sock=msgsock->sock;

printf("\nIt is in JOIN thread now...congo...4...\n");
}





struct Msg* token(char *str1)
{

struct Msg* rcvMsg;
char str[300];
strcpy(str,str1);
printf("\nThis is the printed passed value :\n%s \n",str);
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
//p=strtok(NULL," /:;");
//rcvMsg->contactPort=atoi(p);
//printf("....%d...\n",rcvMsg->contactPort);
rcvMsg->fileInfo= NULL;
rcvMsg->sblNoMsg=0;
//rcvMsg->fileInfo = str->fileInfo;
//rcvMsg->sblNoMsg = str->sblNoMsg;
printf("\n SUCCESSFULLY TOKENIZED \n");
return rcvMsg;
}




int tcpServer(void)
{struct Msg* gotMsg;
struct msgToken *msgsock;
char *msg1;
char *msg ;
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

//printf("\n\n1st ");
	sockfd = socket(servinfo->ai_family,servinfo->ai_socktype,servinfo->ai_protocol);


		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
				sizeof(int)) == -1) {
			perror("setsockopt");
			exit(1);
		}

		if (bind(sockfd, servinfo->ai_addr, servinfo->ai_addrlen) == -1) {
			close(sockfd);
			perror("server: bind");
		
		}

//printf("2nd");
	if (servinfo == NULL)  {
		fprintf(stderr, "server: failed to bind\n");
		return 2;
	}

	freeaddrinfo(servinfo); // all done with this structure

	if (listen(sockfd,back) == -1) {
		perror("listen");
		exit(1);
	}

//printf("3rd");
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
			//if (send(new_fd, "Hello, world!", 13, 0) == -1)
			//	perror("send");
			msg1=malloc(BLEN);
			
			msg1=recvPkt(new_fd);
			printf("The message msg1 is : %s\n", msg1);
			printf("\nreceived successfully...\n");

			printf("Are u here1\n");

			msgsock =(struct msgToken*)malloc(sizeof(struct msgToken));
			msgsock->ptr=msg1;
			msgsock->sock=new_fd;
			printf("........%s.......\n",msgsock->ptr);

			gotMsg=token(msg1);
			int n,ret;
pthread_t thread1, thread2,thread3,thread4;
	
char *msg1, *join,*get,*getrfc,*stablize;

join="JOIN";
get="GET";
getrfc="GETRFC";
stablize="STAB";

printf("....is it ok?....%s.......\n",msgsock->ptr);
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



printf("Are u here 3\n");	
		close(new_fd);
			exit(0);
		}
		//sleep(10);
		//close(new_fd);  // parent doesn't need this
	}

	return 0;
}


