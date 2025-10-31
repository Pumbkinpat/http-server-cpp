#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>

int main() {
  int domain = AF_INET;
  int type = SOCK_STREAM;
  int server_socket_fd = socket(domain, type, 0);

  if (server_socket_fd < 0) {
    perror("Unable to open a socket");
    return(EXIT_FAILURE);
  }

  std::cout << "Socket " << server_socket_fd << " opened" << std::endl;

  sockaddr_in server_address;
  std::memset(&server_address, 0, sizeof(server_address));

  server_address.sin_family = domain;
  server_address.sin_addr.s_addr = INADDR_ANY;
  server_address.sin_port = htons(8080); // makes sure the port number are stored in memory in network byte order

  if (bind(server_socket_fd, (sockaddr*)& server_address, sizeof(server_address)) < 0) {
    perror("Unable to bind to network");
    return(EXIT_FAILURE);
  }

  int status = listen(server_socket_fd, 10);
  if (status < 0) {
    perror("listen failed");
    return (EXIT_FAILURE);
  }
  std::cout << "Server listening on port 8080...\n";

  sockaddr client_address;
  socklen_t client_address_len = sizeof(client_address);
  int client_socket_fd = accept(server_socket_fd, (sockaddr*)& client_address, &client_address_len  ); // store incoming client connection in a queue and return a new socket on first connect

  if (client_socket_fd < 0) {
    perror("fail to accept client connection");
    return EXIT_FAILURE;
  }

  char buffer[1024]; // receives data in stream of bytes so we need an array of bytes to gurantee that we get all data 
  size_t bytes_received = recv(client_socket_fd, buffer, sizeof(buffer), 0);
  if (bytes_received == -1) {
      perror("receiv failed");
  } else if (bytes_received == 0) {
      printf("Client disconnected.\n");
  } else {
      buffer[bytes_received] = '\0'; 
      printf("Received from client: \r\n%s\r\n", buffer);
  }

  size_t length = 8; 
  char* startIndex = strstr(buffer, "/");
  char* endIndex = strstr(buffer, " HTTP");

  char* message = (char*)malloc(sizeof(char) * (length + 1));
  strncpy(message, startIndex, endIndex - startIndex);

  message[length] = '\0';

  size_t bytes_sent = send(client_socket_fd, message, strlen(message), 0);

  if (bytes_sent == -1) {
      perror("send failed");
  } else if (bytes_sent < strlen(message)) {
      // Not all data was sent, handle partial send (e.g., retry sending remaining data)
      fprintf(stderr, "Partial send: sent %zd out of %zu bytes\n", bytes_sent, strlen(message));
  } else {
      printf("Successfully sent message: %s\n", message);
  } 
  
  close(client_socket_fd);
  close(server_socket_fd);
  return 0;
}