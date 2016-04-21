COMPILER = gcc
CCFLAGS = -Wall -g

all: bank

bank: bank.o account.o
	$(COMPILER) $(CCFLAGS) -o bank bank.o account.o
bank.o: bank.c account.h
	$(COMPILER) $(CCFLAGS) -c bank.c
account.o: account.c account.h
	$(COMPILER) $(CCFLAGS) -c account.c

# transfer:
# 	cp sorted-list.h sorted-list.c main.c makefile testplan.txt readme.pdf asst1
# 	tar cfz Asst4.tgz asst4
# 	scp Asst1.tgz kpb81@java.cs.rutgers.edu:~/CS214

clean:
	@rm -f bank
	@rm -f *.o