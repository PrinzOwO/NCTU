#include "server.h"

int Server::passiveTCP(int port, int qlen){
  int sockfd;
  struct sockaddr_in serv_addr;

  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
      Error::error("Socket open fail");
      exit(-1);
    }
  memset((char *) &serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = htonl (INADDR_ANY);
  serv_addr.sin_port = htons (port);

  if (bind (sockfd, (const struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
      Error::error("Cannot bind local address");
      exit(-1);
    }
  if (listen (sockfd, qlen) < 0) {
      Error::error("Listen failed");
      exit(-1);
    }
  return sockfd;
}

