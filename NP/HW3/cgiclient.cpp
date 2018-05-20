#include <iostream>
#include <cstdio>
#include <cstdlib>
#include "server.h"
#include "error.h"
#include <sys/select.h>
#include <netdb.h>
#include "cgi.h"

using namespace std;

int main(){
  
  Cgi::Remote_host host[6];
  Cgi::FDS fds;
  Cgi::sethosts(host, fds);
  Cgi::preprint(host);
  int flag = 5;
  for(int i = 1; i < 6; i++){
    if(host[i].sockfd == 0){
      flag--;
      continue;
    }
    Cgi::connectsock(host[i], fds);
  }

  while(flag){
    memcpy (&fds.rfds, &fds.afds, sizeof(fds.rfds));
    if(select(fds.nfds, &fds.rfds, NULL, NULL, 0) < 0){
      cerr << strerror(errno) << endl;
      Error::error("select fail");
      return -1;
    }
    for(int i = 1; i < 6; i++){
      if(host[i].isconnect == 2 && FD_ISSET(fileno(host[i].fin), &fds.rfds)){
        sendmsg(host[i], fds, i);
      }
      if(host[i].isconnect == 1 && FD_ISSET(host[i].sockfd, &fds.rfds)){
        recvmsg(host[i], i);
      }
    }
    usleep(1000);
  }
  Cgi::posprint();
}
