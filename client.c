#include "client.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <ctype.h>
#define PORTNUM		4200; //blaze it part 

//dope error stuff
void error(char* msg)
{
	perror(msg);
	exit(0);
}

void parseCommand(char* str)
{
	printf("parse command here\n");
}

void *serverCommunicate(void *socketdesc)
{	
	int socket = *(int*)socketdesc;
	char *ping = "ping";
	int sentinel;

	sentinel = write(socket, ping, strlen(ping));
	printf("sentinel: %d",sentinel);

	while(sentinel != -1)
	{
		sentinel = write(socket, ping, strlen(ping));
		//printf("this should be happening a lot");
	}

	//shouldnt get here unless write fails
	return (void*)99;
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

void *handleInput()
{
	//char *userString;
	char buffer[256];
	printf("Lemme get an input\n");
	bzero(buffer, 256);
	fgets(buffer, 255, stdin);
	toLowercase(buffer);
	printf("lowercased: %s\n", buffer);
	return 0;
}



int main(int argc, char *argv[])
{
	// Declare initial vars
    int sockfd = -1;							// file descriptor for our socket
	int portno = -1;							// server port to connect to
	//int n = -1;									// utility variable - for monitoring reading/writing from/to the socket
	//char byte[4];							// char array to store data going to and coming from the server
    struct sockaddr_in serverAddressInfo;		// Super-special secret C struct that holds address info for building our socket
    struct hostent *serverIPAddress;
    //long t1, t2;
    pthread_t server_thread;
    pthread_t user_thread;
    
	
	
	// If the user didn't enter enough arguments, complain and exit
    if (argc < 2)
	{
       fprintf(stderr,"usage %s hostname", argv[0]);
       exit(0);
    }
	
	
	
	/** If the user gave enough arguments, try to use them to get a port number and address **/

	// convert the text representation of the port number given by the user to an int
	portno = PORTNUM;
	
	// look up the IP address that matches up with the name given - the name given might
	//    BE an IP address, which is fine, and store it in the 'serverIPAddress' struct
    serverIPAddress = gethostbyname(argv[1]);
    if (serverIPAddress == NULL)
	{
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
				
	// try to build a socket .. if it doesn't work, complain and exit
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
	{
        error("ERROR creating socket");
	}


	
	/** We now have the IP address and port to connect to on the server, we have to get    **/
	/**   that information into C's special address struct for connecting sockets                     **/

	// zero out the socket address info struct .. always initialize!
    bzero((char *) &serverAddressInfo, sizeof(serverAddressInfo));

	// set a flag to indicate the type of network address we'll be using 
    serverAddressInfo.sin_family = AF_INET;
	
	// set the remote port .. translate from a 'normal' int to a super-special 'network-port-int'
	serverAddressInfo.sin_port = htons(portno);

	// do a raw copy of the bytes that represent the server's IP address in 
	//   the 'serverIPAddress' struct into our serverIPAddressInfo struct
    bcopy((char *)serverIPAddress->h_addr, (char *)&serverAddressInfo.sin_addr.s_addr, serverIPAddress->h_length);



	/** We now have a blank socket and a fully parameterized address info struct .. time to connect **/
	
	// try to connect to the server using our blank socket and the address info struct 
	//   if it doesn't work, complain and exit
	int connectStatus;
	connectStatus = connect(sockfd,(struct sockaddr *)&serverAddressInfo,sizeof(serverAddressInfo));
    
    while(connectStatus != 0) 
	{
        close(sockfd);
        printf("Connection failed, attempting to reconnect in 3 seconds...\n");
        sleep(3);
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        connectStatus = connect(sockfd,(struct sockaddr *)&serverAddressInfo,sizeof(serverAddressInfo));
	}	
	
	printf("Connected to server %s, on port number: %d\n", argv[1], portno);

	if(pthread_create(&server_thread, NULL, serverCommunicate, (void*)&sockfd) < 0)
	{
		error("could not create thread");
	}

	puts("server connection thread started");

	//thread to communicate with user
	if(pthread_create(&user_thread, NULL, handleInput, NULL) < 0)
	{
		error("could not create thread");
	}
	puts("user thread created");
	
	
	/** If we're here, we're connected to the server .. w00t!  **/


	//we're connected in this loop, things the client does go in here
	//idk when it ends honestly i just sigint it

	while(pthread_join(server_thread, (void*)serverCommunicate) == 0)
	{
		printf("test");

	}
	printf("Connection closed by server");



    return 0;
}