#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stack>
#include <unordered_map>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/ipc.h>

#define MAXBUF     10000
#define MAXCMD     5010
#define MAXCMDLINE 300

using namespace std;
  

#ifndef __SHELL__
#define __SHELL__


void welcome(int sockfd){ 
  string wel = "****************************************\n" 
               "** Welcome to the information server. **\n" 
               "****************************************\n";
  write(sockfd, wel.c_str(), wel.size());                   
}               

void tochar(int n, char *arr){
  int cnt = 0;
  while(n > 0){
    arr[cnt++] = n % 10 + '0';
    n /= 10;
  }
  reverse(arr, arr + cnt);
  arr[cnt++] = '\0';
}

struct Usr_info{
  char name[20];
  char ip_port[30];
  int pid;
};

struct Msg_info{
  char msg[2005];
};

struct Client2client_info{
  int read[45], write[45];
  char msg[45][1050];
};

class Usr{
public:
  int shm_id, msg_id, c2c_id;
  key_t shm_key, msg_key, c2c_key;
  int mysockfd;
  int myid;
  int mypid;

  int create_shm(){
    shm_id = shmget(shm_key, sizeof(Usr_info) * 45, IPC_CREAT | 0600);
    msg_id = shmget(msg_key, sizeof(Msg_info) * 45, IPC_CREAT | 0600);
    c2c_id = shmget(c2c_key, sizeof(Client2client_info) * 45, IPC_CREAT | 0600);

    Usr_info *addr = (Usr_info *)shmat(shm_id, NULL, 0);
    for(int i = 0; i < 45; i++){
      addr[i].pid = 0;
    }
    shmdt(addr);
    
    Client2client_info *c2c = (Client2client_info *)shmat(c2c_id, NULL, 0);
    for(int i = 0; i < 45; i++){
      for(int j = 0; j < 45; j++){
        c2c[i].read[j] = 0;
        c2c[i].write[j] = 0;
      }
    }
    return shm_id;
  }

  int addusr(struct sockaddr_in u, int sockfd){
    Usr_info *addr = (Usr_info *)shmat(shm_id, NULL, 0);
    int cnt = 0;
//    char tmp[10];
    while(addr[++cnt].pid != 0);
    strcpy(addr[cnt].name, "(no name)\0");
/*
    strcpy(addr[cnt].ip_port, inet_ntoa(u.sin_addr));
    strcat(addr[cnt].ip_port, string("/").c_str());
    tochar(int(ntohs(u.sin_port)), tmp);
    strcat(addr[cnt].ip_port, tmp);
*/
    strcpy(addr[cnt].ip_port, "CGILAB/511");
    shmdt(addr);

    Client2client_info *c2c = (Client2client_info *)shmat(c2c_id, NULL, 0);
    for(int j = 0; j < 45; j++){
      c2c[cnt].write[j] = 1;
      c2c[cnt].read[j] = 0;
    }
    shmdt(c2c);

    myid = cnt;
    mysockfd = sockfd;
    return cnt;
  }

  void setpid(int idx, int pid){
    Usr_info *addr = (Usr_info *)shmat(shm_id, NULL, 0);
    addr[idx].pid = pid;
    mypid = pid;
    shmdt(addr);
  }

  void rmusr(int idx){
    Usr_info *addr = (Usr_info *)shmat(shm_id, NULL, 0);
    strcpy(addr[idx].name, "\0");
    strcpy(addr[idx].ip_port, "\0");
    addr[idx].pid = 0;
    shmdt(addr);

    Client2client_info *c2c = (Client2client_info *)shmat(c2c_id, NULL, 0);
    for(int j = 0; j < 45; j++){
      c2c[idx].write[j] = 0;
      c2c[idx].read[j] = 0;
    }
    shmdt(c2c);
  }

  string getname(int idx){
    char tmp[20];
    Usr_info *addr = (Usr_info *)shmat(shm_id, NULL, SHM_RDONLY);
    strcpy(tmp, addr[idx].name);
    shmdt(addr);
    return string(tmp);
  }

