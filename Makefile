CC = gcc
CFLAGS = -Wall -Wextra -g -fstack-usage

all: client server

client: client.o
	$(CC) $(CFLAGS) -o $@ $^

server: server.o
	$(CC) $(CFLAGS) -o $@ $^

client.o: client.c
	$(CC) $(CFLAGS) -c -o $@ $^

server.o: server.c
	$(CC) $(CFLAGS) -c -o $@ $^

clean:
	rm -f *.o
