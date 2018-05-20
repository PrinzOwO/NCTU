#include "server.h"
#include "chat.h"
#include "shell.h"
#include <sys/select.h>

using namespace std;

Chat::Table online;
Shell::Info usrshell[50];

bool exec(int, string);

int main(int argc, char* argv[]){
  if(argc != 2) {
    cout << "Usage : ./single {port}" << endl;
    exit(0);
  }

  int nfds = getdtablesize();
  fd_set rfds, afds;
  socklen_t clilen = sizeof (struct sockaddr_in);
  struct sockaddr_in cli_addr;

  int sockfd = Server::passiveTCP(atoi(argv[1]), 5);
  
  FD_ZERO(&afds);
  FD_SET(sockfd, &afds);

  while(1){
    memcpy(&rfds, &afds, sizeof(rfds));
    if(select(nfds, &rfds, NULL, NULL, NULL) < 0){
      Error::error("Select Fail");
      exit(0);
    }
    if(FD_ISSET(sockfd, &rfds)) {
      int newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
      if(newsockfd == 0){
        Error::error("New connect fail");
      }
      else{
        cout << "New connection : " << newsockfd << endl;
        FD_SET(newsockfd, &afds);
        char *newIP = inet_ntoa((in_addr) cli_addr.sin_addr);
        int newPort = ntohs(cli_addr.sin_port);
        Shell::welcomemsg(newsockfd);
        online.add(newsockfd, string(newIP), newPort);
        Shell::prompt(newsockfd);
      }  
    }
    for(int i = 4; i < nfds; i++){
      if(FD_ISSET(i, &rfds)){
        string msg = Shell::readmsg(i); 
        if(msg.substr(0, 4) == "exit"){
          cout << "Connection exit : " << i << endl;
          online.del(i);
          usrshell[i].env.clear();
          close(i);
          FD_CLR(i, &afds);
        }
        else{
          exec(i, msg); 
        }
      }
      usleep(50);
    }
  }
  
}

bool exec(int sockfd, string msg){
  usrshell[sockfd].loadEnv();
  usrshell[sockfd].sockfd = sockfd;
  usrshell[sockfd].buf += msg;
  if(usrshell[sockfd].buf.back() != '\n') return 0;
  if(usrshell[sockfd].setValid() == 0){
    usrshell[sockfd].init();
    Shell::prompt(sockfd);
    return 1;
  }
  usrshell[sockfd].setReady();
  if(usrshell[sockfd].rawbuf.size() == 3 && usrshell[sockfd].rawbuf == "who")
    online.show(sockfd);
  else if(usrshell[sockfd].rawbuf.size() > 3 && usrshell[sockfd].rawbuf.substr(0, 4) == "who ")
    online.show(sockfd);
  else if(usrshell[sockfd].rawbuf.substr(0, 4) == "name" && usrshell[sockfd].rawbuf[4] == ' ') 
    online.name(sockfd, usrshell[sockfd].rawbuf.substr(5, int(usrshell[sockfd].rawbuf.size()))); 
  else if(usrshell[sockfd].rawbuf.substr(0, 4) == "yell" && usrshell[sockfd].rawbuf[4] == ' ')
    online.yell(sockfd, usrshell[sockfd].rawbuf.substr(5, int(usrshell[sockfd].rawbuf.size())));
  else if(usrshell[sockfd].rawbuf.substr(0, 4) == "tell" && usrshell[sockfd].rawbuf[4] == ' ')
    online.tell(sockfd, usrshell[sockfd].rawbuf.substr(5, int(usrshell[sockfd].rawbuf.size())));
  else{
    usrshell[sockfd].strip();
    usrshell[sockfd].setPipe();
    usrshell[sockfd].setOtherIO();
    usrshell[sockfd].setCmd();

//    cout << "[Debug] " << sockfd << " : " << usrshell[sockfd].buf << endl;

    usrshell[sockfd].exec(online);
    usrshell[sockfd].nowid--;
  }
  
  usrshell[sockfd].nowid++;
  usrshell[sockfd].init();
  Shell::prompt(sockfd);
  return 0;
}
