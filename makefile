
all:
	gcc ChordLib.h
	gcc Util.h
	gcc -g Util.c -c 
	gcc -g ChordLib.c -c
	gcc -g clientChord.c -o clientChord ChordLib.o Util.o 

	gcc serverLib.h
	gcc -g serverLib.c -c 
	gcc -g serverChord.c -o serverChord  serverLib.o Util.o ChordLib.o

clean:
	rm *.o serverChord clientChord