  void setname(int idx, string s){
    cout << idx << " set name as " << s << endl;
    Usr_info *addr = (Usr_info *)shmat(shm_id, NULL, 0);
    for(int i = 1; i < 45; i++){
      if(addr[i].pid == 0) continue;
      if(strcmp(addr[i].name, s.c_str()) == 0){
        write(mysockfd, "*** User '", 10);
        write(mysockfd, s.c_str(), s.size());
        write(mysockfd, "' already exists. ***\n", 22);
        shmdt(addr);
        return;
      }
    }
    strcpy(addr[idx].name, s.c_str());
    string tmp = string("*** User from ") + string("CGILAB/511");
    tmp += string(" is named '") + s + string("'. ***\n");

    Msg_info *bmsg = (Msg_info *)shmat(msg_id, NULL, 0);
    for(int i = 1; i < 45; i++){
      if(addr[i].pid == 0) continue;
      strcpy(bmsg[i].msg, tmp.c_str());
    }
    shmdt(bmsg);

    for(int i = 1; i < 45; i++){
      if(addr[i].pid == 0) continue;
      kill(addr[i].pid, SIGUSR1);
    }
    shmdt(addr);
  }

  string getip_port(int idx){
    char tmp[30];
    Usr_info *addr = (Usr_info *)shmat(shm_id, NULL, SHM_RDONLY);
    strcpy(tmp, addr[idx].ip_port);
    shmdt(addr);
    return string(tmp);
  }

  string tell_prefix(int idx, string s){
    Usr_info *addr = (Usr_info *)shmat(shm_id, NULL, SHM_RDONLY);
    string tmp = "*** ";
    tmp += string(addr[idx].name);
    tmp += " told you ***: ";
    tmp += s;
    tmp += "\n";
    return tmp;
  }

  string yell_prefix(int idx, string s){
    Usr_info *addr = (Usr_info *)shmat(shm_id, NULL, SHM_RDONLY);
    string tmp = "*** ";
    tmp += string(addr[idx].name);
    tmp += " yelled ***: ";
    tmp += s;
    tmp += "\n";
    return tmp;
  }

  void login_msg(int idx){
    string s = string("*** User '(no name)' entered from ") + getip_port(idx) + string(". ***\n");
    Usr_info *addr = (Usr_info *)shmat(shm_id, NULL, SHM_RDONLY);
    Msg_info *bmsg = (Msg_info *)shmat(msg_id, NULL, 0);
    for(int i = 1; i < 45; i++){
      if(addr[i].pid == 0) continue;
      strcpy(bmsg[i].msg, s.c_str());
    }
    shmdt(bmsg);

    for(int i = 1; i < 45; i++){
      if(addr[i].pid == 0) continue;
      kill(addr[i].pid, SIGUSR1);
    }
    shmdt(addr);
  }
  
  void logout_msg(int idx){
    string s = string("*** User '") + getname(idx) + string("' left. ***\n");
    Usr_info *addr = (Usr_info *)shmat(shm_id, NULL, SHM_RDONLY);
    Msg_info *bmsg = (Msg_info *)shmat(msg_id, NULL, 0);
    for(int i = 1; i < 45; i++){
      if(addr[i].pid == 0) continue;
      strcpy(bmsg[i].msg, s.c_str());
    }
    shmdt(bmsg);

    for(int i = 1; i < 45; i++){
      if(addr[i].pid == 0) continue;
      kill(addr[i].pid, SIGUSR1);
    }
    shmdt(addr);
  }

  bool tell(int myid, int idx, string s){
    cout << myid << " tell to " << idx << " " << s <<  endl;
    Usr_info *addr = (Usr_info *)shmat(shm_id, NULL, SHM_RDONLY);
    if(addr[idx].pid == 0){
      char tmp[10];
      tochar(idx, tmp);
      write(mysockfd, "*** Error: user #", 17);
      write(mysockfd, tmp, strlen(tmp));
      write(mysockfd, " does not exist yet. ***\n", 25);
      shmdt(addr);
      return 0;
    }
    Msg_info *bmsg = (Msg_info *)shmat(msg_id, NULL, 0);
    s = tell_prefix(myid, s);
    strcpy(bmsg[idx].msg, s.c_str());
    shmdt(bmsg);
    kill(addr[idx].pid, SIGUSR1);
    shmdt(addr);
    return 1;
  }

