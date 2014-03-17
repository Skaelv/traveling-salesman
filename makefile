default: all


all: bin/traveling-salesman
	
run: bin/traveling-salesman
	./bin/traveling-salesman
	
	

bin/traveling-salesman: src/voyCom_a_distribuer.c
	gcc -Wall src/voyCom_a_distribuer.c -o bin/traveling-salesman -lm -lrt

clean:
	rm -f bin/*
