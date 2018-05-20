#include "shell.h"

void Shell::prompt(int sockfd){
  write(sockfd, "% ", 2);
}

void Shell::welcomemsg(int sockfd){
  string msg = "****************************************\n"
               "** Welcome to the information server. **\n"
               "****************************************\n";
  write(sockfd, msg.c_str(), msg.size());
}

string Shell::readmsg(int sockfd){
  char buf[MAXSIZE];
  int len = read(sockfd, buf, sizeof(buf));
  buf[len] = 0;
  return string(buf);
}

void Shell::Info::loadEnv(){
  chdir("/net/cs/104/0416042/rwg1");
  clearenv();
  setenv("PATH", "bin:.", 1);

  for(auto &m : this->env)
    setenv(m.first.c_str(), m.second.c_str(), 1);
}

void Shell::Info::setEnv(string var1, string var2){
  this->env[var1] = var2;
}

void Shell::Info::init(){
  this->valid = 1;
  this->buf.clear();
  this->rawbuf.clear();
  for(int i = 0; i < MAXCMD; i++){
    this->argvs[i] = NULL;
    for(int j = 0; j < MAXSUBCMD; j++){
      this->cmd[i][j].clear();
      if(this->argv[i][j] != NULL) free(this->argv[i][j]);
      this->argv[i][j] = NULL;
    }
  }
  this->pp2.clear();
  this->filein.clear();
  this->fileout.clear();
  this->clientin.clear();
  this->clientout.clear();
}

int Shell::Info::setValid(){
  if(this->buf.size() == 0) this->valid = 0;
  else{
    for(uint i = 0; i < this->buf.size(); i++)
      if(this->buf[i] == '/') {
        this->valid = 0;
        break;
      }
  }
  return this->valid;
}

void Shell::Info::setReady(){
  this->rawbuf = this->buf;
  while(this->rawbuf.back() == '\n' || this->rawbuf.back() == '\r')
    this->rawbuf.pop_back();
}

void Shell::Info::strip(){
  while(this->buf.back() == ' ' || this->buf.back() == '\n' || this->buf.back() == '\r') 
    this->buf.pop_back();
  int cnt = 0;
  while(this->buf[cnt] == ' ' || this->buf[cnt] == '\n' || this->buf[cnt] == '\r') 
    cnt++;
  this->buf.erase(0, cnt);
}

void Shell::Info::setPipe(){
  for(uint i = 0; i < this->buf.size(); i++){
    if(this->buf[i] == '|'){
      int sum = 0;
      i++;
      while(i < this->buf.size() && this->buf[i] >= '0' && this->buf[i] <= '9'){
        sum = sum * 10 + this->buf[i] - '0';
        this->buf[i++] = ' ';
      }
      this->pp2.push_back((sum == 0) ? 1 : sum);
    }
  }
}

void Shell::Info::setOtherIO(){
  for(uint i = 0, cnt = 0; i < this->buf.size(); i++){
    if(this->buf[i] == '|') cnt++;
    else if(this->buf[i] == '>'){
      this->buf[i++] = ' ';
      if(this->buf[i] == ' '){
        i++;
        string tmp;
        while(i < this->buf.size() && buf[i] != ' '){
          tmp += this->buf[i];
          this->buf[i++] = ' ';
        }
        this->fileout[cnt] = tmp;
      }
      else{
        int sum = 0;
        while(i < this->buf.size() && this->buf[i] >= '0' && this->buf[i] <= '9'){
          sum = sum * 10 + this->buf[i] - '0';
          this->buf[i++] = ' ';
        }
        clientout[cnt] = sum;
      }
    }
    else if(this->buf[i] == '<'){
      this->buf[i++] = ' ';
      if(this->buf[i] == ' '){
        i++;
        string tmp;
        while(i < this->buf.size() && buf[i] != ' '){
          tmp += this->buf[i];
          this->buf[i++] = ' ';
        }
        this->filein[cnt] = tmp;
      }
      else{
        int sum = 0;
        while(i < this->buf.size() && this->buf[i] >= '0' && this->buf[i] <= '9'){
          sum = sum * 10 + this->buf[i] - '0';
          this->buf[i++] = ' ';
        }
        clientin[cnt] = sum; 
      }
    }
  }
}

void Shell::Info::setCmd(){
  for(uint i = 0, j = 0, k = 0; i < this->buf.size();){
    if(this->buf[i] == '|'){
      i++;
      j++;
      k = 0;
      while(i < this->buf.size() && this->buf[i] == ' ') i++;
    }
    else if(this->buf[i] == ' '){
      i++;
      k++;
    }
    else{
      this->cmd[j][k].push_back(this->buf[i++]);
    }
  }

  uint cnt = 0;
  for(int i = 0; this->cmd[i][0].size() > 0; i++){
    cnt++;
    for(int j = 0; this->cmd[i][j].size() > 0; j++){
      this->argv[i][j] = strdup(this->cmd[i][j].c_str());
    }
    this->argvs[i] = this->argv[i];
  }

  this->ppnum = cnt;
  while(cnt > this->pp2.size()) this->pp2.push_back(0);
}

