all: server

server: server.o
	g++ -o server server.o
	
server.o: server.cpp
        g++ -c server.cpp
clean:
       rm -f server.o server