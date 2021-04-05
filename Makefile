CC=gcc
LD=gcc
CFLAGS=-g -Wall -pedantic -std=c99
LIBS=-lm
# gtk+-2.0 pour GTK2 (choisi ici)
# gtk+-3.0 pour GTK3
GTKCFLAGS:=-g $(shell pkg-config --cflags gtk+-2.0)
GTKLIBS:=$(shell pkg-config --libs gtk+-2.0)

all: convex cleanO

convex: convex.o points.o pile.o
	$(LD) convex.o points.o pile.o $(GTKLIBS) $(LIBS) -o convex

convex.o: convex.c
	$(CC) -c $(CFLAGS) $(GTKCFLAGS) convex.c -o convex.o

points.o: points.c points.h 
	$(CC) -c $(CFLAGS) $(GTKCFLAGS) points.c -o points.o

pile.o: pile.c pile.h
	$(CC) -c $(CFLAGS) $(GTKCFLAGS) pile.c -o pile.o

clean:
	rm -f convex convex.o points.o

fullclean: clean
	rm -f *~ *.fig.bak

cleanO:
	rm -f *.o
