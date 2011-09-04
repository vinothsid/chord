#include "ChordLib.h"

int main () {
   struct Node * n1;
   struct Msg* m1;
   char *pkt = framePacket("Req",1234,n1,"payload",&m1);
   printf("PKT: %s Port : %d",pkt,m1->hostPort);
   
}
