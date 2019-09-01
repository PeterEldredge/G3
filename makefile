
#### C ####

.PHONY: build view clean

build : build1

build1: con4.c
		gcc con4.c -pthread -o con4.exe

clean:
		-\rm con4.exe