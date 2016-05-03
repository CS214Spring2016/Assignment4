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
char *dict[8] = {" ","open","start","credit","debit","balance","finish","exit"};
pthread_mutex_t pleasework;
pthread_mutex_t otherone;


//fun error method #extensible #objectoriented
void error(char* msg)
{
	perror(msg);
	puts("Yea I hear ya buddy");
	exit(0);
}

void accountActions(int cmd, Account *account)
{

}


/*---------------------------------------------------STRING PARSING STUFF----------------------------------------------*/

int getCommands(char *input, int socketdesc, int index)
{
	pthread_mutex_lock(&pleasework);



	char* ptr;
	char argument[100];
	memset(argument,0,100);
	int len;
	int keyword = 0;
	int i;



	for(i = 0; i < 8; i++)
	{
		if(strstr(input,dict[i]) != NULL)
		{
			ptr = strstr(input,dict[i]);
			len = strlen(dict[i]);
			strncpy(argument, &input[len],strlen(input));
			keyword = i;
			break;	
		}
	}

	printf("index: %d\n", index);

	if(index > -1)
	{
		printf("%s\n",bankPtr->bank->bankAccount[index]->acctName);
	}



			// temp = createAccount(argument);
			// temp->isActive = 1;
			// insert(bankPtr->bank, temp);
			// free(temp);
			// send((*(int*)socketdesc), "Account Opened.", 20,0);

	//open autostarts
	if(keyword == 1)
	{
		if(index != -1)
		{
			send(socketdesc, "Account already started",30,0);
			index = -1;
			
		}
		else
		{
			index = insert(bankPtr->bank, createAccount(argument));
			send(socketdesc, "Account Opened.", 20,0);
			
		}
	}
	//start
	else if(keyword == 2)
	{
		if(index != -1)
		{
			send(socketdesc, "Account already started",30,0);
			index = -1;
			
		}
		index  = findAccount(bankPtr, argument);
		
		//free(keyword);
	}
	//credit
	else if(keyword == 3)
	{
		if(index == -1)
		{
			send(socketdesc, "No account open/not in session", 40, 0);
			
		}
		else
		{
			creditBalance((bankPtr->bank->bankAccount[index]) , argument);
			
		}
	}
	//debit
	else if(keyword == 4)
	{
		if(index == -1)
		{
			send(socketdesc, "No account open/not in session", 40, 0);
			
		}
		else
		{
			if(debitBalance((bankPtr->bank->bankAccount[index]) , argument) == 0)
			{
				send(socketdesc, "Insufficient Funds.", 40, 0);	
				
			}
			else
			{
				send(socketdesc, "Account debited", 20, 0);
				
			}
		}
	}
	//balance report
	else if(keyword == 5)
	{
		if(index == -1)
		{
			send(socketdesc, "no account open/not in session", 40, 0);
			
		}
		else
		{
			float bal = reportBalance(bankPtr->bank->bankAccount[index]);
			char tosend[250];
			snprintf(tosend, 250, "Your balance is: %.2f",bal);
			send(socketdesc,tosend,250,0);
			
		}
	}
	else if(keyword == 6)
	{
		if(index == -1)
		{
			send(socketdesc, "no account open/not in session", 40, 0);
			
		}
		else
		{
			bankPtr->bank->bankAccount[index]->isActive = 0;
			send(socketdesc,"session closed",20,0);
			index = -1;

		}
	}
	//exit
	else if(keyword == 7)
	{
		//puts("Client Disconnected");
		bankPtr->bank->bankAccount[index]->isActive = 0;
		write(socketdesc,"Exiting session...",30);
		close(socketdesc);
		index = -2;
		pthread_mutex_unlock(&pleasework);
	}
	else
	{
		write(socketdesc,"Plz no",30);
	}

	
	
	pthread_mutex_unlock(&pleasework);
	return index;

}


void removeSubstring(char *s,const char *toremove)
{
	while( (s=strstr(s,toremove)) )
	{
    	memmove(s,s+strlen(toremove),1+strlen(s+strlen(toremove)));
	}
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
	int location = -1;
	int incomingmessagesize;
	int sock = (*(int*)socketdesc);
	char inmessage[256];
	char *message = "Hello friend, you have connected to Barrett & Shafran Capital";
	write((*(int*)socketdesc), message, strlen(message));

	

	//this is where we get messages from the client, inmessage is the command
	while(1)
	{
		if((incomingmessagesize = recv(sock, inmessage,256, MSG_WAITALL)) != 0)
		{
			//write(sock,inmessage,sizeof(inmessage));
			location = getCommands(inmessage, sock, location);
			if(location == -2)
			{
				break;
			}

		}
		else
		{
			return NULL;
		}
		bzero(inmessage,sizeof(inmessage));
	}


	puts("Client Disconnected");
	close(sock);

	//memset(message, 0, 255);
	puts("here");
	return (NULL);
}

void *session_acceptor()
{
	int sockfd = -1;
	int portno = -1;
	int cliSocket = -1;

	if(pthread_mutex_init(&pleasework, NULL) != 0)
	{
		puts("mutex init failed");
	}



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
	//void* clientStatus;
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

	pthread_join(client_thread, NULL);
	pthread_mutex_destroy(&pleasework);
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
