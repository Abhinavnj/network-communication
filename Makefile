CFLAGS=-g -std=c99 -Wvla -Wall -fsanitize=address,undefined

server: server.o linkedlist.o
	gcc $(CFLAGS) -o server server.o linkedlist.o -lpthread

server.o: server.c linkedlist.h
	gcc -c $(CFLAGS) server.c

linkedlist.o: linkedlist.c linkedlist.h
	gcc -c $(CFLAGS) linkedlist.c

clean:
	rm -f *.o server linkedlist