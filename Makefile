
all:
	gcc -g -Wall -o lcbf lcbf.c

run:
	./lcbf hello.bf
	complx bf.asm

clean:
	rm lcbf
	rm bf.asm
