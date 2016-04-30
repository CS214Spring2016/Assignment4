COMPILER = gcc
CCFLAGS = -Wall -g

all: server client
serv: server
cli: client


server: server.o
	$(COMPILER) $(CCFLAGS) -lpthread -o server server.o
client: client.o
	$(COMPILER) $(CCFLAGS) -lpthread -o client client.o
server.o: server.c server.h
	$(COMPILER) $(CCFLAGS) -c server.c
client.o: client.c client.h
	$(COMPILER) $(CCFLAGS) -c client.c
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

c:
	@rm -f bank
	@rm -f *.o