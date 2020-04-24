# Horrible makefile, pardon me! Someday I'll take my time to learn this thing...
tester:
	gcc -c cpu/*.c -Wall -O3
	g++ -c tester.cc terminal/*.cc -Wall -O3
	g++ -o tester *.o -lsfml-graphics -lsfml-window -lsfml-system -O3
dtester:
	gcc -c cpu/*.c disassembler/*.c -Wall -D WITH_DISASSEMBLER
	g++ -c tester.cc terminal/*.cc -Wall -D WITH_DISASSEMBLER
	g++ -o tester *.o -lsfml-graphics -lsfml-window -lsfml-system
terminal_tester:
	g++ terminal/*.cc -Wall -pedantic-errors -O3 -lsfml-graphics -lsfml-window -lsfml-system -D TERMINAL_ONLY -o terminal_tester
clear:
	rm -f *.o tester terminal_tester
