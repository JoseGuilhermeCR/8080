# Horrible makefile, pardon me! Someday I'll take my time to learn this thing...
tester:
	gcc tester.c cpu/*.c disassembler/*.c -Wall -pedantic-errors -O3 -D WITH_DISASSEMBLER -o tester
