HOW TO COMPILE
-To compile the server without the makefile, use the command 'gcc -o mserver server.c -lm'
-To compile the client without the makefile, use the command 'gcc -o client client.c'
-To compile both at once with the makefile, use the command 'make'

HOW TO EXECUTE
-To execute the server after running the makefile, use the command './mserver <port_number>'
-To execute the client after running the makefile, use the command './client <port_number>'

HOW TO TEST
-Always make sure to use the same port number on both the server and the client
-When entering values on the client, always make sure to put parenthesis around the numbers, or else the program will not detect them
	-Examples of valid inputs:
		sin(5) + cos(3) + (-2)
		log(4) + (2+1)
		(8+6)
		(4) + (3*2)
	-Exaples of invalid inputs:
		sin(5) + cos(3) + -2
		log4 + (2+1)
		8+6
		(4) + 3*2
-When entering operations that have parameters, make sure to keep them outside of parenthesis, or the program will detect them as nested parenthesis, and that will be an Invalid Expression
	-Examples of valid inputs:
		sin(5) * cos(3)
		log(2) - cos(14)
	-Examples of invalid inputs:
		(sin(5) * cos(3))
		(log(2)) - cos(14)
