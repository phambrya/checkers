# Makefile for termCheckers
#
CC = gcc
CFLAGS = -g -Wall -Wextra
OBJS = checkBoard.o gameDriver.o turnLogic.o networkUtilities.o

all: build

build: $(OBJS)
	$(CC) $(CFLAGS) -o termCheckers $(OBJS)

clean:
	rm -f termCheckers $(OBJS)

run: build
	./termCheckers

# dependency list
$(OBJS) : checkBoard.h globals.h turnLogic.h networkUtilities.h