  void yell(int myid, string s){
    Usr_info *addr = (Usr_info *)shmat(shm_id, NULL, SHM_RDONLY);
    s = yell_prefix(myid, s);
    Msg_info *bmsg = (Msg_info *)shmat(msg_id, NULL, 0);
    for(int i = 1; i < 45; i++){
      if(addr[i].pid == 0) continue;
      strcpy(bmsg[i].msg, s.c_str());
    }
    shmdt(bmsg);
    for(int i = 1; i < 45; i++){
      if(addr[i].pid == 0) continue;
      kill(addr[i].pid, SIGUSR1);
    }
    shmdt(addr);
  }

  void who(int sockfd){
    Usr_info *addr = (Usr_info *)shmat(shm_id, NULL, SHM_RDONLY);
    write(sockfd, "<ID>\t<nickname>\t<IP/port>\t<indicate me>\n", 40);
    for(int i = 1; i < 45; i++){
      if(addr[i].pid == 0) continue;
      char tmp[1000];
      tochar(i, tmp);
      strcat(tmp, "\t");
      strcat(tmp, addr[i].name);
      strcat(tmp, "\t");
      strcat(tmp, addr[i].ip_port);
/*
      write(sockfd, tmp, strlen(tmp));
      write(sockfd, "\t", 1);
      write(sockfd, addr[i].name, strlen(addr[i].name));
      write(sockfd, "\t", 1);
      write(sockfd, addr[i].ip_port, strlen(addr[i].ip_port));
*/
      if(getpid() == addr[i].pid) {
        strcat(tmp, "\t<-me");
//        write(sockfd, "\t<-me", 6);
      }
      strcat(tmp, "\n");
      write(sockfd, tmp, strlen(tmp));
    }
    shmdt(addr);
  }
  
  void Wboardcast(string s, int idx){
    string output, strtmp;
    char chartmp[10];
    output = string("*** ") + getname(myid) + string(" (#");
    tochar(myid, chartmp);
    output += string(chartmp) + string(") just piped '") + s;
    output += string("' to ") + getname(idx) + string(" (#");
    tochar(idx, chartmp);
    output += string(chartmp) + string(") ***\n");

    Usr_info *addr = (Usr_info *)shmat(shm_id, NULL, SHM_RDONLY);
    Msg_info *bmsg = (Msg_info *)shmat(msg_id, NULL, 0);
    for(int i = 1; i < 45; i++){
      if(addr[i].pid == 0) continue;
      strcpy(bmsg[i].msg, output.c_str());
    }
    shmdt(bmsg);
    for(int i = 1; i < 45; i++){
      if(addr[i].pid == 0) continue;
      kill(addr[i].pid, SIGUSR1);
    }
    shmdt(addr);
  }

  void Rboardcast(string s, int idx){
    string output, strtmp;
    char chartmp[10];

    output = string("*** ") + getname(myid) + string(" (#");
    tochar(myid, chartmp);
    output += string(chartmp) + string(") just received from ");
    output += getname(idx) + string(" (#");
    tochar(idx, chartmp);
    output += string(chartmp) + string(") by '") + s + string("' ***\n");

    Usr_info *addr = (Usr_info *)shmat(shm_id, NULL, SHM_RDONLY);
    for(int i = 1; i < 45; i++){
      if(addr[i].pid == 0) continue;
      Msg_info *bmsg = (Msg_info *)shmat(msg_id, NULL, 0);
      strcpy(bmsg[i].msg, output.c_str());
      shmdt(bmsg);
    }
    for(int i = 1; i < 45; i++){
      if(addr[i].pid == 0) continue;
      kill(addr[i].pid, SIGUSR1);
    }
    shmdt(addr);
 
  }

