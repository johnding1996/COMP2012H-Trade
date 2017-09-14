all: server_main client_main
server_main: server_main.o server.o
	g++ -o server_main server_main.o server.o  
client_main: client_main.o client.o
	g++ -o client_main client_main.o client.o
server_main.o: server_main.cpp proj1.h
	g++ -c server_main.cpp
client_main.o: client_main.cpp proj1.h
	g++ -c client_main.cpp
server.o: server.cpp
	g++ -c server.cpp
client.o: client.cpp
	g++ -c client.cpp
clean:
	rm *.o
