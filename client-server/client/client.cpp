#include <iostream>
#include <string>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>


int main(int argc, char** argv) {
  if(argc != 3) {
    std::cerr << "wrong number og arguments" << std::endl;
    exit(-1);
  }

  const char* host = argv[1]; 
  int port = atoi(argv[2]);

  // create a socket to connect to server
  int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (clientSocket < 0) {
    std::cerr << strerror(errno) << std::endl;
    exit(errno);
  }

  // Binding to port
  sockaddr_in serverAddress;

  // For security doing memset()
  memset(&serverAddress, 0, sizeof(sockaddr_in));

  // Connect to the server
  serverAddress.sin_family = AF_INET;
  serverAddress.sin_addr.s_addr = inet_addr(host);
  //if not given then getting standart port
  serverAddress.sin_port = htons(port);
  int connected = connect(clientSocket, reinterpret_cast<sockaddr*>(&serverAddress), sizeof(serverAddress));

  if (connected == -1) {
    std::cerr << strerror(errno) << std::endl;
    close(clientSocket);
    exit(-1);
  }

  while(true) {
    int N;
    std::cout << "Enter number: ";
    std::cin >> N;
    
    if(N == 0) { 
        break; 
    }
    //for BigEnd
    N = htons(N);
    //sending
    int sent = send(clientSocket, &N, sizeof(N), 0);
    if(sent == -1) {
      std::cerr << strerror(errno) << std::endl;
      close(clientSocket);
      exit(-1);
    }

    double res;
    //recieving
    int recieved = recv(clientSocket, &res, sizeof(res), 0);
    if(recieved == -1 || recieved == 0) {
      std::cerr << strerror(errno) << std::endl;
      close(clientSocket);
      exit(-1);
    }
    res = ntohl(res);
    std::cout << "The result is " << res << std::endl;
  }
  close(clientSocket);
  return 0;
}