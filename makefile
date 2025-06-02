main: main.c
	clear
	cc main.c -o bin/main 

out.asm: main
	bin/main test.ln > bin/out.asm

run: out.asm
	nasm -felf64 bin/out.asm -o bin/out.o
	ld bin/out.o -o bin/out
	bin/out

print: main
	bin/main test.ln

debug:
	clear
	cc main.c -o bin/main -DDEBUG -g 
	bin/main test.ln

test:
	clear
	nasm -felf64 fib.asm -o bin/fib.o
	ld bin/fib.o -o bin/fib
	bin/fib

gdb:
	clear
	cc main.c -g -O0 -o bin/main -DDEBUG
	gdb bin/main
