cc = gcc
OBJ = ChordLib.o clientChord.o

all: ChordLib.o clientChord.o
	$(CC) -o clientChord ChordLib.o clientChord.o

clientChord.o: ChordLib.h ChordLib.c

ChordLib.o: ChordLib.h ChordLib.c


