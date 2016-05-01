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

void removeSpaces(char* source)
{
  char* i = source;
  char* j = source;
  while(*j != 0)
  {
    *i = *j++;
    if(*i != ' ')
      i++;
  }
  *i = 0;
}

void checkCommands(char *input)
{
	const char *dict[7];
	char *ptr;
	int pos;
	dict[0] = "open";
	dict[1] = "start";
	dict[2] = "credit";
	dict[3] = "debit";
	dict[4] = "balance";
	dict[5] = "finish";
	dict[6] = "exit";

	for(int i = 0; i < 7; i++)
	{
		if(strstr(input,dict[i]) != NULL)
		{
			ptr = strstr(input,dict[i]);
			pos = ptr - input;
			printf("found keyword: %s at location %d\n", dict[i], pos);
		}
	}
}

void parseCommand(char* str)
{
	printf("parse command here\n");
}

/*--------------------------------------------------------------INPUT AND SERVER COMMUNICATION----------------------------*/

void *getFromServer(void *socketdesc)//this is the reader thread
{	

	int incomingmessagesize;
	int sock = *(int*)socketdesc;
	char inmessage[256];
	//char *message = "Hello friend, you have connected to Barrett & Shafran Community Trust\n";
//	printf("about to write");
	//write(sock, message, strlen(message));
	memset(inmessage, 0, 256);
	while((incomingmessagesize = recv(sock, inmessage,256,0))>0){
		puts(inmessage);
	}
	puts("Server Disconnected");
//
//	printf("client closed i guess");

	return 0;
}



void *sendToServer(void *socketdesc)// this is the writer thread
{

	int outgoingmessagesize = 265;
	int sock = *(int*)socketdesc;
	char outmessage[outgoingmessagesize];
	memset(outmessage, 0, outgoingmessagesize);
	while(fgets(outmessage,outgoingmessagesize,stdin)){
		write(sock, outmessage, outgoingmessagesize);
	}

		return 0;
}



int main(int argc, char *argv[])
{
	// Declare initial vars
    int sockfd = -1;						
	int portno = -1;
    struct sockaddr_in serverAddressInfo;		
    struct hostent *serverIPAddress;
    int connectStatus;
    pthread_t server_thread;
    pthread_t user_thread;
    
	
	
	// If the user didn't enter enough arguments, complain and exit
    if (argc < 2)
	{
       fprintf(stderr,"usage %s hostname", argv[0]);
       exit(0);
    }
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
	



	connectStatus = connect(sockfd,(struct sockaddr *)&serverAddressInfo,sizeof(serverAddressInfo));
    
    while(connectStatus != 0) 
	{
        close(sockfd);
        printf("Connection failed, attempting to reconnect in 3 seconds...\n");
        sleep(3);
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        connectStatus = connect(sockfd,(struct sockaddr *)&serverAddressInfo,sizeof(serverAddressInfo));
	}	
	
	printf("Announcement: Connected to server %s, on port number: %d\n", argv[1], portno);

	if(pthread_create(&server_thread, NULL, getFromServer, (void*)&sockfd) < 0)
	{
		error("could not create thread");
	}

	//thread to communicate with user
	if(pthread_create(&user_thread, NULL, sendToServer, (void*)&sockfd) < 0)
	{
		error("could not create thread");
	}

	//we're connected in this loop, things the client does go in here
	//idk when it ends honestly i just sigint it

	while(pthread_join(server_thread, (void*)getFromServer) == 0)
	{
		printf("test");

	}
	printf("Connection closed by server");



    return 0;
}
