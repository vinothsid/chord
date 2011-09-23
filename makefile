
all:
	gcc ChordLib.h
	gcc Util.h
	gcc -g Util.c -c 
	gcc -g ChordLib.c -lpthread -c

	gcc -g -o peer Peer.c -lpthread ChordLib.o 
	gcc -g -o peerZero PeerZero.c -lpthread ChordLib.o 
clean:
	rm *.o serverChord clientChord
