#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/sendfile.h>
#include <fcntl.h>
#include <sys/stat.h> 
#include "error.h"

using namespace std;


#ifndef __SERVER__
#define __SERVER__
#define prepath "/u/cs/104/0416042/public_html/"


namespace Server{

  struct Header{
    string method;
    string URI;
    string qstr;
    string proto;
    string abs_path = string(prepath);
  };

  int passiveTCP(int, int);
  int httpd();
  void readpkg(Server::Header &);
  void set_envs(Server::Header &);
  void exec_hw3();
  void exec_cgi();
  void exec_htm();
}
#endif
