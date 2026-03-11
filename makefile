both: server client
server_only: server
client_only: client

server: server.o room.o user.o
	gcc -o server server.o room.o user.o

client: client.c
	gcc -o client client.c

server.o: server.c user.o room.o
	gcc -c server.c

room.o: room.h room.c
	gcc -c room.c 

user.o: user.h user.c
	gcc -c user.c
