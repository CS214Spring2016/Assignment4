#ifndef ACCOUNT_H
#define ACCOUNT_H
#include <stdlib.h>

struct account_
{
	char *acctName;
	float acctBalance;
	int isActive;

};
typedef struct account_ Account;

struct bank_
{
	Account *accounts[20];
};
typedef struct bank_ Bank;

Account *createAccount(char* name);
void creditBalance(Account *acc, float amount);
void debitBalance(Account *acc, float amount);
void destroyAccount(Account *acc);
void flagActive(Account *acc);
void flagInactive(Account *acc);
void printStatus(Account *account);








#endif