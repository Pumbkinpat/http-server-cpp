#include <iostream>
#include <cstring>
#include <stdexcept>
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
  size_t bytes_received = recv(client_socket_fd, buffer, sizeof(buffer), 0); // receives data by bytes then stores into buffer array

  if (bytes_received == -1) {
      perror("receiv failed");
  } else if (bytes_received == 0) {
      printf("Client disconnected.\n");
  } else {
      buffer[bytes_received] = '\0'; 
      printf("Received from client: \r\n%s\r\n", buffer);
  }

  char* startIndex = std::strstr(buffer, "HTTP");
  char* endIndex = std::strstr(buffer, "\r\n");

  try {
    // make sure the distance between start and end is not negative if not there will be segmentation fault 
    if (!endIndex) {
      throw std::invalid_argument("seems like strstr() method can not find the end");
    }

    if (endIndex - startIndex <= 0) {
      throw std::length_error("seems like end in less then start!");
    }
  } catch(std::length_error& e) {
    std::cout << "Length error: " << e.what() << std::endl;
  } catch (std::invalid_argument& e) {
    std::cout << "Invalid argument: " << e.what() << std::endl;
  }

  char* startURLPath = std::strstr(buffer, "/");
  char* endURLPath = std::strstr(buffer, " HTTP");

  char* message = (char*)malloc(sizeof(char) * (endIndex - startIndex + 1));
  std::strncpy(message, startIndex, endIndex - startIndex);

  if (endURLPath - startURLPath > 1) {
    std::strcat(message, " 404 Not Found\r\n\r\n"); 
    message[endIndex - startIndex + 18] = '\0';
  } else {
    std::strcat(message, " 200 OK\r\n\r\n"); 
    message[endIndex - startIndex + 11] = '\0';
  }

  size_t bytes_sent = send(client_socket_fd, message, strlen(message) + 1, 0);

  if (bytes_sent == -1) {
      perror("send failed");
  } else if (bytes_sent < strlen(message)) {
      // Not all data was sent, handle partial send (e.g., retry sending remaining data)
      fprintf(stderr, "Partial send: sent %zd out of %zu bytes\n", bytes_sent, strlen(message));
  } else {
      printf("Successfully sent message: %s\n", message);
  } 
  
  shutdown(client_socket_fd, SHUT_WR);
  close(client_socket_fd);
  close(server_socket_fd);
  return 0;
}