  int read_cmd(int idx, string &s, string cmd){
/*
    Usr_info *check = (Usr_info *)shmat(shm_id, NULL, SHM_RDONLY);
    if(check[idx].pid == 0){
      char tmp[10];
      tochar(idx, tmp);
      string s = string("*** Error: user #");
      s += string(tmp) + string(" does not exist yet. ***\n");
      write(mysockfd, s.c_str(), s.size());
      shmdt(check);
      return 0;
    }
    shmdt(check);
*/
    Client2client_info *addr = (Client2client_info *)shmat(c2c_id, NULL, 0);
    if(addr[myid].read[idx] == 0){
      char tmp[10];
      write(mysockfd, "*** Error: the pipe #", 21);
      tochar(idx, tmp);
      write(mysockfd, tmp, strlen(tmp));
      write(mysockfd, "->#", 3);
      tochar(myid, tmp);
      write(mysockfd, tmp, strlen(tmp));
      write(mysockfd, " does not exist yet. ***\n", 25);
      shmdt(addr);
      return 0;
    }
    else{
      s += string(addr[myid].msg[idx]);

      cout << "XXXXXXXXXXXXXXXXXXXXX" << endl;
      cout << s << endl;
      cout << "XXXXXXXXXXXXXXXXXXXXX" << endl;

      strcpy(addr[myid].msg[idx], "\0");
      addr[myid].read[idx] = 0;
      addr[myid].write[idx] = 1;
      Rboardcast(cmd, idx); 
      shmdt(addr);
      return 1;
    }
  }

  int write_cmd(int idx, string &s, string cmd){
    Usr_info *check = (Usr_info *)shmat(shm_id, NULL, SHM_RDONLY);
    if(check[idx].pid == 0){
      char tmp[10];
      tochar(idx, tmp);
      string s = string("*** Error: user #");
      s += string(tmp) + string(" does not exist yet. ***\n");
      write(mysockfd, s.c_str(), s.size());
      shmdt(check);
      return 0;
    }
    shmdt(check);

    Client2client_info *addr = (Client2client_info *)shmat(c2c_id, NULL, 0);
    cout << idx << " : " << addr[idx].write << endl;  
    if(addr[idx].write[myid] == 0){
      char tmp[10];
      write(mysockfd, "*** Error: the pipe #", 21);
      tochar(myid, tmp);
      write(mysockfd, tmp, strlen(tmp));
      write(mysockfd, "->#", 3);
      tochar(idx, tmp);
      write(mysockfd, tmp, strlen(tmp));
      write(mysockfd, " already exists. ***\n", 21);
      shmdt(addr);
      return 0;
    }
    else{
      cout << "Write SHM : ";
      cout << s << endl;
      strcpy(addr[idx].msg[myid], s.c_str());
      s.clear();
      addr[idx].read[myid] = 1;
      addr[idx].write[myid] = 0;
      Wboardcast(cmd, idx);
      shmdt(addr);
      return 1;
    }
  }

};

struct Shell_info{
  int valid;
  int sockfd;
  int nowid;
  int ppnum;
  string buf;
  string raw;
  string cmd[MAXCMD][MAXCMDLINE];
  char **argvs[MAXCMD];
  char *argv[MAXCMD][MAXCMDLINE];
  vector<int> pp2;
  unordered_map<int, string> filein;
  unordered_map<int, string> fileout;
  unordered_map<int, string> catchbuf;


  unordered_map<int, int> clientin;
  unordered_map<int, int> clientout;


/* ---------------------------- Function ------------------------------ */
  
  void init(){
    valid = 1;
    buf.clear();
    for(int i = 0; i < MAXCMD; i++){
      argvs[i] = NULL;
      for(int j = 0; j < MAXCMDLINE; j++){
        cmd[i][j].clear();
        if(argv[i][j] != NULL) {
          free(argv[i][j]);
          argv[i][j] = NULL;
          cmd[i][j].clear();
        }
      }
    }
    pp2.clear();
    filein.clear();
    fileout.clear();

    clientin.clear();
    clientout.clear();
  }
  
