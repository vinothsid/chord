#include "ChordLib.h"

char *itoa(int num); //converts integer to a character string, make sure to hold the return in a place holder and free it after use

char *nodeToString(struct Node *n); //converts ip and port information of a host into a single contiguous string where ip and port are separated by a ":"
