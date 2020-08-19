# TODO: This makefile is compiling even though no changes occur, this should probably be changed...
# Although, it's a lot simpler doing it like this since I won't have to put stuff manually in here.
# THIS IS PLAIN GARBAGE, CHANGE IT AS SOON AS POSSIBLE
CC = gcc
CFLAGS = -g -Wall -Wconversion -Wshadow -pedantic-errors -std=c11

HEADERS := $(wildcard *.h)
HEADERS += $(wildcard ../*.h)

SOURCES := $(wildcard *.c)
SOURCES += $(wildcard ../*.c)
SOURCES := $(filter-out ../main.c, $(SOURCES))

OBJECTS := $(patsubst %.c, %.o, $(SOURCES))

tests : $(OBJECTS)
	$(CC) $(OBJECTS) $(CFLAGS) -o tests

$(OBJECTS) : $(SOURCES) $(HEADERS)
	$(CC) -c $(SOURCES)

.PHONY : clean

clean :
	rm  *.o
	rm tests
