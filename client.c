#include "client.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

//dope error stuff
void error(char* msg)
{
	perror(msg);
	exit(0);
}

void startClient(char* serverAddress)
{
	
}