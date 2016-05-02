COMPILER = gcc
CCFLAGS = -Wall -g

all: server client
serv: server
cli: client
at: accounttester


server: server.o account.o
	$(COMPILER) $(CCFLAGS) -lpthread -o server server.o account.o	
client: client.o account.o
	$(COMPILER) $(CCFLAGS) -lpthread -o client client.o account.o
server.o: server.c server.h account.h
	@$(COMPILER) $(CCFLAGS) -c server.c
client.o: client.c client.h account.h
	@$(COMPILER) $(CCFLAGS) -c client.c
account.o: account.c account.h
	@$(COMPILER) $(CCFLAGS) -c account.c

accounttester: accounttester.o account.o
	$(COMPILER) $(CCFLAGS) -lpthread -o accounttester accounttester.o account.o	
accounttester.o: accounttester.c account.h
	@$(COMPILER) $(CCFLAGS) -c accounttester.c



# transfer:
# 	cp sorted-list.h sorted-list.c main.c makefile testplan.txt readme.pdf asst1
# 	tar cfz Asst4.tgz asst4
# 	scp Asst1.tgz kpb81@java.cs.rutgers.edu:~/CS214

clean:
	@rm -f *.out
	@rm -f *.o

c:
	@rm -f *.o