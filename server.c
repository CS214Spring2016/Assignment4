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
#define PORTNUM	4200; //blaze it

//fun error method #extensible #objectoriented
void error(char* msg)
{
	perror(msg);
	puts("Yea I hear ya buddy");
	exit(0);
}


/*---------------------------------------------------STRING PARSING STUFF----------------------------------------------*/

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
void *printInfo()
{
    do
    {
        sleep(20);
        puts("i'm printing what should be account information\n");
    }
    while(1);

    pthread_exit(NULL);
}


/*-------------------------------------------------------SESSION ACCEPTOR---------------------------------------------------*/

//this is where the client and server communicate
//i dont know what that means exactly
//okay
void *accepted_connection(void *socketdesc)
{
	puts("about to write\n");
	int incomingmessagesize;
	int sock = *(int*)socketdesc;
	char inmessage[256];
	char *message = "Hello friend, you have connected to Barrett & Shafran Community Trust\n";
//	printf("about to write");
	write(sock, message, strlen(message));
	memset(inmessage, 0, 256);
	while((incomingmessagesize = recv(sock, inmessage,256,0))>0){
		puts(inmessage);
	}
//
	puts("Client Disconnected");

	return 0;
}

void *session_acceptor()
{
	int sockfd = -1;
	int portno = -1;
	int cliSocket = -1;

	unsigned int cliLen = -1;
	struct sockaddr_in serverAddressInfo;
	struct sockaddr_in clientAddressInfo;
	
	portno = PORTNUM;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	//problem with ending, socket reuse
	int iSetOption = 1;
	if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &iSetOption, sizeof(iSetOption))<0)
		puts("couldnt change options");
	//end problem

    if (sockfd < 0)
	{
       error("ERROR opening socket");
	}
	bzero((char *) &serverAddressInfo, sizeof(serverAddressInfo));
	serverAddressInfo.sin_port = htons(portno); 
    serverAddressInfo.sin_family = AF_INET;
    serverAddressInfo.sin_addr.s_addr = INADDR_ANY;

    if(bind(sockfd, (struct sockaddr *) &serverAddressInfo, sizeof(serverAddressInfo)) < 0)
	{
		error("ERROR on binding");
	}


	//client-service thread
	pthread_t client_thread;
	pthread_attr_t cliThreadAttr;
	void* clientStatus;
	pthread_attr_init(&cliThreadAttr);
	pthread_attr_setdetachstate(&cliThreadAttr, PTHREAD_CREATE_JOINABLE);
	pthread_attr_setscope(&cliThreadAttr, PTHREAD_SCOPE_SYSTEM);


	listen(sockfd,5);
	puts("Waiting for connections...");
	cliLen = sizeof(clientAddressInfo);

	while((cliSocket = accept(sockfd, (struct sockaddr*)&clientAddressInfo, &cliLen)))
	{
		puts("Announcement: Connection made with client");
		if(pthread_create(&client_thread, &cliThreadAttr, accepted_connection, &cliSocket) < 0)
		{
			puts("unable to create accepted client thread");
		}
	}

	if(cliSocket < 0)
	{
		error("Connection accept failed");
	}

	pthread_join(client_thread, &clientStatus);

	return 0;

}

/*------------------------------------------------------------MAIN METHOD--------------------------------------------------*/


int main(int argc, char *argv[])
{


	// int sockfd = -1;
	// int portno = -1;
	// struct sockaddr_in serverAddressInfo;

	//thread stuff instantiation
	//handles, arguments, attributes, statuses
	//thread status things
	void* acceptorStatus;
	void* infoStatus;
	//actual threads
	pthread_t acceptor_thread;
	pthread_t info_thread;
	//create empty thread attribute struct
	pthread_attr_t acceptorThreadAttr;
	pthread_attr_t infoThreadAttr;
	pthread_attr_init(&acceptorThreadAttr);
	pthread_attr_init(&infoThreadAttr);
	// set the initialized attribute struct so that the pthreads created will be joinable
	pthread_attr_setdetachstate(&acceptorThreadAttr, PTHREAD_CREATE_JOINABLE);
	pthread_attr_setdetachstate(&infoThreadAttr, PTHREAD_CREATE_JOINABLE);
	// set the initialized attribute struct so that the pthreads created will be kernel threads
	pthread_attr_setscope(&acceptorThreadAttr, PTHREAD_SCOPE_SYSTEM);
	pthread_attr_setscope(&infoThreadAttr, PTHREAD_SCOPE_SYSTEM);

	if(pthread_create(&acceptor_thread, &acceptorThreadAttr, session_acceptor, NULL) < 0)
	{
		error("could not create session acceptor thread");
	}

    if(pthread_create(&info_thread, &infoThreadAttr, printInfo, NULL) < 0)
    {
    	error("could not create info print thread");
    }

    pthread_attr_destroy(&acceptorThreadAttr);
	pthread_attr_destroy(&infoThreadAttr);
	
	// wait for the threads to finish .. make the threadStatus variables point to 
	//    the value of pthread_exit() called in each
	pthread_join(acceptor_thread, &acceptorStatus);
	pthread_join(info_thread, &infoStatus);

		
	return 0; 
}
