#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <signal.h>
#include "server.h"
#include "error.h"
#include "socks4.h"

using namespace std;

static void killself (int sig){
  if (sig == SIGCHLD)
      while (waitpid (-1, NULL, WNOHANG) > 0);
    signal (sig, killself);
}

int main(int argc, char *argv[]){

  if(argc != 2){
    cout << "Usage : ./socksserver {port}\n";
    exit(0);
  }

  signal (SIGCHLD, killself);

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
      cerr << "[New Conection]" << endl;
      Socks4::sockschild(cli_addr);
      exit(0);
    }
    close(newsock); 
  }
}
