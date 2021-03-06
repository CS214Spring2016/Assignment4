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
#define PORTNUM		4200; //blaze it part 2

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

void fixString(char *source)
{
	toLowercase(source);
	stripNonAlpha(source);
	removeSpaces(source);
}




/*--------------------------------------------------------------INPUT AND SERVER COMMUNICATION----------------------------*/

void *getFromServer(void *socketdesc)//this is the reader thread
{	
	int incomingmessagesize;
	int sock = *(int*)socketdesc;
	char inmessage[256];
	//memset(inmessage, 0, 256);
	while(1)
	{
		if((incomingmessagesize = recv(sock, inmessage,256, 0))>0)
		{
			puts(inmessage);
		}
		else
		{
			pthread_exit(NULL);
		}

	}
	puts("Server Disconnected");

	return 0;
}



void *sendToServer(void *socketdesc)// this is the writer thread
{

	int sock = *(int*)socketdesc;
	char outmessage[256];
	memset(outmessage, 0, 256);
	while(1)
	{
		if(fgets(outmessage,255,stdin) != 0)
		{
			fixString(outmessage);
			write(sock,outmessage,256);
		}
		sleep(2);
		//fflush(stdin);
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
	//actual threads
	//create empty thread attribute struct
	pthread_attr_t serverThreadAttr;
	pthread_attr_t clientThreadAttr;
	pthread_attr_init(&serverThreadAttr);
	pthread_attr_init(&clientThreadAttr);
	// set the initialized attribute struct so that the pthreads created will be joinable
	pthread_attr_setdetachstate(&serverThreadAttr, PTHREAD_CREATE_JOINABLE);
	pthread_attr_setdetachstate(&clientThreadAttr, PTHREAD_CREATE_JOINABLE);
	// set the initialized attribute struct so that the pthreads created will be kernel threads
	pthread_attr_setscope(&serverThreadAttr, PTHREAD_SCOPE_SYSTEM);
	pthread_attr_setscope(&clientThreadAttr, PTHREAD_SCOPE_SYSTEM);
    
	
	
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
		error("could not create server thread");
	}

	//thread to communicate with user
	if(pthread_create(&user_thread, NULL, sendToServer, (void*)&sockfd) < 0)
	{
		error("could not create user thread");
	}

	//we're connected in this loop, things the client does go in here
	//this keeps everything alive and holy shit thats scary
	//idk when it ends honestly i just sigint it

	while(pthread_join(server_thread, (void*)getFromServer) == 0)
	{
		printf("test");

	}
	printf("Connection closed by server");



    return 0;
}