  Shell_info(int sockfd = 0){
    valid = 1;
    this->sockfd = sockfd;
    nowid = 0;
    for(int i = 0; i < MAXCMD; i++){
      argvs[i] = NULL;
      for(int j = 0; j < MAXCMDLINE; j++){
        argv[i][j] = NULL;
      }
    }
    catchbuf.clear();
  }
};

class Parser{
private:

  bool isvalid(string &buf){
    for(int i = 0; i < int(buf.size()); i++){
      if(buf[i] == '/') return 0;
      if(i <= int(buf.size()) && buf[i] == '.' && buf[i+1] == '.') return 0;
    }
    return 1;
  }

  int ischatbox_cmd(string &s, Usr &usr){
    int cnt = 0;
    while(s[cnt] == ' ') cnt++;
    if(s.substr(cnt, cnt + 3) == string("who")){
      usr.who(usr.mysockfd);
      return 2;
    }
    if(s.substr(cnt, cnt + 4) == string("tell")) {
      int cnt1 = cnt + 4, num = 0; 
      while(s[cnt1] == ' ') cnt1++;
      while(s[cnt1] >= '0' && s[cnt1] <= '9'){
        num = num * 10 + s[cnt1] - '0';
        cnt1++;
      }
      cnt1++;
      usr.tell(usr.myid, num, s.substr(cnt1, int(s.size()) - cnt1 + 1));
      return 3; 
    }
    if(s.substr(cnt, cnt + 4) == string("yell")){ 
      int cnt1 = cnt + 5;
      usr.yell(usr.myid, s.substr(cnt1, int(s.size()) - cnt1 + 1));
      return 4; 
    }
    if(s.substr(cnt, cnt + 4) == string("name")){
      int cnt1 = cnt + 5;
      while(s[cnt1] == ' ') cnt1++;
      usr.setname(usr.myid, s.substr(cnt1, int(s.size()) - cnt1 + 1));
      return 5; 
    }
    if(s.substr(cnt, cnt + 4) == string("exit")){
      usr.logout_msg(usr.myid);
      usr.rmusr(usr.myid);
      close(usr.mysockfd);
      exit(0);
    }
    return 0;
  }

  int read_cmd(Shell_info &info, Usr &usr){
    info.init();

    char tmp[MAXBUF];

    while(1){
      int len = read(info.sockfd, tmp, sizeof(tmp));

      for(int i = 0; i < len; i++){
        if(tmp[i] != '\r' && tmp[i] != '\n')
          info.buf.push_back(tmp[i]);
      }

      if(tmp[len - 1] == '\n') break;
    }

    info.raw = info.buf;
    int flag = ischatbox_cmd(info.buf, usr); 
    if(flag) return 0;

    if(isvalid(info.buf) == 0){
      write(info.sockfd, "FUCK YOU!\n", 10);
      exit(0);
    }

    if(info.buf.size() == 0){
      info.valid = 0;
      return 0;
    }
    return 1;
  }

  void strip(string &buf){
    int first = -1, last = -1, cnt = 0;
    for(auto word : buf){
      if(word != ' ' && word != '\t'){
        if(first< 0) first = cnt;
        last = cnt;
      }
      cnt++;
    }
    if(first < 0 && last < 0) buf = string("");
    else{
      if(last + 1 < int(buf.size())) buf.erase(last + 1, int(buf.size()) + 1 - last);
      buf.erase(0, first);
    }
  }

  void pipe2(Shell_info &info){
    for(int i = 0; i < int(info.buf.size()); i++){
      if(info.buf[i] == '|'){
        int sum = 0;
        i++;
        while(i < int(info.buf.size()) && info.buf[i] >= '0' && info.buf[i] <= '9'){
          sum = sum * 10 + info.buf[i] - '0';
          info.buf[i++] = ' ';
        }
        info.pp2.push_back((sum == 0) ? 1 : sum);
      }
    }
  }

