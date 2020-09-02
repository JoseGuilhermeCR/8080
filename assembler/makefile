CC = gcc
CFLAGS = -g -Wall -Wconversion -Wshadow -std=c11

objs = main.o file_buffer.o preprocessor.o string_manipulation.o assembler.o vecu8.o

assembler : $(objs)
	$(CC) $(objs) $(CFLAGS) -o assembler

main.o : main.c file_buffer.h preprocessor.h assembler.h error.h
file_buffer.o : file_buffer.c file_buffer.h error.h
preprocessor.o : preprocessor.c preprocessor.h file_buffer.h string_manipulation.h error.h
assembler : file_buffer.h error.h vecu8.h
string_manipulation.o :
vecu8.o :

.PHONY : clean

clean :
	rm *.o
	rm assembler
