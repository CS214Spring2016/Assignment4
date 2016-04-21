#include "account.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
	printf("this is main running\n");
	Account *test;
	char *inputName = argv[1];
	test = createAccount(inputName);

	creditBalance(test,420.69f);
	printStatus(test);

	debitBalance(test,419);
	printStatus(test);

	debitBalance(test, 40);
	printStatus(test);












	return 0;
}