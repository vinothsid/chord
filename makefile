
all:
	gcc ChordLib.h
	gcc Util.h
	gcc -g Util.c -c 
	gcc -g ChordLib.c -c
	gcc -g clientChord.c -o clientChord ChordLib.o Util.o 

	gcc -g -o peer Peer.c -lpthread ChordLib.o Util.o 
clean:
	rm *.o serverChord clientChord