  void ClientIO(char op, string &buf, unordered_map<int, int> &unmap){
    stack<int> idx, del;
    for(int i = 0, cnt = 0; i < int(buf.size()); i++){
      if(buf[i] == '|') cnt++;
      else if(buf[i] == op && buf[i+1] >= '0' && buf[i+1] <= '9'){
        idx.push(i++);
        
        int num = 0;
        while(i < int(buf.size()) && buf[i] >= '0' && buf[i] <= '9'){
          num = num * 10 + buf[i] - '0';
          buf[i] = ' ';
          i++;
        }

        int sp = 0;
        while(i < int(buf.size()) && (buf[i] == ' ' || buf[i] == '\t')){
          sp++;
          i++;
        }

        unmap[cnt] = num;
        del.push(sp);
      }
    }

    while(!idx.empty()){
      int front = idx.top();
      int to = del.top() + 1;
      idx.pop(); 
      del.pop();
      buf.erase(front, to); 
    }
  }
  
  void fileIO(char op, string &buf, unordered_map<int, string> &unmap){
    stack<int> idx, del;
    for(int i = 0, cnt = 0; i < int(buf.size()); i++){
      if(buf[i] == '|') cnt++;
      else if(buf[i] == op){
        idx.push(i++);

        int sp = 0;
        while(i < int(buf.size()) && (buf[i] == ' ' || buf[i] == '\t')){
          sp++;
          i++;
        }

        string tmp;
        while(i < int(buf.size()) && buf[i] != ' '){
          tmp.push_back(buf[i++]);
          sp++;
        }
        unmap[cnt] = tmp;
        del.push(sp);
      }
    }

    while(!idx.empty()){
      int front = idx.top();
      int to = del.top() + 1;
      idx.pop(); 
      del.pop();
      buf.erase(front, to); 
    }
  }

  void parse(Shell_info &info){
    for(int i = 0, j = 0, k = 0; i < int(info.buf.size()); ){
      if(info.buf[i] == '|') {
        i++;
        j++;
        k = 0;
        while(i < int(info.buf.size()) && info.buf[i] == ' ') i++;
      }
      else if(i < int(info.buf.size()) && info.buf[i] == ' '){
        i++;
        k++;
      }
      else if(i < int(info.buf.size())) {
        info.cmd[j][k].push_back(info.buf[i++]);
      }
    }

    int cnt = 0;
    for(int i = 0; int(info.cmd[i][0].size()) > 0; i++){
      cnt++;
      for(int j = 0; int(info.cmd[i][j].size()) > 0; j++){
        info.argv[i][j] = strdup(info.cmd[i][j].c_str());
      }
      info.argvs[i] = info.argv[i];
    }
            
    info.ppnum = cnt;
    while(int(info.pp2.size()) < cnt) info.pp2.push_back(0);
  }

public:
  void run(Shell_info &info, Usr &usr){
    int flag = read_cmd(info, usr);
    if(flag == 0) {
      info.valid = 0;
      return;  
    }
    else{
      strip(info.buf);
      pipe2(info);
      ClientIO('>', info.buf, info.clientout);
      ClientIO('<', info.buf, info.clientin);

      cout << "Client : \n";
      for(auto i : info.clientout) cout << i.first << " " << i.second << endl;
      cout << endl;

      fileIO('>', info.buf, info.fileout);
      fileIO('<', info.buf, info.filein);

      cout << "File : \n";
      for(auto i : info.fileout) cout << i.first << " " << i.second << endl;
      cout << endl;


      parse(info);
    }
  }
};

class Process{
public:
  int Ifd[2], Ofd[2], errfd[2], unfd[2];

  void printerror(Shell_info &info, int idx){
    write(info.sockfd, "Unknown command: [", 18);
    write(info.sockfd, info.argv[idx][0], strlen(info.argvs[idx][0]));
/*
    for(int j = 0; info.argvs[idx][j]; j++){
      if(j == 0){
        write(info.sockfd, info.argvs[idx][j], strlen(info.argvs[idx][j]));
      }

      else{
        write(info.sockfd, " ", 1);
        write(info.sockfd, info.argvs[idx][j], strlen(info.argvs[idx][j]));
      }
    }
*/
    write(info.sockfd, "].\n", 3);
    if(idx != 0) info.nowid--;
  }
  
