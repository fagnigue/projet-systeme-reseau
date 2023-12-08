.SILENT: all
all: server client

dependencies.o: sources/dependencies.c
	gcc -c sources/dependencies.c -o objects/dependencies.o

bd.o: sources/bd.c
	gcc -c sources/bd.c -o objects/bd.o

socket.o: sources/socket.c
	gcc -c sources/socket.c -o objects/socket.o

main.o: sources/main.c
	gcc -c sources/main.c -o objects/main.o

client.o: sources/client.c
	gcc -c sources/client.c -o objects/client.o

server: dependencies.o bd.o socket.o main.o
	gcc -Wall objects/dependencies.o objects/bd.o objects/socket.o objects/main.o -o bin/server

client: client.o
	gcc -Wall objects/client.o -o bin/client

clean:
	rm objects/*.o
	rm bin/*