#include "socks4.h"

string Socks4::getIP(string IP){
  string ip;
  string tmp[4];
  int cnt = 0, part[4];
  IP += ".";
  for(auto c : IP){
    if(c == '.') {
      if(tmp[cnt] == "*") part[cnt] = 0;
      else part[cnt] = atoi(tmp[cnt].c_str());
      cnt++;
    }
    else tmp[cnt].push_back(c);
  }

  ip.resize(32);
  for(int i = 0; i < 4; i++){
    for(int j = 7; j >= 0; j--){
      ip[i * 8 + j] = (part[i] & 1 ? '1' : '0');
      part[i] >>= 1;
    }
  }
  return ip;
}

void Socks4::Rule::setIpMask(string IP){

  this->IP = Socks4::getIP(IP);
  for(auto c : IP)
    if(c == '*') this->mask -= 8;
}

void Socks4::PF::setPF(){
  ifstream fin("socks.conf");
  string tmp;
  while(getline(fin, tmp)){
    stringstream ss;
    Socks4::Rule newrule;
    ss << tmp << endl;
    
    ss >> tmp;
    if(tmp == "permit") newrule.pass = 1;
    ss >> tmp;
    if(tmp == "b") newrule.mode = 1;
    ss >> tmp;
    newrule.setIpMask(tmp);

    this->rule.push_back(newrule);
  }
}

bool Socks4::PF::fit(string IP, int mode){
  int pass = 0;
  string comeip = (IP.size() == 32 ? IP : Socks4::getIP(IP));  
  for(auto r : this->rule){
    int flag = 1;
    if(mode != r.mode) continue;
    for(int i = 0; i < r.mask && flag; i++) 
      if(comeip[i] != r.IP[i]) flag = 0;
    if(flag){
      if(r.pass) pass = 1;
      else pass = 0;
      break;
    }
  }
  return pass;
}

Socks4::Request::Request(uchar *buf = NULL, 
                         string user = "", 
                         string srcIP = "", 
                         uint srcPort = 0, 
                         bool AC = 0){

  if(buf == NULL) return;
  
  cerr << "[CD] " << (int)buf[1] << endl;

  this->vn = buf[0];
  this->cd = buf[1];
  this->destPort = (((uint)(uchar)buf[2]) << 8) | (uint)(uchar)buf[3];
  this->setDestIP(buf[4], buf[5], buf[6], buf[7]);
  this->user = user;
  
  this->srcPort = (uint)srcPort;
  this->srcIP = srcIP;
  this->AC = AC;
}

void Socks4::Request::show(){
  cerr << endl;
  cerr << "<S_IP>    :" << this->srcIP << endl;
  cerr << "<S_PORT>  :" << this->srcPort << endl;
  cerr << "<D_IP>    :" << this->destIP << endl;
  cerr << "<D_PORT>  :" << this->destPort << endl;
  cerr << "<Command> :" << (this->cd==1?"CONNECT":(this->cd==2?"BIND":"OTHER")) << endl;
  cerr << "<Reply>   :" << (this->AC ? "Accept" : "Reject") << endl;
}

string Socks4::Request::setDestIP(char buf0, char buf1, char buf2, char buf3){
  string tmp;
  stringstream ss;
  ss << (int)(uchar)buf0 << ".";
  ss << (int)(uchar)buf1 << ".";
  ss << (int)(uchar)buf2 << ".";
  ss << (int)(uchar)buf3 << endl;
  ss >> tmp;
  this->destIP = tmp;
  return tmp;
}

void Socks4::Reply::sendreply(){
  int cnt = 0;
  uchar pkg[10];
  pkg[cnt++] = this->vn;
  pkg[cnt++] = this->cd;
  pkg[cnt++] = (uchar)(this->destPort / 256);
  pkg[cnt++] = (uchar)(this->destPort % 256);

  uint sum = 0;
  for(int i = 0; i < (int)this->destIP.size(); i++){
    if(this->destIP[i] == '.'){
      pkg[cnt++] = (uchar)sum;
      sum = 0;
    }
    else{
      sum = sum * 10 + this->destIP[i] - '0';
    }
  }
  pkg[cnt++] = (uchar)sum;
  pkg[cnt++] = 0;
  write(1, pkg, 8);
  
  cerr << hex << "[TAT] [";
  for(int i = 0; i < 8; i++)
    cerr << (uint)pkg[i] << " ";
  cerr << dec << "]" << endl;

  fflush(stdout);
}

