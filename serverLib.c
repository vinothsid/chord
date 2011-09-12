#include "serverLib.h"
#define port "5000"
#define back 10
#define BLEN 300
void sigchld_handler(int s)
{
	while(waitpid(-1, NULL, WNOHANG) > 0);
}

void func(char *str){
char *msg, *msg1,*msg2;
msg = strtok(str,",");

while(msg!=NULL)
{

msg1=msg;
msg=strtok(NULL,",");
msg2=msg;
}
printf("hello");
printf("\nThis is the printed passed value %s \n",msg1);

}


int tcpServer(void)
{
	pthread_t thread1;
	char *msg ="SAB GOL MAL HAIN BHAI SAB GOL MAL HAIN";
char *msg1;
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
//printf("\n%s\n",msg);
while(recv(new_fd,msg1,300,0)>0)
{
printf("receiving.....\n");
}

msg=strcat(msg,",");
//msg=strcat(msg,msg1);
printf("\n%s\n",msg);
printf("\n%s\n",msg);
ret = pthread_create (&thread1,NULL, (void *)func,msg);
pthread_join(thread1,NULL);
printf("Error in getting out of thread ");

			

			close(new_fd);
			exit(0);
		}
		close(new_fd);  // parent doesn't need this
	}

	return 0;
}


