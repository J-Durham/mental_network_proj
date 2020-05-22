start: server.c client.c
	gcc -o mserver server.c -lm
	gcc -o client client.c
