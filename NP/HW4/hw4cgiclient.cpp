#include <iostream>
#include <sstream>
#include <cstdio>
#include <cstdlib>
#include "server.h"
#include "error.h"
#include <sys/select.h>
#include <netdb.h>

using namespace std;

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
  string socksip;
  string socksport;
  string filename = "/u/cs/104/0416042/public_html/";
};

void sethosts(Remote_host *, FDS &);
void connectsock(Remote_host &, FDS &);
void sendmsg(Remote_host &, FDS &, int);
void recvmsg(Remote_host &, int);
void preprint(Remote_host *);
void print(string, int);
void posprint();


int main(){
  
  Remote_host host[6];
  FDS fds;
  sethosts(host, fds);
  preprint(host);
  int flag = 5;
  for(int i = 1; i < 6; i++){
    if(host[i].sockfd == 0){
      flag--;
      continue;
    }
    connectsock(host[i], fds);
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
  posprint();
}

void sethosts(Remote_host host[], FDS &fds){
  int flag[6];
  char *msg = strdup(getenv("QUERY_STRING")), *tok[6][5];

  tok[1][0] = strtok(msg, "&");
  tok[1][1] = strtok(NULL, "&");
  tok[1][2] = strtok(NULL, "&");
  tok[1][3] = strtok(NULL, "&");
  tok[1][4] = strtok(NULL, "&");
  flag[1] = 0;
  for(int i = 2; i < 6; i++){
    tok[i][0] = strtok(NULL, "&");
    tok[i][1] = strtok(NULL, "&");
    tok[i][2] = strtok(NULL, "&");
    tok[i][3] = strtok(NULL, "&");
    tok[i][4] = strtok(NULL, "&");
    flag[i] = 0;
  }

  for(int i = 1; i < 6; i++){
    memset((char *) &host[i].addr, 0, sizeof(host[i].addr));
    if(strlen(tok[i][0]) > 3){ 
      host[i].ip = string(tok[i][0]).erase(0, 3);
      /*
      host[i].addr.sin_family = AF_INET;
      host[i].addr.sin_addr.s_addr = inet_addr(host[i].ip.c_str());
      */
      flag[i]++;
      cerr << host[i].ip.c_str() << endl;
    }
    if(strlen(tok[i][1]) > 3){
      host[i].port = string(tok[i][1]).erase(0, 3);
      /*
      host[i].addr.sin_port = htons(atoi(host[i].port.c_str()));
      */
      flag[i]++;
      cerr << host[i].port.c_str() << endl;
    }
    if(strlen(tok[i][2]) > 3) {
      host[i].filename += string(tok[i][2]).erase(0, 3);
      cerr << host[i].filename << endl;
      host[i].fin = fopen(host[i].filename.c_str(), "r");
      if(host[i].fin == NULL) continue;
      FD_SET (fileno (host[i].fin), &fds.afds);
      fds.nfds++; 
      flag[i]++;
    }
    if(strlen(tok[i][3]) > 4){
      host[i].socksip = string(tok[i][3]).erase(0, 4);
      host[i].addr.sin_family = AF_INET;
      host[i].addr.sin_addr.s_addr = inet_addr(host[i].socksip.c_str());
      flag[i]++;
      cerr << host[i].socksip << endl;
    }
    if(strlen(tok[i][4]) > 4){
      host[i].socksport = string(tok[i][4]).erase(0, 4);
      host[i].addr.sin_port = htons(atoi(host[i].socksport.c_str()));
      flag[i]++;
      cerr << host[i].socksport.c_str() << endl;
    }
    if(flag[i] != 5) host[i].isconnect = -1;
    else {
      host[i].sockfd = socket(AF_INET, SOCK_STREAM, 0);
    }
  }
  
  free(msg);
}

void connectsock(Remote_host &host, FDS &fds){
  
  if(host.sockfd > 0 && host.isconnect == 0){
/*
    struct hostent *he = gethostbyname(host.ip.c_str());
    struct in_addr** addr_list = (struct in_addr **)he->h_addr_list;
    inet_pton(AF_INET, inet_ntoa(*addr_list[0]), &host.addr.sin_addr.s_addr);
    int flag = fcntl(host.sockfd, F_GETFL, 0);
    fcntl(host.sockfd, F_SETFL, flag | O_NONBLOCK);
*/
    if(connect(host.sockfd, (struct sockaddr *) &(host.addr), sizeof(host.addr)) < 0){
      if (errno != EINPROGRESS && errno != EALREADY) {
        Error::error("connect fail");
        return;
      }
    }
    cerr << "Success Connected" << endl;

    /* 
    char *fw = inet_ntoa((in_addr) host.addr.sin_addr);
    cerr << string(fw).substr(0, 11) << endl;
    if(string(fw).substr(0, 11) == string("140.113.167")){
      cerr << "BAN" << endl;
      cout << "Connection denied" << endl;
      close(host.sockfd);
      return;
    }
    */

    unsigned char prepkg[15];
    int cnt = 0, sum = 0;
    prepkg[cnt++] = (unsigned char)(unsigned int)4;
    prepkg[cnt++] = (unsigned char)(unsigned int)1;
    prepkg[cnt++] = (unsigned char)(unsigned int)(atoi(host.port.c_str()) / 256);
    prepkg[cnt++] = (unsigned char)(unsigned int)(atoi(host.port.c_str()) % 256);
    
    for(int i = 0; i < host.ip.size(); i++){
      if(host.ip[i] == '.'){
        prepkg[cnt++] = (unsigned char)(unsigned int)sum;
        sum = 0;
      }
      else{
        sum = sum * 10 + host.ip[i] - '0';
      }
    }
    prepkg[cnt++] = (unsigned char)(unsigned int)sum;
    prepkg[cnt++] = '\0';
    
    cerr << "[PKG] " << hex;
    for(int i = 0; i < cnt; i++)
      cerr << (unsigned int)prepkg[i] << " ";
    cerr << dec << endl;
   
    write(host.sockfd, prepkg, cnt);
    read(host.sockfd, prepkg, 8);

    FD_SET(host.sockfd, &fds.afds);
    host.isconnect++;
    fds.nfds++; 
  }
  cerr << "[Connect Finish]" << endl;
}

void sendmsg(Remote_host &host, FDS &fds, int idx){
  char tmp[10005];
  string msg;
  fgets(tmp, sizeof(tmp), host.fin);
  msg = string(tmp);
  cerr << msg;

  print(msg, idx);
  write(host.sockfd, msg.c_str(), msg.size());
  host.isconnect = 1;

  if(msg.substr(0, 4) == "exit"){
    close(host.sockfd);
    FD_CLR(host.sockfd, &fds.afds);
    FD_CLR(fileno(host.fin), &fds.afds);
    memset((char *) &host.addr, 0, sizeof(host.addr));
    fclose(host.fin);
    host.ip.clear();
    host.port.clear();
    host.filename.clear();
    host.isconnect = 0;
    host.sockfd = 0;
  }
}

void recvmsg(Remote_host &host, int idx){
  string msg;
  char tmp[10005];
  int len = read(host.sockfd, tmp, sizeof(tmp));
  tmp[len] = 0;
  msg = string(tmp);
  if(msg.size() == 0) return;
//  cerr << msg;
//  fflush(stderr);
  print(msg, idx);
  if(msg[int(msg.size()) - 2] == '%' && msg[int(msg.size()) - 1] == ' ') host.isconnect = 2;
}

void preprint(Remote_host *host){
  string msg; 
  
  msg += "Content-Type: text/html\n\n";
  msg += "<html>\n";
  msg += "<head>\n";
  msg += "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\" />\n";
  msg += "</head>\n";
  msg += "<body bgcolor=#336699>\n";
  msg += "<font face=\"Courier New\" size=2 color=#FFFF99>\n";
  msg += "<table width=\"800\" border=\"1\">\n";
  msg += "<tr>\n";
  for(int i = 1; i < 6; i++){
    msg += "<td>";
    if(host[i].ip != "") msg += host[i].ip;
    msg += "</td>\n";
  }
  msg += "</tr>\n";
  msg += "<tr>\n";
  for(int i = 1; i < 6; i ++){
    stringstream ss;
    string tmp;
    ss << i << endl;
    ss >> tmp;
    msg += "<td valign=\"top\" id=\"m" + tmp + "\"></td>\n";
  }
  msg += "</tr>\n";
  msg += "</table>\n";
  cout << msg;
  fflush(stdout);
}

void print(string msg, int idx){
  stringstream ss;
  string num;
  ss << idx;
  ss >> num;
  cout << "<script>document.all['m" + num + "'].innerHTML += \"";
  for(int i = 0; i < int(msg.size()); i++){
    if(msg[i] == '\"') cout << "&quot;";
    else if(msg[i] == '\'') cout << "&apos;";
    else if(msg[i] == '&') cout << "&amp;";
    else if(msg[i] == '<') cout << "&lt;";
    else if(msg[i] == '>') cout << "&gt;";
    else if(msg[i] == '\n') cout << "<br>";
    else if(msg[i] == '\r') cout << "";
    else cout << msg[i];
  }
  cout << "\";</script>\n";
  fflush(stdout);
}

void posprint(){
  cout << "</font>\n";
  cout << "</body>\n";
  cout << "</html>\r\n";
  cout <<"\r\n";
  fflush(stdout);
}
