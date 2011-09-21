#include<stdio.h>
#include "serverLib.h"

int main()
{
	int i=0;
	printf("Hello how are you ??\n");
	printf ("\nConnecting to the CHORD LIGHT SYSTEM.........");
	printf ("\n CHORD LIGHT IS NOW LISTENING ");

	initFingerTable("127.0.0.1",5000); //For peer 0 only this initFingerTable and following initialisation is needed. For all other peers it should be done in join
        for (i=0; i<4; i++) {
                strcpy(finger[i]->ipstr,origin->ipstr);
                finger[i]->keyID=origin->keyID;
		finger[i]->port = origin->port;
        }

	printf("Server main(): self address : %s self Port: %d\n",finger[0]->ipstr,finger[0]->port);

//	if (!fork()) {
//		while(1) {
//			sleep(5);
//			printf("Stabilize thread\n");
//			stabilize();
//		}
			
//	} else {
		tcpServer();
//	}
}

