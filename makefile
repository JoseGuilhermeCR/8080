make:
	gcc i8080.c cpu/*.c disassembler/*.c -Wall -pedantic-errors -O3 -D WITH_DISASSEMBLER -o i8080
