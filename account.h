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
	Account *bankAccount[20];
	int numAccounts;
};
typedef struct bank_ Bank;

struct BankPtr
{
	Bank *bank;
};
typedef struct BankPtr BankPtr;

Bank *openBank();
BankPtr *createBankPtr();
Account *createAccount(char* name);
void creditBalance(Account *acc, char* amount);
int debitBalance(Account *acc, char* amount);
void destroyAccount(Account *acc);
void flagActive(Account *acc);
void flagInactive(Account *acc);
void printStatus(BankPtr *bPtr);
int findAccount(BankPtr *bPtr, char* name);
int insert(Bank *bank, Account *account);
float reportBalance(Account *account);







#endif