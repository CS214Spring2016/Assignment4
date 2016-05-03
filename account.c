#include "account.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

Bank *openBank()
{
	return (Bank *)calloc(1,sizeof(struct bank_));
}

BankPtr *createBankPtr()
{
	BankPtr *ptr = (BankPtr*)calloc(1,sizeof(struct BankPtr));

	if(ptr == NULL)
	{
		return NULL;
	}

	ptr->bank = openBank();
	ptr->bank->numAccounts = 0;

	if(ptr->bank != NULL)
	{
		return ptr;
	}
	else
	{
		free(ptr);
		return NULL;
	}
}

Account *createAccount(char* name)
{
	Account* account = (Account *)malloc(sizeof(struct account_));
	if(strlen(name) >= 100)
	{
		name[99] = '\0';
	}
	if(name == NULL)
	{
		return NULL;
	}
	if(account != NULL)
	{
		account->acctName = name;
		account->acctBalance = 0.00f;
		account->isActive = 1;
		return account;
	}
	else
	{
		return NULL;
	}

}


void creditBalance(Account *acc, char* amount)
{
	Account *temp;
	temp = acc;
	float am = atof(amount);
	temp->acctBalance += am;
	acc = temp;
}


int debitBalance(Account *acc, char* amount)
{
	float am = atof(amount);
	if((acc->acctBalance - am) < 0)
	{
		return 0;
	}
	else
	{
		acc->acctBalance -= am;
		return 1;
	}
}

int insert(Bank *bank, Account *account)
{
	int ret = 0;
	Account *acc = account;
	int location = bank->numAccounts;
	if((Bank *)(bank) == NULL)
	{
		printf("bank null\n");
	}
	else
	{
		bank->bankAccount[location] = acc;
		bank->numAccounts++;
		return ret;
	}

	return 0;
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

float reportBalance(Account *acc)
{
	Account *temp;
	temp = acc;
	float bal;
	if(temp == NULL)
	{	
		return 0;
	}
	else
	{
		bal = temp->acctBalance;
		return bal;
	}
}

int findAccount(BankPtr *bPtr, char* name)
{
	Bank *bank;
	Account *acct;
	for(int i = 0; i < 20; i++)
	{
		bank = bPtr->bank;
		if((Bank *)(bank) == NULL)
		{
			printf("Bank pointer bank null\n");
		}
		else
		{
			acct = bank->bankAccount[i];
			if((Account *)(acct) == NULL)
			{
				printf("No accounts to search.\n");
			}
			else
			{
				if(strcmp((acct->acctName), name) == 0)
				{
					acct->isActive = 1;
					return i;
					printf("account found and marked active\n");
				}
				else
				{
					printf("no account by that name found\n");
				}
			}
		}

	}

	return 0;
}

void printStatus(BankPtr *bPtr)
{	
	Bank *bank;
	Account *acct;
	char *aName;
	float aBal;
	int aAct;
	for(int i = 0; i < 20; i++)
	{
		bank = bPtr->bank;
		if((Bank *)(bank) == NULL)
		{
			printf("Bank pointer bank null\n");
		}
		else
		{
			acct = bank->bankAccount[i];
			if((Account *)(acct) == NULL)
			{
				//printf("No information for account: %d\n", i);
			}
			else
			{
				aName = acct->acctName;
				aBal = acct->acctBalance;
				aAct = acct->isActive;
				printf("Account Name: %s\n",acct->acctName);
				printf("Account Balance: %.2f\n", acct->acctBalance);
				printf("Active Flag: %d\n", acct->isActive);
			}
		}
	}

	return;
}

