#include "ChordLib.h"
#include<pthread.h>

void *PeerServer(void *newNodeServer ) {

	tcpServer((struct Node *)newNodeServer );
}

void *PeerClient(void *newNode) {
	printf("Peer with server address : %s joining\n",nodeToString((struct Node *)newNode));
	join(((struct Node *)newNode)->ipstr,((struct Node*)newNode)->port);
	stabilize();
}

void *stabilizeThread() {
	while(1) {
		sleep(8);
		stabilize();
                sleep(2);
                fixFingers();

		printTable();
	}
}
int main(int argc, char *argv[])
{
	int i=0;
	char *t;
	struct Node *selfNode;
	pthread_t threadClient,threadServer,threadStabilize;
	selfNode = (struct Node *)malloc(sizeof(struct Node));
	strcpy(selfNode->ipstr,argv[1]);
	selfNode->port = atoi(argv[2]);
	printf("Peer with server address : %s joining\n",nodeToString(selfNode));
	printf ("\n CHORD LIGHT IS NOW LISTENING ");


//	tcpServer();
//	sleep(2);
//	if (!fork()) s{
//		while(1) {
//			sleep(5);
//			printf("Stabilize thread\n");
//			stabilize();
//		}
			
//	} else {
	pthread_create(&threadServer,NULL,PeerServer,(void *)selfNode);

	pthread_create(&threadClient,NULL,PeerClient,(void *)selfNode);
//	}
	pthread_create(&threadStabilize,NULL,stabilizeThread,NULL);

	
	pthread_join(threadServer,NULL);	
}