void Socks4::sockschild(struct sockaddr_in cli_addr){
  Socks4::PF pf;
  pf.setPF();
  usleep(100);
  uchar buf[MAXLEN], c;
  string tmp;
  int len = read(0, buf, 8);
  buf[len] = 0;
  
  cerr << "[LEN] " << len << endl;

  while(read(0, &c, 1) && c != 0);

  cerr << "[Finish NULL] " << endl;

  char *srcIP = inet_ntoa((in_addr) cli_addr.sin_addr);
  uint srcPort = ((struct sockaddr_in)cli_addr).sin_port;
  Socks4::Request request = Socks4::Request(buf, tmp, srcIP, srcPort, 0);

  request.AC = pf.fit(request.destIP, request.cd == 2);
  request.show(); 
  Socks4::Reply reply = Socks4::Reply();
  if(request.AC) {
    reply.cd = 90;
    reply.destIP = request.destIP;
    reply.destPort = request.destPort;
  }

  if(!request.AC) exit(0);
  fflush(stdout);
  int sockfd;
  if(request.cd == 1) sockfd = connect(request);
  else sockfd = bind(reply);
  fflush(stdout);
  reply.sendreply();
  Socks4::exchange(sockfd);
}

int Socks4::connect(Request &req){
  cerr << "[CONNECT]" << endl;
  int sockfd;
  struct sockaddr_in serv_addr;

  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(req.destPort);
  serv_addr.sin_addr.s_addr = inet_addr(req.destIP.c_str());

  if ((sockfd = socket (PF_INET, SOCK_STREAM, 0)) < 0) {
    Error::error("Build socket fail");
    return -1;
  }

  if (connect (sockfd, (const struct sockaddr *) &serv_addr, sizeof (serv_addr)) < 0) {
    perror("connect:");
    Error::error("Connect fail");
    return -1;
  }
  cerr << "[CONNECT Finish]" << endl;
  return sockfd;
}

int Socks4::bind(Reply &rep){
  cerr << "[BIND]" << endl;
  int sockfd, newsockfd;
  socklen_t clilen = sizeof (struct sockaddr_in);
  struct sockaddr_in cli_addr; 

  if ((sockfd = Server::passiveTCP (0, 0)) < 0) {
    Error::error("PassiveTCP error");
    return -1;
  }
  getsockname(sockfd, (struct sockaddr *)&cli_addr, &clilen);
  rep.destIP = "0.0.0.0";
  rep.destPort = ntohs(cli_addr.sin_port);
  rep.sendreply(); 
  clilen = sizeof (struct sockaddr_in);
  if ((newsockfd = accept (sockfd, (struct sockaddr *) &cli_addr, &clilen)) < 0) {
    Error::error("Accept Error");
    return -1;
  }
  rep.destIP = "0.0.0.0";
  return newsockfd;
}

void Socks4::exchange(int sockfd){
  int flag = 1;
  fd_set rfds, afds;
  memset(&afds, 0, sizeof(afds));
  FD_SET(sockfd, &afds);
  FD_SET(0, &afds);

  while(1){
    memcpy(&rfds, &afds, sizeof(afds));
    if(select(sockfd + 1, &rfds, NULL, NULL, NULL) < 0){
      Error::error("Select fail");
      return;
    }
    if(FD_ISSET(sockfd, &rfds)){
      cerr << "[sockfd sockfd]" << endl;
      char buf[MAXLEN];
      int len = read(sockfd, buf, sizeof(buf));
      if (!len){
        cerr << "[EXIT]" << endl;
        close(sockfd);
        close(1);
        close(0);
        return;
      }

      buf[len] = 0;
      cerr << "[from] " << buf;
      if(flag) {
        cerr << "<Content> :" << buf << endl;
        flag = 0;
      }
      cerr << "[Content] " <<  buf << endl;
      write(1, buf, len);
      fflush(stdout);
    }
    if(FD_ISSET(0, &rfds)){
      cerr << "[sockfd 0]" << endl;
      char buf[MAXLEN];
      int len = read(0, buf, sizeof(buf));
      if (!len) {
        cerr << "[EXIT]" << endl;
        close(sockfd);
        close(0);
        close(1);
        return;
      }

      buf[len] = 0;
      write(sockfd, buf, len);
    }
  }
}

