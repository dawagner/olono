OPTS = -g

default: olono
all: olono

olono:libCases.o libPlay.o libDisplay.o main.o
	gcc -o olono libCases.o libDisplay.o libPlay.o main.o $(OPTS)

libInit.o: libInit.c
	gcc -o libInit.o -c libInit.c $(OPTS)

libDisplay.o: libDisplay.c
	gcc -o libDisplay.o -c libDisplay.c $(OPTS)

libPlay.o: libPlay.c
	gcc -o libPlay.o -c libPlay.c $(OPTS)

libCases.o: libCases.c
	gcc -o libCases.o -c libCases.c $(OPTS)

main.o: main.c
	gcc -o main.o -c main.c $(OPTS)

doc:
	doxygen ../doxygen/Doxyfile
	$(MAKE) -C ../doxygen/latex -f Makefile

clean:
	rm -v *.o
	rm -v ./olono

mrproper:
	rm -v *.o
	rm -v ./olono
	rm -vR ../doxygen/html ../doxygen/latex

