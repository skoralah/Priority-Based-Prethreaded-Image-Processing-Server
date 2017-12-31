CC = gcc
CFLAGS = -O2 -Wall -I .

# This flag includes the Pthreads library on a Linux box.
# Others systems will probably require something different.
LIB = -lpthread

all: server

server: server.cpp csapp.o
	g++ -std=c++11 server.cpp threadfunc.cpp echo.cpp processimage.cpp `pkg-config --libs --cflags opencv` csapp.o -o server -lpthread

csapp.o: csapp.c
	$(CC) $(CFLAGS) -c csapp.c

clean:
	rm server
	
