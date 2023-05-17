#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <thread>
#include <cmath>
#include <mutex>
#include <condition_variable>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>


// Queue for incoming requests
//std::queue<int> connection_queue;


bool is_prime(int num) {
  if(num <= 1) {
    return false;
  }

  for(int i = 2; i * i <= num; ++i) {
    if(num % i == 0) {
      return false; 
    }
  }
  return true;
}

double calculate_prime(int n) {
   std::vector<int> primes;
    int num = 2;  

    while (primes.size() < n) {
        if (isPrime(num)){
          primes.push_back(num);
        }
            
        num++;
    }
    
    return primes.back();
}

// Function to handle client connections
void handle_connection(int clientSocket)
{
  while(true) { 
    // Receive the buff number from the client
    int buff;
    //                    sockfd        buff    length    flags
    int recv_bytes = recv(clientSocket, &buff, sizeof(buff), 0);
    if (recv_bytes != sizeof(buff)){
      std::cerr << strerror(errno) << std::endl;
      close(clientSocket);
      exit(-1);
    }

    
    double result = calculate_prime(buff);

    // Send the calculated number back to the client
    //                     sockfs       buff     length     flags
    int sent_bytes = send(clientSocket, &result, sizeof(result), 0);

    if (sent_bytes != sizeof(result)){
      std::cerr << "failed to send" << std::endl;
      close(clientSocket);
      exit(-1);
    }
  }

  close(clientSocket);
}

int main(int argc, char** argv) {
  if(argc != 2) {
    std::cerr << "wrong number og arguments" << std::endl;
    exit(-1);
  }
  int port = std::atoi(argv[1]);
  
  // Create a thread pool // max_threads
  std::vector<std::thread> thread_pool(10);


 //  SOCK_STREAM with TCP. with UDP SOCK_DGRAM
  int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (serverSocket < 0) {
    std::cerr << strerror(errno) << std::endl;
    exit(-1);
  }

  // Binding to port
  sockaddr_in serverAddress;

  // For security doing memset()
  memset(&serverAddress, 0, sizeof(sockaddr_in));

  serverAddress.sin_family = AF_INET;
  serverAddress.sin_addr.s_addr = INADDR_ANY;
  serverAddress.sin_port = htons(port);
  
  int binded = bind(serverSocket, reinterpret_cast<sockaddr*>(&serverAddress), sizeof(serverAddress));
  if (binded == -1){
   std::cerr << strerror(errno) << std::endl;
   close(serverSocket);
   exit(-1);    
  }

  // Listen for incoming connections
  //serverSocket to listen, SOMAXCONN for max queue length 
  if (listen(serverSocket, SOMAXCONN) == -1)
  {
    std::cerr << strerror(errno) << std::endl;
    close(serverSocket);
    exit(-1);
  }

  // Accept and handle client connections
  while (true)
  {
    // Accept a client connection
    sockaddr_in clientAddress;
    socklen_t clientAddressLength = sizeof(clientAddress);

    memset(&clientAddress, 0, clientAddressLength);

    //accept from client      int sockfd, struct sockaddr *restrict addr, socklen_t *restrict addrlen
    int clientSocket = accept(serverSocket, reinterpret_cast<sockaddr*>(&clientAddress), &clientAddressLength);
    if (clientSocket == -1) {
      std::cerr << strerror(errno) << std::endl;
      continue;
    }

    // A new thread to handle the client connection //runs handle_connection on clientSocket
    std::thread clientThread(handle_connection, clientSocket);
    clientThread.detach();
  }

  // Close the server socket
  close(serverSocket);
  return 0;
}