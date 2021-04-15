CC=gcc
LD=gcc
CFLAGS=-g -Wall -pedantic -std=c99
LIBS=-lm
# gtk+-2.0 pour GTK2 (choisi ici)
# gtk+-3.0 pour GTK3
GTKCFLAGS:=-g $(shell pkg-config --cflags gtk+-2.0)
GTKLIBS:=$(shell pkg-config --libs gtk+-2.0)

all: main cleanO

main: main.o points.o particules.o forces.o obstacles.o arbre.o
	$(LD) main.o points.o particules.o forces.o obstacles.o arbre.o $(GTKLIBS) $(LIBS) -o main

main.o: main.c
	$(CC) -c $(CFLAGS) $(GTKCFLAGS) main.c -o main.o

points.o: points.c points.h 
	$(CC) -c $(CFLAGS) $(GTKCFLAGS) points.c -o points.o

particules.o: particules.c particules.h 
	$(CC) -c $(CFLAGS) $(GTKCFLAGS) particules.c -o particules.o

forces.o: forces.c forces.h 
	$(CC) -c $(CFLAGS) $(GTKCFLAGS) forces.c -o forces.o

obstacles.o: obstacles.c obstacles.h
	$(CC) -c $(CFLAGS) $(GTKCFLAGS) obstacles.c -o obstacles.o

arbre.o: arbre.c arbre.h
	$(CC) -c $(CFLAGS) $(GTKCFLAGS) arbre.c -o arbre.o

cleanO:
	rm -f *.o

clean:
	rm -f main main.o particules.o forces.o arbre.o points.o

fullclean: clean
	rm -f *~ *.fig.bak
