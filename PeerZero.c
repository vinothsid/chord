#include "ChordLib.h"
#include<pthread.h>
void *PeerServer(void *newNodeServer ) {

        tcpServer((struct Node *)newNodeServer );
}

void *PeerClient(void *newNode) {
	printf("Peer with server address : %s joining\n",nodeToString((struct Node *)newNode));
	join(((struct Node *)newNode)->ipstr,((struct Node*)newNode)->port);
	stabilize();
	printTable();

}

void *stabilizeThread() {
        while(1) {
	if( totalPeers>0 && finger[1]->keyID!=0   ) {
        	sleep(2);
                stabilize();
		fixFingers();
                printTable();
	}
        }
}

int main()
{
	int i=0;
	char *t;
	pthread_t threadClient,threadServer,threadStabilize ;
	printf ("\n CHORD LIGHT IS NOW LISTENING ");

	totalPeers=0;
	initFingerTable( PEER_ZERO_IP, PEER_ZERO_PORT); //For peer 0 only this initFingerTable and following initialisation is needed. For all other peers it should be done in join
        for (i=0; i<4; i++) {
                strcpy(finger[i]->ipstr,origin->ipstr);
                finger[i]->keyID=origin->keyID;
		finger[i]->port = origin->port;
        }

	printf("Server main(): self address : %s self Port: %d\n",finger[0]->ipstr,finger[0]->port);
//	tcpServer();
//	sleep(2);
//	if (!fork()) s{
//		while(1) {
//			sleep(5);
//			printf("Stabilize thread\n");
//			stabilize();
//		}
			
//	} else {
	pthread_create(&threadServer,NULL,PeerServer,(void *)finger[0]);

	pthread_create(&threadStabilize,NULL,stabilizeThread,NULL);

	pthread_join(threadServer,NULL);
	
	pthread_join(threadStabilize,NULL);

//	pthread_create(&threadClient,NULL,PeerClient,(void *)finger[0]);
//	}

	
	
}



