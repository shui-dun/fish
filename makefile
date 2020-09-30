CC = gcc
LIBS = -lpthread -lcurses
SOURCE = $(wildcard *.c)
OBJS = $(patsubst %.c, %.o, ${SOURCE})
CFLAGS = -Wall -std=gnu11
fish: ${OBJS}
	${CC} -o fish ${OBJS} ${LIBS}
clean:
	rm -f fish ${OBJS}