CFLAGS=-g -pedantic -std=c89 -Wall
CC=cc

all: cite

clean: 
	rm -f cite

install:
	$(CC) $(CFLAGS) cite.c -o cite
	chmod 755 cite
	cp cite /usr/local/bin