void Shell::Info::printerror(int idx){
  string tmp = "Unknown command: [" + string(this->argvs[idx][0]) + "].\n";
  /*
  for(int i = 0; this->argvs[idx][i]; i++){
    if(i) tmp += " " + string(this->argvs[idx][i]);
    else tmp += string(this->argvs[idx][i]);
  }
  tmp += "].\n";
  */
  write(this->sockfd, tmp.c_str(), tmp.size());
  if(idx) this->nowid--;
}

void Shell::Info::callSetenv(int idx){
  int cnt = 1;
  while(this->argvs[idx][cnt]) cnt++;
  if(cnt == 3) {
    setenv(this->argvs[idx][1], this->argvs[idx][2], 1);
    this->setEnv(this->argvs[idx][1], this->argvs[idx][2]);
  }
  else this->printerror(idx);
}

void Shell::Info::callGetenv(int idx){
  int cnt = 1;
  while(this->argvs[idx][cnt]) cnt++;
  if(cnt == 2) {
    char *msg = getenv(this->argvs[idx][1]);
    string tmp = string(this->argvs[idx][1]) + "=";
    tmp += string(msg) + "\n";
    write(this->sockfd, tmp.c_str(), tmp.size());
  }
  else this->printerror(idx);
}

void Shell::Info::exec(Chat::Table &online){
  for(int i = 0; i < this->ppnum; i++){
    if(strcmp(this->argvs[i][0], "setenv") == 0)
      this->callSetenv(i);
    else if(strcmp(this->argvs[i][0], "printenv") == 0)
      this->callGetenv(i);
    else{
      if(this->filein.find(i) != this->filein.end()){
        string tmp;
        ifstream fin(this->filein[i]);
        while(getline(fin, tmp))
          this->catchbuf[this->nowid] += (tmp + "\n");
        fin.close();
      }

      if(this->clientin.find(i) != this->clientin.end()){
        this->catchbuf[this->nowid] += online.recvmsg(this->sockfd, clientin[i], this->rawbuf);
      }

      if(this->fileout.find(i) != this->fileout.end()){
        ofstream fout(this->fileout[i]);
        fout.close();
      }

      int Ifd[2], Ofd[2], errfd[2], unfd[2];
      pipe(Ifd); pipe(Ofd); pipe(errfd); pipe(unfd);

      pid_t pid = fork();
      if(pid < 0) {
        cout << "Fork Fail" << endl;
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

        if(execvp(this->argvs[i][0], this->argvs[i]) < 0){
          printerror(i);
          write(unfd[1], "Error", 5);
          close(unfd[0]);
          close(unfd[1]);

          exit(-1);
        }
      }
      else{
        char tmp[MAXSIZE];
        int len;

        if(this->catchbuf.find(this->nowid) != this->catchbuf.end()){
          write(Ifd[1], this->catchbuf[this->nowid].c_str(), this->catchbuf[this->nowid].size());
        }
        close(Ifd[1]);
        close(Ifd[0]);
        close(Ofd[1]);
        close(errfd[1]);
        close(unfd[1]);

        wait(NULL);

        while(1){
          len = read(errfd[0], tmp, sizeof(tmp));
          if(len > 0) write(this->sockfd, tmp, len);
          else break;
        }
        close(errfd[0]);

        len = read(unfd[0], tmp, sizeof(tmp));
        if(len > 0){
          if(i == 0) this->nowid++;
          this->valid = 0;
        }
        else{
          this->catchbuf.erase(this->nowid);
          while(1){
            len = read(Ofd[0], tmp, sizeof(tmp));
            if(len > 0) 
              this->catchbuf[this->nowid + this->pp2[i]] += string(tmp, 0, len);
            else break;
          }
        }
        close(unfd[0]);
        close(Ofd[0]);
      }

      if(this->valid == 0) {
        if(i) this->catchbuf.erase(this->nowid);
        return;
      }

      if(this->clientout.find(i) != this->clientout.end()){
        int flag = online.sendmsg(this->sockfd, this->clientout[i], this->catchbuf[this->nowid], this->rawbuf);
        this->catchbuf.erase(this->nowid);
        if(flag == 0) return;
      }

      if(this->fileout.find(i) != this->fileout.end()){
        ofstream fout(this->fileout[i]);
        fout << this->catchbuf[this->nowid];
        fout.close();
        this->catchbuf.erase(this->nowid);
      }

      if(this->pp2[i] == 0 && this->catchbuf.find(this->nowid) != this->catchbuf.end()) 
        write(this->sockfd, this->catchbuf[this->nowid].c_str(), this->catchbuf[this->nowid].size());
      this->catchbuf.erase(this->nowid);

      this->nowid++;
    }
  }
}

