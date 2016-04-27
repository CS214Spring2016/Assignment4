#include "server.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

//fun error method #extensible #objectoriented
void error(char* msg)
{
	perror(msg);
	exit(0);
}

int startServer(char* portNumber)
{
	int sockfd = -1; 
	//file descriptor for server socket
	
	int newsockfd = -1;
	//descriptor for client socket
	
	int portno = -1;
	//port to connect to
	
	int clilen = -1;															
	// utility variable - size of clientAddressInfo below
	
	int n = -1;																
	// utility variable - for monitoring reading/writing from/to the socket
	
	char buffer[256];													
	// char array to store data going to and coming from the socket

	//c network library structs
	struct sockaddr_in serverAddressInfo;	
	struct sockaddr_in clientAddressInfo;

	if(portNumber == NULL)
	{
		return 0;
	}

	return 0;
}