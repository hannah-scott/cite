CFLAGS=-g -Wall
CC=cc

clean: 
	rm -f cite

install:
	$(CC) $(CFLAGS) cite.c -o cite
	chmod 755 cite
	mv cite /usr/local/bin
