#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstring>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "define.h"
#include "server.h"

using namespace std;

#ifndef __SOCK4__
#define __SOCK4__

namespace Socks4{

  string getIP(string IP);

  struct Rule{
    int pass;      // pass : 1, else : 0
    int mode;      // b    : 1, else : 0
    string IP;
    int mask;

    void setIpMask(string);
    Rule(int pass = 0, int mode = 0, string IP = "", int mask = 32) : 
      pass(pass), mode(mode), IP(IP), mask(mask) {}
  };

  struct PF{
    vector<Rule> rule;

    void setPF();
    bool fit(string, int);
    PF(vector<Rule> rule = vector<Rule>()) : rule(rule) {}
  };

  struct Request{
    uchar vn;
    uchar cd;
    uint destPort;
    string destIP;
    uint srcPort;
    string srcIP;
    string user;

    bool AC;

    void show();
    string setDestIP(char, char, char, char);
    Request(uchar *, string, string, uint, bool);
  };

  struct Reply{
    uchar vn;
    uchar cd;
    string destIP;
    uint destPort;

    void sendreply();
    Reply(uchar vn = 0, uchar cd = 91, string destIP = "0.0.0.0", uint destPort = 0) :
      vn(vn), cd(cd), destIP(destIP), destPort(destPort) {} 
  };

  void sockschild(struct sockaddr_in);
  int connect(Request &);
  int bind(Reply &);
  void exchange(int);
}
#endif
