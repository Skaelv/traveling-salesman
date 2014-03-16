default: all


all: bin/traveling-salesman

run: bin/traveling-salesman
	

bin/voyCom_distribuer.c: src/voyCom_distribuer.c
	gcc -Wall -std=c99 src/voyCom_distribuer.c -o bin/traveling-salesman

clean:
	rm -f bin/*
