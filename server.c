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
#define PORTNUM  4200; //blaze it

//fun error method #extensible #objectoriented
void error(char* msg)
{
	perror(msg);
	exit(0);
}


/*---------------------------------------------------STRING PARSING STUFF----------------------------------------------*/


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

//what gets called when accept happens
//this just wants to keep the connection alive i think
//no need to send anything visible to the client?
//idk man
//gonna try tokenizing client side
void *accepted_connection(void *socketdesc)
{
	int sock = *(int*)socketdesc;
	int read_size;
	char client_message[2000];

	while((read_size = recv(sock, client_message, 2000,0)) > 0)
	{
		//get message from client here and make it lowercase
		//should tokenize in here and maybe call account methods
		client_message[read_size] = '\0';
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

	printf("client closed i guess");

	return 0;
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


 //    if (argc < 2)
	// {
 //        fprintf(stderr,"ERROR, no port provided\n");
 //        exit(1);
 //    }

	/*
	Get user port input, make into number
	Attempt to open socket
	If socket doesnt open right throw error and exit
	*/ 
	portno = PORTNUM;
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
    pthread_t accept_thread;
    pthread_t infoPrint_thread;

    pthread_create(&infoPrint_thread, NULL, printInfo, (void*)&infoThreadNum);

    //handle new client connections in threads
    //keep connection open and hand out new threads to every connection
	
	//apparently this causes a race condition in weird circumstances
	//i think we can fix this later, fuck threads rn
	while((newsockfd = accept(sockfd, (struct sockaddr*)&clientAddressInfo, &clilen)))
	{
		puts("connection made");

		if(pthread_create( &accept_thread, NULL, accepted_connection, (void*)&newsockfd) < 0)
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