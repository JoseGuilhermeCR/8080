# Horrible makefile, pardon me! Someday I'll take my time to learn this thing...
tester:
	gcc -c cpu/*.c -Wall
	g++ -c tester.cc terminal/*.cc -Wall
	g++ -o tester *.o -lSDL2
dtester:
	gcc -c cpu/*.c disassembler/*.c -Wall -D WITH_DISASSEMBLER
	g++ -c tester.cc terminal/*.cc -Wall -D WITH_DISASSEMBLER
	g++ -o tester *.o -lSDL2
clear:
	rm -f *.o
	rm tester