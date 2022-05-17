tictactoe_tests: tictactoe_tests.o tictactoe.o
	gcc tictactoe_tests.o tictactoe.o -o tictactoe_tests

tictactoe_tests.o: tictactoe_tests.c
	gcc -c tictactoe_tests.c

tictactoe.o: tictactoe.c tictactoe.h
	gcc -c tictactoe.c


server: server.o tictactoe.o common.o
	gcc server.o tictactoe.o common.o -o server -lpthread
	

client: client.o tictactoe.o common.o
	gcc client.o tictactoe.o common.o -o client -lpthread

server.o: server.c
	gcc -c server.c 

client.o: client.c
	gcc -c client.c 

common.o: common.c common.h
	gcc -c common.c
clean:
	rm *.o

