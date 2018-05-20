#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <map>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "Shell_info.h"

using namespace std;

//#ifndef
#define MAXLINE 512
//#endif
int SERV_TCP_PORT;
int sockfd;
Usr usr;

void read_usrshm(int sig){
  Msg_info *addr = (Msg_info *)shmat(usr.msg_id, NULL, 0);
  write(usr.mysockfd, addr[usr.myid].msg, strlen(addr[usr.myid].msg));
  strcpy(addr[usr.myid].msg, "\0");
  shmdt(addr);
}

void child_exit(int sig){
  while(waitpid(-1, NULL, WNOHANG) > 0);
}

void server_exit(int sig){
  cout << "exit" << endl;
  shmctl(usr.shm_id, IPC_RMID, NULL);
  shmctl(usr.msg_id, IPC_RMID, NULL);
  shmctl(usr.c2c_id, IPC_RMID, NULL);
  exit(0);
}

void shell(int sockfd, Usr &usr){
  cout << sockfd << endl;
  signal(SIGUSR1, read_usrshm);
  welcome(sockfd);
  usr.login_msg(usr.myid);
  Login login(sockfd);
  login.run(usr);
}

int main(int argc, char *argv[]){
  
  SERV_TCP_PORT = atoi(argv[1]);

  int newsockfd, childpid;
  socklen_t clilen;
  struct sockaddr_in serv_addr, cli_addr;

  usr.create_shm();

  if( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0 ){  
    perror("Server : can't open stream socket.");      
  }

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

  signal(SIGCHLD, child_exit);
  signal(SIGINT, server_exit);

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
      usr.addusr(cli_addr, newsockfd);
      usr.setpid(usr.myid, getpid());

      close(sockfd);
      shell(newsockfd, usr);
      exit(-1);
    }
    // Parent Process
    close(newsockfd);
  }

  return 0;
}
