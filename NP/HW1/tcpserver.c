#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "shell.h"

using namespace std;

//#ifndef
#define SERV_TCP_PORT 7000
#define MAXLINE 512
//#endif

void shell(int sockfd){
  Shell::Login login(sockfd);
  login.run();
}

int main(){
  int sockfd, newsockfd, childpid;
  socklen_t clilen;
  struct sockaddr_in serv_addr, cli_addr;
  
  /* Open TCP Socket
   * int socket(int domain, int type, int protocol);
   * AF_INIT : IPV4, AF_INET6 : IPV6
   * SOCK_STREAM : TCP, SOCK_DGRAM : UDP
   * 0 : default protocol
   */
  if( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0 ){  
    perror("Server : can't open stream socket.");      
  }

  /* Set Server component 
   * bzero()    : init
   * sin_family : domain type
   * sin_addr   : set ip
   * sin_port   : set port
   */
  memset( (char*)&serv_addr, '\0',sizeof(serv_addr) );
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); 
  serv_addr.sin_port = htons(SERV_TCP_PORT);


  if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
    perror("server: can't bind local address");
    exit(1);
  }
  /* Listen
   * int listen(int sockfd, int backlog);
   * backlog : maximum number of the connection to server
   */
  listen(sockfd, 5);

  while(1){
    clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd, (struct sockaddr*) &cli_addr, &clilen);
    if(newsockfd < 0){
      perror("server: accept error");
      continue;
    }
    
    if( (childpid = fork()) < 0) perror("server: fork error");
    else if(childpid == 0){
      // Child Process
      close(sockfd);
      shell(newsockfd);
      exit(-1);
    }
    // Parent Process
    close(newsockfd);
  }

  return 0;
}
