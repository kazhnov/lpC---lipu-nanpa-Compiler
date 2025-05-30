main: main.c
	clear
	cc main.c -o bin/main 

out.asm: main
	bin/main test.ln > bin/out.asm

run: out.asm
	nasm -felf64 bin/out.asm -o bin/out.o
	ld bin/out.o -o bin/out
	bin/out

asm: main
	clear
	bin/main test.ln

debug:
	clear
	cc main.c -o bin/main -DDEBUG
	bin/main test.ln
