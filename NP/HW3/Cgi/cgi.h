#include <cstdio>
#include <cstring>
#include <sstream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/sendfile.h>
#include <fcntl.h>
#include <sys/stat.h> 
#include <errno.h>
#include <netdb.h>
#include "error.h"

using namespace std;

#ifndef __CGI__
#define __CGI__
#define pt "% "
namespace Cgi{
  struct FDS{
    int nfds = 3;
    fd_set rfds, afds;
  };

  struct Remote_host{
    struct sockaddr_in addr;
    FILE *fin = NULL;
    int sockfd = 0;
    int isconnect = 0;
    string ip;
    string port;
    string filename = "/u/cs/104/0410750/public_html/";
  };

  void sethosts(Cgi::Remote_host *, Cgi::FDS &);
  void connectsock(Cgi::Remote_host &, Cgi::FDS &);
  void sendmsg(Cgi::Remote_host &, Cgi::FDS &, int);
  void recvmsg(Cgi::Remote_host &, int);
  void preprint(Cgi::Remote_host *);
  void print(string, int);
  void posprint();
};
#endif
