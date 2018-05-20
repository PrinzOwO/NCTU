#include <iostream>
#include <cstdio>
#include <cstdlib>
#include "server.h"
#include "error.h"

using namespace std;


int main(int argc, char *argv[]){

  if(argc == 1){
    cout << "Usage : ./httpd {port}\n";
    exit(0);
  }

  int listensock, newsock, port = atoi(argv[1]);
  socklen_t clilen;
  pid_t childpid;
  struct sockaddr_in cli_addr;

  listensock = Server::passiveTCP(port, 5);

  while(1){
    newsock = accept(listensock, (struct sockaddr *)&cli_addr, &clilen);
    if(newsock < 0){
      Error::error("Accept fail");
      continue;
    }
    if((childpid = fork()) < 0) {
      Error::error("Fork fail");
      exit(-1);
    }
    else if(childpid == 0){
      dup2(newsock, 0);
      dup2(newsock, 1);

      close(newsock);
      close(listensock);
      exit(Server::httpd());
    }
    close(newsock); 
  }
}


