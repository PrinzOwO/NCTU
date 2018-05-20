#include "cgi.h"

void Cgi::sethosts(Cgi::Remote_host host[], Cgi::FDS &fds){
  int flag[6];
  char *msg = strdup(getenv("QUERY_STRING")), *tok[6][3];

  tok[1][0] = strtok(msg, "&");
  tok[1][1] = strtok(NULL, "&");
  tok[1][2] = strtok(NULL, "&");
  flag[1] = 0;
  for(int i = 2; i < 6; i++){
    tok[i][0] = strtok(NULL, "&");
    tok[i][1] = strtok(NULL, "&");
    tok[i][2] = strtok(NULL, "&");
    flag[i] = 0;
  }

  for(int i = 1; i < 6; i++){
    memset((char *) &host[i].addr, 0, sizeof(host[i].addr));
    if(strlen(tok[i][0]) > 3){ 
      host[i].ip = string(tok[i][0]).erase(0, 3);
      host[i].addr.sin_family = AF_INET;
      host[i].addr.sin_addr.s_addr = inet_addr(host[i].ip.c_str());
      flag[i]++;
      cerr << host[i].ip.c_str() << endl;
    }
    if(strlen(tok[i][1]) > 3){
      host[i].port = string(tok[i][1]).erase(0, 3);
      host[i].addr.sin_port = htons(atoi(host[i].port.c_str()));
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
    if(flag[i] != 3) host[i].isconnect = -1;
    else {
      host[i].sockfd = socket(AF_INET, SOCK_STREAM, 0);
    }
  }
  
  free(msg);
}

void Cgi::connectsock(Cgi::Remote_host &host, Cgi::FDS &fds){
  
  if(host.sockfd > 0 && host.isconnect == 0){

    struct hostent *he = gethostbyname(host.ip.c_str());
    struct in_addr** addr_list = (struct in_addr **)he->h_addr_list;
    inet_pton(AF_INET, inet_ntoa(*addr_list[0]), &host.addr.sin_addr.s_addr);
    int flag = fcntl(host.sockfd, F_GETFL, 0);
    fcntl(host.sockfd, F_SETFL, flag | O_NONBLOCK);

    if(connect(host.sockfd, (struct sockaddr *) &(host.addr), sizeof(host.addr)) < 0){
      if (errno != EINPROGRESS && errno != EALREADY) {
        Error::error("connect fail");
        return;
      }
    }
    cerr << "Success Connected" << endl;
    FD_SET(host.sockfd, &fds.afds);
    host.isconnect++;
    fds.nfds++; 
  }
}

void Cgi::sendmsg(Cgi::Remote_host &host, Cgi::FDS &fds, int idx){
  char tmp[10005];
  string msg;
  fgets(tmp, sizeof(tmp), host.fin);
  msg = string(tmp);
//  cerr << msg;

  Cgi::print(msg, idx);
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

void Cgi::recvmsg(Cgi::Remote_host &host, int idx){
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

void Cgi::preprint(Cgi::Remote_host *host){
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

void Cgi::print(string msg, int idx){
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

void Cgi::posprint(){
  cout << "</font>\n";
  cout << "</body>\n";
  cout << "</html>\r\n";
  cout <<"\r\n";
  fflush(stdout);
}
