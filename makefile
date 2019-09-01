#### C ####

.PHONY: build view clean

build : build1

build1: con4.c
	gcc con4.c -pthread -o con4.exe

run: con4.exe
	./con4.exe "${GAMES}" "${DIMENSION}"

test: con4.exe
	GAMES=4 DIMENSION=8 make run

clean:
	-\rm con4.exe