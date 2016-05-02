#include "account.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <ctype.h>
#include <unistd.h>

char *dict[7] = {"open","start","credit","debit","balance","finish","exit"};
BankPtr *bankPtr;

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
void fixString(char *source)
{
	toLowercase(source);
	stripNonAlpha(source);
	removeSpaces(source);
}
int getCommands(char *input, int currentaccount)
{
	//int replysock = *(int*)socketdesc;
	char* ptr;
	char argument[100];
	int len;
	int keyword = -1;
	Account *temp;
	Account *activeAccount;



	for(int i = 0; i < 7; i++)
	{
		if(strstr(input,dict[i]) != NULL)
		{
			ptr = strstr(input,dict[i]);
			len = strlen(dict[i]);
			memset(argument,0,100);
			strncpy(argument, &input[len],strlen(input));
			keyword = i;
			puts(dict[i]);
			puts(argument);
		}
	}

			// temp = createAccount(argument);
			// temp->isActive = 1;
			// insert(bankPtr->bank, temp);
			// free(temp);
			// send(replysock, "Account Opened.", 20,0);

	switch(keyword){
		case 0:
				//temp = createAccount(argument);
				//temp->isActive = 1;
				//insert(bankPtr->bank, temp);
				//free(temp);
				puts("great job");
				break;
			case 1:
				findAccount(bankPtr, argument);
				break;
			case 2:
				puts("credit");
				break;
			case 3:
				puts("debit");
				break;
			case 4:
				puts("balance");
				break;
			case 5:
				puts("finish");
				break;
			case 6:
				puts("exit");
				break;
			default:
				puts("fell through");
				break;
	}


	return keyword;
}


BankPtr *bankPtr;
int main(){
	bankPtr = createBankPtr();
	int outgoingmessagesize = 265;
	int current_account = -1;
		//int sock = *(int*)socketdesc;
		char outmessage[outgoingmessagesize];
	while(fgets(outmessage,outgoingmessagesize,stdin)){
			fixString(outmessage);
			//write(sock, outmessage, outgoingmessagesize);
			printf("%d\n",getCommands(outmessage, current_account));
			sleep(2);
		}
	return 0;
}
