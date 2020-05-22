/*
	Simple udp client
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
 
#define SERVER "129.120.151.94"
#define BUFLEN 512  //Max length of buffer
 
void die(char *s)
{
	perror(s);
	exit(1);
}
 
int main(int argc, char **argv)
{
	//check for correct # of arguments
	if(argc != 2)
	{
		printf("Error in number of arguments. Correct usage is ./client <port_number>\n");
		return 0;
	}
	int PORT = atoi(argv[1]);
	struct sockaddr_in si_other;
	int s, i, slen=sizeof(si_other);
	char buf[BUFLEN];
	char message[BUFLEN];
 
	if ( (s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
	{
		die("socket");
	}
 
	memset((char *) &si_other, 0, sizeof(si_other));
	si_other.sin_family = AF_INET;
	si_other.sin_port = htons(PORT);
	 
	if (inet_aton(SERVER , &si_other.sin_addr) == 0) 
	{
		fprintf(stderr, "inet_aton() failed\n");
		exit(1);
	}
 
	while(1)
	{
		printf("exprsn: ");
		gets(message);
		 
		//send the message
		if (sendto(s, message, strlen(message) , 0 , (struct sockaddr *) &si_other, slen)==-1)
		{
			die("sendto()");
		}

		if(strcmp(message, "quit") == 0)
		{
			printf("Shutting down...\n");
			break;
		}
		//receive a reply and print it
		//clear the buffer by filling null, it might have previously received data
		memset(buf,'\0', BUFLEN);
		//try to receive some data, this is a blocking call
		if (recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen) == -1)
		{
			die("recvfrom()");
		}
		if(buf[0] == 'I')
		{
			strcpy(buf, "Invalid Expression");
		}
		printf("result: %s\n", buf);
	}

	close(s);
	return 0;
}
