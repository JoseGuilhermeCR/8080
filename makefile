make:
	gcc disassembler/*.c -O3 -Wall -pedantic-errors -o bin/disassembler
	gcc i8080.c cpu/*.c disassembler/*.c -Wall -pedantic-errors -O3 -D WITH_DISASSEMBLER -o bin/i8080
	cd assembler && cargo build --release && mv target/release/assembler ../bin
