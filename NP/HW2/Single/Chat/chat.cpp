#include "chat.h"

int Chat::Table::add(int sockfd, string IP, int port){
  int cnt = 1;
  for(auto &obj : this->table) {
    if(obj.first != cnt) break;
    cnt++;
  }
//  this->table[cnt] = Chat::Info("(no name)", IP, port, sockfd);
  this->table[cnt] = Chat::Info("(no name)", "CGILAB", 511, sockfd);
  this->fd2id[sockfd] = cnt;

  string msg = "*** User '" + table[cnt].name + "' entered from " + table[cnt].IP + "/" + to_string(table[cnt].port) + ". ***\n";
  for(auto &obj : this->table)
    write(obj.second.sockfd, msg.c_str(), msg.size());
  
  return cnt;
}

void Chat::Table::del(int sockfd){
  int id = this->fd2id[sockfd];
  string msg = "*** User '" + table[id].name + "' left. ***\n";
  for(auto &obj : this->table)
    write(obj.second.sockfd, msg.c_str(), msg.size());
  
  for(int i = 0; i < 50; i++) this->mailbox[sockfd][i].clear();
  this->table.erase(id);
  this->fd2id.erase(sockfd);
}

void Chat::Table::show(int sockfd){
  string msg = "<ID>\t<nickname>\t<IP/port>\t<indicate me>\n";
  for(auto &usr : this->table){
    msg += to_string(usr.first) + "\t";
    msg += usr.second.name + "\t";
    msg += usr.second.IP + "/" + to_string(usr.second.port);
    if(sockfd == usr.second.sockfd) msg += "\t<-me";
    msg += "\n";
  }
  write(sockfd, msg.c_str(), msg.size());
}

void Chat::Table::name(int sockfd, string name){
  while(name.size() && (name.back() == '\n' || name.back() == '\r')) name.pop_back();

  int flag = 1;
  for(auto &usr : this->table)
    if(usr.second.name == name) flag = 0;

  if(flag == 0){
    string msg = "*** User '" + name + "' already exists. ***\n";
    write(sockfd, msg.c_str(), msg.size());
  }
  else{
    int id = this->fd2id[sockfd];
    this->table[id].name = name;

    string msg = "*** User from " + this->table[id].IP + "/" + to_string(this->table[id].port) + " is named '" + name + "'. ***\n";
    for(auto &usr : this->table)
      write(usr.second.sockfd, msg.c_str(), msg.size());
  }
}

void Chat::Table::yell(int sockfd, string msg){
  while(msg.size() && (msg.back() == '\n' || msg.back() == '\r')) msg.pop_back();

  int id = this->fd2id[sockfd];

  string tmp = "*** " + this->table[id].name + " yelled ***: " + msg + "\n";

  for(auto &usr : this->table)
    write(usr.second.sockfd, tmp.c_str(), tmp.size());
}

void Chat::Table::tell(int sockfd, string msg){
  while(msg.size() && (msg.back() == '\n' || msg.back() == '\r')) msg.pop_back();

  int cnt = 0;
  while(msg[cnt++] != ' ');
  
  int sendid = this->fd2id[sockfd];
//  int recvid = atoi(msg.substr(0, cnt).c_str());
  
  cout << msg.substr(0, cnt) << endl;

  int recvid;
  if(msg[0] >= '0' && msg[0] <= '9')
    recvid = atoi(msg.substr(0, cnt).c_str());
  else{
    for(auto &usr : this->table){
      if(msg.substr(0, cnt - 1) == usr.second.name){
        recvid = usr.first;
        break;
      }
      recvid = 0;
    }
  }
  
  msg.erase(0, cnt);
  
  if(this->table.find(recvid) == this->table.end()) {
    string tmp = "*** Error: user #" + to_string(recvid) + " does not exist yet. ***\n";
    write(sockfd, tmp.c_str(), tmp.size());
  }
  else{
    string tmp = "*** " + this->table[sendid].name + " told you ***: " + msg + "\n";
    write(this->table[recvid].sockfd, tmp.c_str(), tmp.size());
  }
}

int Chat::Table::sendmsg(int ssockfd, int did, string msg, string cmd){
  int sid = this->fd2id[ssockfd];
  if(this->table.find(did) == this->table.end()){
    string tmp = "*** Error: user #" + to_string(did);
    tmp += " does not exist yet. ***\n";
    write(ssockfd, tmp.c_str(), tmp.size());
    return 0;
  }

  int dsockfd = this->table[did].sockfd;
  if(!this->mailbox[dsockfd][ssockfd].empty()){
    string tmp = "*** Error: the pipe #" + to_string(sid);
    tmp += "->#" + to_string(did) + " already exists. ***\n";
    write(ssockfd, tmp.c_str(), tmp.size());
    return 0;
  }
  else{
    string tmp = "*** ";
    tmp += this->table[sid].name + " (#" + to_string(sid);
    tmp += ") just piped '" + cmd + "' to ";
    tmp += this->table[did].name + " (#" + to_string(did) + ") ***\n";
    for(auto &usr : this->table) 
      write(usr.second.sockfd, tmp.c_str(), tmp.size());
    this->mailbox[dsockfd][ssockfd] = msg;
    return 1;
  }
}

string Chat::Table::recvmsg(int ssockfd, int did, string cmd){
  int sid = this->fd2id[ssockfd];
  if(this->table.find(did) == this->table.end()){
    string tmp = "*** Error: the pipe #" + to_string(did);
    tmp += "->#" + to_string(sid) + " does not exist yet. ***\n";
    write(ssockfd, tmp.c_str(), tmp.size());
    return "";
  }
  int dsockfd = this->table[did].sockfd;
  if(this->mailbox[ssockfd][dsockfd].empty()){
    string tmp = "*** Error: the pipe #" + to_string(did);
    tmp += "->#" + to_string(sid) + " does not exist yet. ***\n";
    write(ssockfd, tmp.c_str(), tmp.size());
    return "";  
  }
  else{
    string tmp = "*** ";
    tmp += this->table[sid].name + " (#" + to_string(sid);
    tmp += ") just received from " + this->table[did].name + " (#" + to_string(did); 
    tmp += ") by '" + cmd + "' ***\n";
    for(auto &usr : this->table)
      write(usr.second.sockfd, tmp.c_str(), tmp.size());
    string msg = this->mailbox[ssockfd][dsockfd];
    this->mailbox[ssockfd][dsockfd].clear();
    return msg; 
  }
}

