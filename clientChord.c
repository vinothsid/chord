#include "ChordLib.h"
#include <stdlib.h>


int main () {
   struct Node * n1;
   struct Msg* m1;
   //char *pkt = framePacket("Req",1234,n1,"payload",&m1);
   //printf("PKT: %s Port : %d",pkt,m1->hostPort);
   //printf("Vinoth");
//   getKey(1011);
   join("127.0.0.1",4000);
   printf ("The corresponding rfc is : %s\n",findRFCfromID(469));
   //getRFCrequest(604,finger[0]);
   triggerSingleRFC(668);
	//stabilize();
   //printTable();
}
