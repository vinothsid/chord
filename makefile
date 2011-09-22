
all:
	gcc ChordLib.h
	gcc Util.h
	gcc -g Util.c -c 
	gcc -g ChordLib.c -lpthread -c

	gcc -g -o clientChord clientChord.c -lpthread ChordLib.o Util.o 
	gcc -g -o serverChord serverChord.c -lpthread ChordLib.o Util.o
	gcc -g -o peer Peer.c -lpthread ChordLib.o Util.o 
	gcc -g -o peerZero PeerZero.c -lpthread ChordLib.o Util.o 
clean:
	rm *.o serverChord clientChord
