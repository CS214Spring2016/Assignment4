#include "account.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

Account *createAccount(char* name)
{
	Account* account = calloc(1,sizeof(struct account_));
	if(strlen(name) >= 100)
	{
		name[99] = '\0';
	}
	if(account != NULL)
	{
		account->acctName = name;
		account->acctBalance = 0.00f;
		account->isActive = 0;
		return account;
	}
	else
	{
		return NULL;
	}

	return 0;
}


void creditBalance(Account *acc, float amount)
{
	acc->acctBalance += amount;
}


void debitBalance(Account *acc, float amount)
{
	if((acc->acctBalance - amount) < 0)
	{
		printf("invalid debit attempt: insufficient funds\n");
	}
	else
	{
		acc->acctBalance -= amount;
	}
}


void destroyAccount(Account *acc)
{
	if(acc != NULL)
	{
		if(acc->acctName != NULL)
		{
			free(acc->acctName);
		}
		free(acc);
	}
	else
	{
		printf("you tried to a free a null account\n");
		//do nothing
	}

}


void flagActive(Account *acc)
{
	acc->isActive = 1;
}


void flagInactive(Account *acc)
{
	acc->isActive = 0;
}

void printStatus(Account *account)
{
	printf("Account Name: %s\n", account->acctName);
	printf("Account Balance: %.2f\n", account->acctBalance);
	printf("Account active flag: %d\n",account->isActive);
}

