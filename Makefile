all: server client
server: 
	gcc ./select.c -o server
client:
	gcc ./client.c -o client