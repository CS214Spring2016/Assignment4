#include "server.h"
#include "account.h"
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


/*---------------------------------------------------STRING PARSING STUFF----------------------------------------------*/
int parseCommand(char *command)
{
	//7 commands: open, start, credit, debit, balance, finish, exit
	//char* firstLetter;
	return 0;
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

/*--------------------------------------------ACCOUNT HANDLING THINGS GO HERE-----------------------------------------------*/
void *printInfo(void *threadid)
{
    long tid;
    tid = (long)threadid;
    do
    {
        sleep(20);
        printf("i'm printing what should be account information\n");
    }
    while(1);

    pthread_exit(NULL);
}


/*-------------------------------------------------------SESSION ACCEPTOR---------------------------------------------------*/
void *session_acceptor(void *socketdesc)
{
	int sock = *(int*)socketdesc;
	int read_size;
	char client_message[2000];

	while((read_size = recv(sock, client_message, 2000,0)) > 0)
	{
		//get message from client here and make it lowercase
		//should tokenize in here and maybe call account methods
		client_message[read_size] = '\0';
		toLowercase(client_message);
		stripNonAlpha(client_message);
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



/*------------------------------------------------------------MAIN METHOD--------------------------------------------------*/

int main(int argc, char *argv[])
{
	int sockfd = -1;
	int newsockfd = -1;	
	int portno = -1;
	unsigned int clilen = -1;
	struct sockaddr_in serverAddressInfo;
	struct sockaddr_in clientAddressInfo;
	long infoThreadNum;

	//TODO: find a place to put the accounts


    if (argc < 2)
	{
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }

	/*
	Get user port input, make into number
	Attempt to open socket
	If socket doesnt open right throw error and exit
	*/ 
	portno = atoi(argv[1]);
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
	{
       error("ERROR opening socket");
	}

	/** We now have the port to build our server socket on .. time to set up the address struct **/
	/*
	Setting up address struct
	zero out socket info, init first
	set remote port with serveraddressinfo
	set flags to indicate type of address using and willing to communicate with
	*/
	bzero((char *) &serverAddressInfo, sizeof(serverAddressInfo));
	serverAddressInfo.sin_port = htons(portno); 
	//address type we use
    serverAddressInfo.sin_family = AF_INET;
    //address type we get 
    serverAddressInfo.sin_addr.s_addr = INADDR_ANY;
     
	 
	 /*bind server socket and listen for connections,
	 also write to console that we're listening for connections
	 */
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
    pthread_t acceptor_thread;
    pthread_t infoPrint_thread;

    pthread_create(&infoPrint_thread, NULL, printInfo, (void*)&infoThreadNum);

    //handle new client connections in threads
    //keep connection open and hand out new threads to every connection
	
	while((newsockfd = accept(sockfd, (struct sockaddr*)&clientAddressInfo, &clilen)))
	{
		puts("connection made");

		if(pthread_create( &acceptor_thread, NULL, session_acceptor, (void*)&newsockfd) < 0)
		{
			error("could not create thread");
		}

		puts("session opened with client");
	}

	if(newsockfd < 0)
	{
		error("accept failed");
	}
	
	
	return 0; 
}