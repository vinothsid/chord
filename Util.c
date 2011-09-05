#include "Util.h"
extern int debug;

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
        if (debug == 1) {
                printf("nodeToString() :From itoa IP String : %s ipPortStr : %s\n",portStr,ipPortStr);
        }
        strcat(ipPortStr,portStr);
        free(portStr);
        if (debug == 1) {
                printf("nodeToString(): IP String : %s\n",ipPortStr);
        }
        return ipPortStr;
}

