cc = gcc
OBJ = ChordLib.o clientChord.o Util.o

all: $(OBJ)
	$(CC) -o peer.o $(OBJ)

clientChord.o: ChordLib.h ChordLib.c 

ChordLib.o: ChordLib.h ChordLib.c Util.o

Util.o: ChordLib.h Util.h Util.c

clean:
	rm  *.o



