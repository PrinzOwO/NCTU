#include <iostream>
#include <unistd.h>
#include <set>
#include <map>

using namespace std;

#ifndef __CHAT__
#define __CHAT__

namespace Chat{

  struct Info{
    string name;
    string IP;
    int port;
    int sockfd;

    Info(string name = "(no name)", string IP = "", int port = 0, int sockfd = 0) :
      name(name), IP(IP), port(port), sockfd(sockfd) {};
  };

  struct Table{
    map<int, Chat::Info> table;
    map<int, int> fd2id;
    string mailbox[50][50];

    int add(int, string, int);
    void del(int);
    void show(int);
    void name(int, string);
    void yell(int, string);
    void tell(int, string);

    int sendmsg(int, int, string, string);
    string recvmsg(int, int, string);
  };

}

#endif
