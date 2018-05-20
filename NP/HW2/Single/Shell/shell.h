#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include "error.h"
#include "define.h"
#include "chat.h"
#include <vector>
#include <map>
#include <unordered_map>
#include <sys/wait.h>

using namespace std;

#ifndef __SHELL__
#define __SHELL__
namespace Shell{
  void prompt(int);
  void welcomemsg(int);
  string readmsg(int);
  
  struct Info{
    int valid;
    int sockfd;
    int nowid;
    int ppnum;
    string buf;
    string rawbuf;
    string cmd[MAXCMD][MAXSUBCMD];
    char **argvs[MAXCMD];
    char *argv[MAXCMD][MAXSUBCMD];
    vector<int> pp2;

    unordered_map<int, string> catchbuf;

    unordered_map<int, string> filein;
    unordered_map<int, string> fileout;

    unordered_map<int, int> clientin;
    unordered_map<int, int> clientout;

    map<string, string> env;

    void init();
    int setValid();
    void setReady();
    void strip();
    void setCmd();
    void setPipe();
    void setOtherIO();

    void printerror(int);
    void callSetenv(int);
    void callGetenv(int);
    void exec(Chat::Table &);

    void loadEnv();
    void setEnv(string, string);

    Info(int sockfd = 0){
      this->valid = 1;
      this->sockfd = sockfd;
      this->nowid = 0;
      for(int i = 0; i < MAXCMD; i++){
        this->argvs[i] = NULL;
        for(int j = 0; j < MAXSUBCMD; j++) this->argv[i][j] = NULL;
      }
      this->catchbuf.clear();
    }
  };
}
#endif
