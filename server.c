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



BankPtr *bankPtr;

//fun error method #extensible #objectoriented
void error(char* msg)
{
	perror(msg);
	puts("Yea I hear ya buddy");
	exit(0);
}


/*---------------------------------------------------STRING PARSING STUFF----------------------------------------------*/

void getCommands(char *input, void *socketdesc)
{
	// int sock = *(int*)socketdesc;
	// char *reply = calloc(1,100);
	const char *dict[7];
	char* ptr;
	char argument[100];
	int len;
	Account *temp;
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
			len = strlen(dict[i]);
			memset(argument,0,100);
			strncpy(argument, &input[len],strlen(input));
			switch(i)
			{
				//open
				case 0:
					temp = createAccount(argument);
					temp->isActive = 1;
					insert(bankPtr->bank, temp);
					free(temp);
					break;
				case 1:
					findAccount(bankPtr, argument);
					break;
				case 2:
					printf("credit");
					break;
				case 3:
					printf("debit");
					break;
				case 4:
					printf("balance");
					break;
				case 5:
					printf("finish");
					break;
				case 6:
					printf("exit");
					break;	
				default:
					printf("fell through");
			}
			return;
		}
		return;
	}
	return;

}


/*--------------------------------------------ACCOUNT PRINTING THINGS GO HERE-----------------------------------------------*/
void *printInfo()
{
    do
    {
        sleep(5);
        printStatus(bankPtr);
        puts("END OF REPORT");

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
	int incomingmessagesize;
	int sock = *(int*)socketdesc;
	char inmessage[256];
	char *message = "Hello friend, you have connected to Barrett & Shafran Community Trust";
	write(sock, message, strlen(message));
	//memset(message, 0, 255);

	//this is where we get messages from the client, inmessage is the command
	while((incomingmessagesize = recv(sock, inmessage,256,0))>0){
		//get commands takes a socket for writeback to client
		getCommands(inmessage, socketdesc);
	}

	puts("Client Disconnected");

	memset(message, 0, 255);
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

	bankPtr = createBankPtr();

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
