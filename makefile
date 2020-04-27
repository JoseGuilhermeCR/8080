tester:
	gcc tester.c cpu/*.c disassembler/*.c -Wall -pedantic-errors -O3 -D WITH_DISASSEMBLER -o tester
