/*
	Simple udp server
*/
//compile with -lm at the end so it compiles with the math library
#include <stdio.h> 
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <math.h>
#define BUFLEN 512  //Max length of buffer
//#define PORT 6702   //The port on which to listen for incoming data
 

void die(char *s)
{
	perror(s);
	exit(1);
}
 
int main(int argc, char **argv)
{
	if(argc != 2)
	{
		printf("Error in number of arguments. Correct usage is ./mserver <port_number>\n");
		return 0;
	}
	int PORT = atoi(argv[1]);
	struct sockaddr_in si_me, si_other;

	int s, i, slen = sizeof(si_other) , recv_len;
	char buf[BUFLEN];

	char opChar[4];
	int opInt[8];
	float opFloat[5];
	int cCount;
	int iCount;
	int fCount;
	char intHolder[10];
	char op;

	//create a UDP socket
	if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
	{
		die("socket");
	}

	// zero out the structure
	memset((char *) &si_me, 0, sizeof(si_me));

	si_me.sin_family = AF_INET;
	si_me.sin_port = htons(PORT);
	si_me.sin_addr.s_addr = htonl(INADDR_ANY);

	//bind socket to port
	if( bind(s , (struct sockaddr*)&si_me, sizeof(si_me) ) == -1)
	{
		die("bind");
	}
	 
	//keep listening for data
	while(1)
	{
		int l = 0;
		for(l = 0; l < strlen(buf); l++)
		{
			buf[l] = ' ';
		}
//		printf("Waiting for data...");
		fflush(stdout);
		 
		//try to receive some data, this is a blocking call
		if ((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen)) == -1)
		{
			die("recvfrom()");
		}
		 
		//print details of the client/peer and the data received
		/*
		printf("Received packet from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
		printf("Data: %s\n" , buf);
		*/

		if(strcmp(buf, "quit") == 0)
		{
			exit(1);
		}

		//DO THE WORK/MATH

		//step 1: remove all spaces so all messages are uniform
		char* k = buf;
        	char* n = buf;
        	while(*n != 0)
        	{
                	*k = *n;
                	*n++;
                	if(*k != ' ')
                	{
                        	k++;
               		}
        	}
        	*k = 0;
//		printf("%s\n", buf);

		//step 2: check for nested parenthesis to see if expression is invalid
		l = 0;
		int bad = 0;
		int invalid = 0;
		for(l = 0; l < strlen(buf); l++)
		{
			if(buf[l] == '(')
			{
				if(bad == 1)
				{
					invalid = 1;
				}
				else
				{
					bad = 1;
				}
			}
			if(buf[l] == ')')
			{
				bad = 0;
			}
		}

		//step 3: check for unfinished expression to see if expression is invalid
		l = 0;
		bad = 0;
		for(l = 0; l < strlen(buf); l++)
		{
			//3a: check for parenthesis before or after empty expression
			if((buf[l] == '+') || (buf[l] == '*') || (buf[l] == '/') || (buf[l] == '^'))
			{
				if(buf[l+1] == ')')
				{
					invalid = 1;
				}
				if(buf[l+1] == '\0')
				{
					invalid = 1;
				}
				if(buf[l-1] == '(')
				{
					invalid = 1;
				}
			}
			if(buf[l] == '-')
			{
				if(buf[l+1] == ')')
				{
					invalid = 1;
				}
				if(buf[l+1] == '\0')
				{
					invalid = 1;
				}
			}

			//3b: check for empty parenthesis in a function
			if(buf[l] == '(')
			{
				if(buf[l+1] == ')')
				{
					//if(buf[l-1] == ('n' || 's' || 't' || 'g'))
					if((buf[l-1] == 'n') || (buf[l-1] == 's') || (buf[l-1] == 't') || (buf[l-1] == 'g'))
					{
//						printf("bad3\n");
						invalid = 1;
					}
				}
			}
		}

		//step 4: return invalid, or do processing
		if(invalid == 1)
		{
			//4a: return invalid
			strcpy(buf, "Invalid Expression");
		}
		else
		{
			//4b: process the math
			//needed things:
				//4 characters to hold possible operation
					//code will be + for add, - for sub, * for mult, / for divide, & for sqrt, ^ for power, e for e, s for sin, c for cos, l for log
					//can use a 4 character array to do it.
					//array will be char opChar[4]
				//16 integers to hold possible operands
					//use array to hold them
					//array will be int opInt[16]
				//4 floats to hold temporary answers
				//1 float for the final answer
					//use a 5 indexed array to hold floats
					//array will be float opFloat[5]
					//first 4 indexes will be temp values, last will be final answer
				//counters for all arrays
					//cCount
					//iCount
					//fCount
			//how to process:
				//first, loop around the whole string to group together operands
					//if they are in the same parens, make them adjacent
					//if not, skip the first spot in the pair
						//ex (2+5) - log(5)
							//int array will be (2,5, ,5)
				//next, loop around whole string to group together operations
					//ex (2+5) - log(5)
						//op array will be (+,-,l)
				//next, do individual operations and store in float array
					//will need to skip every other operation or shit fails
					//ex (2+5) - log(5)
						//float array will be (7,.6990, , , )
				//next, do every other operation, committing them all to the final value in the array
					//ex (2+5) - log(5)
						//float array will be (7,.6990, , ,6.301)
				//convert final float to char, and store it in buf
			cCount = 0;
			iCount = 0;
			fCount = 0;
			int hCount = 0;
			//reset all the arrays
			memset(&opChar[0], 'p', sizeof(opChar));
			memset(&opInt[0], -4111212, sizeof(opInt));
			memset(&opFloat[0], 0, sizeof(opFloat));
			memset(&intHolder[0], 0, sizeof(intHolder));
			//loop for operands
			l = 0;
			int in = 0;
			for(l = 0; l < strlen(buf); l++)
			{
				//get the values for within the parens here
				//keep adding to the intHolder until we see an operation sign or end of parenthesis
				if(in == 1)
				{
					if((buf[l] == '+') || (buf[l] == '*') || (buf[l] == '/') || (buf[l] == '^') || (buf[l] == ')'))
					{
						//put holder into array
						sscanf(intHolder, "%d", &opInt[iCount]);
						hCount = 0;
						memset(&intHolder, 0, sizeof(intHolder));
						iCount++;
					}
					else if(buf[l] == '-')
					{
						if(buf[l-1] == '^')
						{
//							printf("adding %c to holder\n", buf[l]);
                                                	intHolder[hCount] = buf[l];
                                                	hCount++;
						}
						else
						{
//							sscanf(intHolder, "%d", &opInt[iCount]);
							sscanf(intHolder, "%d", &opInt[iCount]);
                                                	hCount = 0;
                                                	memset(&intHolder, 0, sizeof(intHolder));
                                                	iCount++;
						}
					}
					else if(buf[l] == 'e')
					{
						in = 1;
					}
					else
					{
//						printf("adding %c to holder\n", buf[l]);
						intHolder[hCount] = buf[l];
						hCount++;
						if(buf[l+1] == ')')
						{
							in = 0;
							sscanf(intHolder, "%d", &opInt[iCount]);
                                                	hCount = 0;
                                                	memset(&intHolder, 0, sizeof(intHolder));
                                                	iCount++;
						}
					}
				}

				if(buf[l] == '(')
                                {
//                                      printf("in\n");
                                        in = 1;
                                        if(buf[l+1] == '-')
                                        {
                                                intHolder[hCount] = '-';
                                                hCount++;
                                                l++;
                                        }
                                }

				//incrementing the single var functions so the integers are alone in the array
				if((buf[l] == 's') || (buf[l] == 'l') || (buf[l] == 'e'))
				{
					iCount++;
				}

				//start to pull the data for the operands
				if((buf[l] == '+') || (buf[l] == '*') || (buf[l] == '/') || (buf[l] == '^'))
				{
					opChar[cCount] = buf[l];
					cCount++;
				}
				if(buf[l] == '-')
				{
					if((buf[l-1] == '(') || (buf[l-1] == '^'))
					{
						cCount = cCount;
					}
					else
					{
						opChar[cCount] = buf[l];
						cCount++;
					}
				}
				if(buf[l] == 'e')
				{
					opChar[cCount] = buf[l];
					cCount++;
					l++;
				}
				if(buf[l] == 'l')
				{
					opChar[cCount] = buf[l];
					cCount++;
				}
				if(buf[l] == 's')
				{
					if(buf[l+1] == '(')
					{
						opChar[cCount] = 'c';
						cCount++;
					}
					if(buf[l+1] == 'q')
					{
						opChar[cCount] = '&';
						cCount++;
					}
					if(buf[l+1] == 'i')
					{
						opChar[cCount] = 's';
						cCount++;
					}
				}

			}
//			printf("opInt: \n");
			for(l = 0; l < 8; l++)
			{
//				printf("%i\n", opInt[l]);
			}
//			printf("opChar: \n");
			for(l = 0; l < 4; l++)
			{
//				printf("%c\n", opChar[l]);
			}

			//now to do the initial, shitty math
			//work on operations 1 and 3, on the pairs in (0,1) and (2,3)
			//part 1: get the operation for the first pair
				//note: will need to check if the second pair exists
			op = opChar[0];
			if(op == '+')
			{
				opFloat[0] = opInt[0]+opInt[1];
			}
			if(op == '-')
			{
//				printf("sub\n");
//				printf("%d %d\n", opInt[0], opInt[1]);
				opFloat[0] = opInt[0]-opInt[1];
			}
			if(op == '*')
			{
				opFloat[0] = opInt[0]*opInt[1];
			}
			if(op == '/')
			{
				opFloat[0] = (float)opInt[0]/opInt[1];
			}
			if(op == '^')
			{
				opFloat[0] = (float)pow((double)opInt[0], (double)opInt[1]);
			}
			if(op == '&')
			{
				opFloat[0] = (float)sqrt((double)opInt[1]);
			}
			if(op == 'e')
			{
				opFloat[0] = (float)exp((double)opInt[1]);
			}
			if(op == 's')
			{
				opFloat[0] = (float)sin((double)opInt[1]);
			}
			if(op == 'c')
			{
				opFloat[0] = (float)cos((double)opInt[1]);
			}
			if(op == 'l')
			{
				opFloat[0] = (float)log10((double)opInt[1]);
			}
//			printf("float 1 = %.4f\n", opFloat[0]);
			//work on the second pair
			if(opInt[3] != -1802201964)
			{
//				printf("in if\n");
				op = opChar[2];
	                        if(op == '+')
	                        {
	                                opFloat[1] = opInt[2]+opInt[3];
	                        }
	                        if(op == '-')
	                        {
	                                opFloat[1] = opInt[2]-opInt[3];
	                        }
	                        if(op == '*')
	                        {
	                                opFloat[1] = opInt[2]*opInt[3];
	                        }
	                        if(op == '/')
	                        {
	                                opFloat[1] = (float)opInt[2]/opInt[3];
	                        }
	                        if(op == '^')
	                        {
	                                opFloat[1] = (float)pow((double)opInt[2], (double)opInt[3]);
	                        }
	                        if(op == '&')
	                        {
	                                opFloat[1] = (float)sqrt((double)opInt[3]);
	                        }
	                        if(op == 'e')
	                        {
	                                opFloat[1] = (float)exp((double)opInt[3]);
	                        }
	                        if(op == 's')
	                        {
	                                opFloat[1] = (float)sin((double)opInt[3]);
	                        }
        	                if(op == 'c')
	                        {
	                                opFloat[1] = (float)cos((double)opInt[3]);
	                        }
	                        if(op == 'l')
	                        {
	                                opFloat[1] = (float)log10((double)opInt[3]);
	                        }
//				printf("float 2 = %.4f\n", opFloat[1]);
			}
			else if(opInt[2] != -1802201964)
			{
				opFloat[1] = (float)opInt[2];
//				printf("float 2 = %.4f\n", opFloat[1]);
			}

			//now do grouped operations
			op = opChar[1];
			if(op != 'p')
			{
				if(op == '+')
				{
					opFloat[4] = opFloat[0] + opFloat[1];
				}
				if(op == '-')
				{
					opFloat[4] = opFloat[0] - opFloat[1];
				}
				if(op == '*')
				{
					opFloat[4] = opFloat[0] * opFloat[1];
				}
				if(op == '/')
				{
					opFloat[4] = opFloat[0] / opFloat[1];
				}
				if(op == '^')
				{
					opFloat[4] = (float)pow((double)opFloat[0], (double)opFloat[1]);
				}
			}
			else
			{
				opFloat[4] = opFloat[0];
			}
			op = opChar[3];
			if(op != 'p')
			{
				if(op == '+')
                                {
                                        opFloat[4] = opFloat[4] + (float)opInt[4];
                                }
                                if(op == '-')
                                {
                                        opFloat[4] = opFloat[4] - (float)opInt[4];
                                }
                                if(op == '*')
                                {
                                        opFloat[4] = opFloat[4] * (float)opInt[4];
                                }
                                if(op == '/')
                                {
                                        opFloat[4] = opFloat[4] / (float)opInt[4];
                                }
                                if(op == '^')
                                {
                                        opFloat[4] = (float)pow((double)opFloat[0], (double)opInt[4]);
                                }
			}
//			printf("final answer = %.4f\n", opFloat[4]);
			sprintf(buf, "%.4f", opFloat[4]);
			if(strcmp(buf, "nan") == 0)
			{
				strcpy(buf, "Invalid Expression");
			}

		}

		//now reply the client with the same data
		if (sendto(s, buf, recv_len, 0, (struct sockaddr*) &si_other, slen) == -1)
		{
			die("sendto()");
		}
	}
 
	close(s);
	return 0;
}