  void call_exit(int sockfd){
    cout << "[Debug]" << sockfd << " Logout!" << endl;
    close(sockfd);
    exit(0);
  }
  
  void call_setenv(Shell_info &info, int idx){
    int cnt = 1;
    while(info.argvs[idx][cnt]) cnt++;
    if(cnt == 3) {
      setenv(info.argvs[idx][1], info.argvs[idx][2], 1);
      cout << "[Debug]Setenv Success!" << endl;
      return;
    }
    else printerror(info, idx);
    cout << "[Debug]Setenv Fail!" << endl;
  }

  void call_getenv(Shell_info &info, int idx){
    int cnt = 1;
    while(info.argvs[idx][cnt]) cnt++;
      if(cnt == 2){
        char *msg;
        msg = getenv(info.argvs[idx][1]);
        write(info.sockfd, info.argvs[idx][1], strlen(info.argvs[idx][1]));
        write(info.sockfd, "=", 1);
        write(info.sockfd, msg, strlen(msg));
        write(info.sockfd, "\n", 1);
        cout << "[Debug]Getenv Success!" << endl;
      }
      else{
        printerror(info, idx);
        cout << "[Debug]Getenv Fail!" << endl;
      }
  }

  bool call_special_function(Shell_info &info, int idx){
    bool flag = 1;
//    if(strcmp(info.argvs[idx][0], "exit") == 0) call_exit(info.sockfd);
    if(strcmp(info.argvs[idx][0], "setenv") == 0) call_setenv(info, idx);
    else if(strcmp(info.argvs[idx][0], "printenv") == 0) call_getenv(info, idx);
    else flag = 0;

    return flag;
  }

  void read_inputfile(Shell_info &info, int idx){
    cout << "[Debug]Read File!" << endl;
    string tmp;
    ifstream fin;
    fin.open(info.filein[idx]);
    while(getline(fin, tmp)){
      info.catchbuf[info.nowid] += (tmp + "\n");
    }
    fin.close();
  }

  void write_outputfile(Shell_info &info, int idx){
    cout << "[Debug]Write File " << info.fileout[idx] << endl;
    ofstream fout;
    fout.open(info.fileout[idx]);
    fout << info.catchbuf[info.nowid];
    info.catchbuf.erase(info.nowid);
    fout.close();
  }

  void socket_output(Shell_info &info){
    write(info.sockfd, info.catchbuf[info.nowid].c_str(), info.catchbuf[info.nowid].size());
    info.catchbuf.erase(info.nowid);
  }

  void four_pipe(){
    if(pipe(Ifd) < 0){
      cout << "[Debug]Pipe I Create Fail!" << endl;
      exit(0);
    }
    if(pipe(Ofd) < 0){
      cout << "[Debug]Pipe O Create Fail!" << endl;
      exit(0);
    }
    if(pipe(errfd) < 0){
      cout << "[Debug]Pipe Err Create Fail!" << endl;
      exit(0);
    }
    if(pipe(unfd) < 0){
      cout << "[Debug]Pipe Unkown Create Fail!" << endl;
      exit(0);
    }

  }
  
