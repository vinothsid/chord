
all:
	gcc ChordLib.h
	gcc Util.h
	gcc Util.c -c 
	gcc ChordLib.c -c
	gcc clientChord.c -o clientChord ChordLib.o Util.o 

	gcc serverLib.h
	gcc serverLib.c -c 
	gcc serverChord.c -o serverChord  serverLib.o Util.o ChordLib.o

clean:
	rm *.o serverChord clientChord
