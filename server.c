#include "server.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <pthread.h>

//fun error method #extensible #objectoriented
void error(char* msg)
{
	perror(msg);
	exit(0);
}

void toLowercase(char *str)
{
	for(int i = 0; i<strlen(str); i++)
	{
		str[i] = tolower(str[i]);
	}
}

void stripNonAlpha(char *str)
{
	for(int i = 0; i < strlen(str); i++)
	{
		if(isalnum(str[i]) == 0)
		{
			str[i] = ' ';
		}
	}
}

void *session_acceptor(void *socketdesc)
{
	int sock = *(int*)socketdesc;
	int read_size;
	char *message, client_message[2000];

	//send test thing on thread
	// message = "hello this is the session acceptor, i'll fix your strings for now";
	// write(sock,message,strlen(message));

	while((read_size = recv(sock, client_message, 2000,0)) > 0)
	{
		//get message from client here and make it lowercase
		client_message[read_size] = '\0';
		toLowercase(client_message);
		write(sock, client_message, strlen(client_message));
		memset(client_message, 0, 2000);
	}

	if(read_size == 0)
	{
		printf("client disconnect\n");
		fflush(stdout);
	}
	else if(read_size == -1)
	{
		error("recv failed");
	}

	return 0;
}

void tokenizeInput(char *str)
{
	const char delims[2] = " ";
	char *token;

	//first token
	token = strtok(str,delims);


	while(token != NULL)
	{
		printf("token: %s",token);
		token = strtok(NULL,delims);
	}
}

int main(int argc, char *argv[])
{
	int sockfd = -1;
	int newsockfd = -1;	
	int portno = -1;
	unsigned int clilen = -1;
	// int n = -1;		
	// char buffer[256];
	struct sockaddr_in serverAddressInfo;
	struct sockaddr_in clientAddressInfo;


    if (argc < 2)
	{
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }

    /** If the user gave enough arguments, try to use them to get a port number and address **/

	// convert the text representation of the port number given by the user to an int  
	portno = atoi(argv[1]);
	 
	// try to build a socket .. if it doesn't work, complain and exit
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
	{
       error("ERROR opening socket");
	}

	/** We now have the port to build our server socket on .. time to set up the address struct **/

	// zero out the socket address info struct .. always initialize!
	bzero((char *) &serverAddressInfo, sizeof(serverAddressInfo));

	// set the remote port .. translate from a 'normal' int to a super-special 'network-port-int'
	serverAddressInfo.sin_port = htons(portno);
	 
	// set a flag to indicate the type of network address we'll be using  
    serverAddressInfo.sin_family = AF_INET;
	
	// set a flag to indicate the type of network address we'll be willing to accept connections from
    serverAddressInfo.sin_addr.s_addr = INADDR_ANY;
     
	 
	 
	 /** We have an address struct and a socket .. time to build up the server socket **/
     
    // bind the server socket to a specific local port, so the client has a target to connect to      
    if (bind(sockfd, (struct sockaddr *) &serverAddressInfo, sizeof(serverAddressInfo)) < 0)
	{
		error("ERROR on binding");
	}
			  
	// set up the server socket to listen for client connections
    listen(sockfd,5);
    puts("waiting for connections...");
	
	// determine the size of a clientAddressInfo struct
    clilen = sizeof(clientAddressInfo);
    pthread_t thread_id;

    //handle new client connections in threads
	
	while((newsockfd = accept(sockfd, (struct sockaddr*)&clientAddressInfo, &clilen)))
	{
		puts("connection made");
		char* message;

		if(pthread_create( &thread_id, NULL, session_acceptor, (void*)&newsockfd) < 0)
		{
			error("could not create thread");
		}

		puts("handler assigned");
	}

	if(newsockfd < 0)
	{
		error("accept failed");
	}
	
	
	return 0; 
}