  void create_process(Shell_info &info, int idx){
    pid_t pid = fork();

    if(pid < 0){
      cout << "[Debug]Fork Fail!" << endl;
      exit(0);
    }
    else if(pid == 0){
      dup2(Ifd[0], 0);
      close(Ifd[0]);
      close(Ifd[1]);

      dup2(Ofd[1], 1);
      close(Ofd[1]);
      close(Ofd[0]);

      dup2(errfd[1], 2);
      close(errfd[1]);
      close(errfd[0]);
      
      if(execvp(info.argvs[idx][0], info.argvs[idx]) < 0){
        printerror(info, idx);

        write(unfd[1], "Error", 5);
        close(unfd[0]);
        close(unfd[1]);
        
        exit(-1);
      }
    }
    else{        
      char tmp[MAXBUF];
      int len;

      if(info.catchbuf.find(info.nowid) != info.catchbuf.end()){

        cout << "RRRRRRRRRRRRRRRRRRRRRR" << endl;
        cout << info.catchbuf[info.nowid] << endl;;
        cout << "RRRRRRRRRRRRRRRRRRRRRR" << endl;

        write(Ifd[1], info.catchbuf[info.nowid].c_str(), info.catchbuf[info.nowid].size());
      }
      close(Ifd[1]);
      close(Ifd[0]);
      close(Ofd[1]);
      close(errfd[1]);
      close(unfd[1]);

      wait(NULL);

      while(1){
        len = read(errfd[0], tmp, sizeof(tmp));
        if(len > 0) write(info.sockfd, tmp, len);
        else break;
      }
      close(errfd[0]);

      len = read(unfd[0], tmp, sizeof(tmp));
      if(len > 0){
        cout << "[Debug]Invalid Command!" << endl;
        if(idx == 0) info.nowid++;
        info.valid = 0;
      }
      else{
        if(info.catchbuf.find(info.nowid) != info.catchbuf.end()) info.catchbuf.erase(info.nowid);
        while(1){
          len = read(Ofd[0], tmp, sizeof(tmp));
          if(len > 0) info.catchbuf[info.nowid + info.pp2[idx]] += string(tmp, 0, len);
          else break;
        }
      }

      close(unfd[0]);
      close(Ofd[0]); 
    }
  }

  void create_pipe(Shell_info &info, Usr &usr){
    for(int i = 0; i < info.ppnum; i++){
      cout << "[Debug]Process " << info.nowid << "->" << info.nowid + info.pp2[i] << endl;
      cout << "RAW : " << info.raw << endl << endl;
      cout << "AFTER : " << info.buf << endl << endl;
      
      if(call_special_function(info, i)) continue;
      
      if(info.clientin.find(i) != info.clientin.end()){
        int flag = usr.read_cmd(info.clientin[i], info.catchbuf[info.nowid], info.raw);
        cout << "OOOOOOOOOOOOOOOOOOO" << endl;
        cout << info.catchbuf[info.nowid] << endl;
        cout << "OOOOOOOOOOOOOOOOOOO" << endl;
        cout << flag << endl;

        if(flag == 0) return;
      }

      if(info.filein.find(i) != info.filein.end()) read_inputfile(info, i);

      if(info.fileout.find(i) != info.fileout.end()){
        ofstream fout;
        fout.open(info.fileout[i]);
        fout.close();
      }

      four_pipe();       
      
      create_process(info, i);

      if(info.valid == 0) return;

      if(info.clientout.find(i) != info.clientout.end()){
        cout << "Process client out : " << endl;
        int flag = usr.write_cmd(info.clientout[i], info.catchbuf[info.nowid], info.raw);
        info.catchbuf.erase(info.nowid);
        if(flag == 0) return;
      }

      if(info.fileout.find(i) != info.fileout.end()) write_outputfile(info, i);

      if(info.pp2[i] == 0 && info.catchbuf.find(info.nowid) != info.catchbuf.end()) socket_output(info);

      info.nowid++;
    }
    cout << endl;
  }

  void run(Shell_info &info, Usr &usr){
    if(info.valid == 0) {
      return;
    }
    create_pipe(info, usr);     
  }

};

class Login{
public:
  Shell_info info;

  void envsetting(){
    chdir("/net/cs/104/0416042/rwg");
    clearenv();
    setenv("PATH", "bin:.", 1);
  }

  void prompt(int sockfd){
    string prom = "% ";
    while(1){
      int len = write(sockfd, prom.c_str(), prom.size());
      if(len == 2) break; 
    }
  }

  void run(Usr &usr){
    envsetting();
 
    Parser parser;
    Process process;
 
    while(1){
      prompt(info.sockfd);
      parser.run(info, usr);
      process.run(info, usr);
    }

  }

  Login(int sockfd){
    info.sockfd = sockfd;
    cout << "[Debug]" << sockfd << " login!" << endl;
  }
};
#endif
