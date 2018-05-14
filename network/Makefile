CC=gcc
CFLAGS=-g -Wall -O2

all:
	$(CC) $(CFLAGS) -o MyNETBench-client MyNETBench-client.c -lpthread
	$(CC) $(CFLAGS) -o MyNETBench-server MyNETBench-server.c -lpthread

clean:
	$(RM) *.o MyNETBench-client MyNETBench-server