all: client

client: client.o
	g++ -o client client.o
	
client.o: client.cpp
        g++ -c client.cpp
clean:
       rm -f client.o client