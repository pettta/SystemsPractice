# Tommy Pett
# Makefile for assignment 8 for CS283  
CC=gcc

all: client

client: client.c 
	${CC} -o client client.c -pthread

# utility 
clean:
	rm -f client
	
