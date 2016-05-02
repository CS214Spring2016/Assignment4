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

void insert(Bank *bank, Account *account)
{
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
	}

	return;
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

char *reportBalance(Account *acc)
{
	char *toReturn = calloc(1,20);
	float bal;
	if(acc == NULL)
	{
		char *message = "Null balance. Please call your bank's local branch.";
		return message;
	}
	else
	{
		bal = acc->acctBalance;
		snprintf(toReturn, 20, "%.2f", bal);
		return (char*)toReturn;	
	}
}

void findAccount(BankPtr *bPtr, char* name)
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
					printf("account found and marked active\n");
				}
				else
				{
					printf("no account by that name found\n");
				}
			}
		}

	}

	return;
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
				printf("No information for account: %d\n", i);
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

