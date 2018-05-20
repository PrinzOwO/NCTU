#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stack>
#include <unordered_map>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAXBUF     10000
#define MAXCMD     5010
#define MAXCMDLINE 300

using namespace std;

namespace Shell{ 
  
  
  struct Info{
    int valid;
    int sockfd;
    int nowid;
    int ppnum;
    string buf;
    string cmd[MAXCMD][MAXCMDLINE];
    char **argvs[MAXCMD];
    char *argv[MAXCMD][MAXCMDLINE];
    vector<int> pp2;
    unordered_map<int, string> filein;
    unordered_map<int, string> fileout;
    unordered_map<int, string> catchbuf;


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
    }
    
    Info(int sockfd = 0){
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

    bool read_cmd(Shell::Info &info){
      info.init();

      char tmp[MAXBUF];

      while(1){
        int len = read(info.sockfd, tmp, sizeof(tmp));

        for(int i = 0; i < len; i++){
          if(tmp[i] != '\r' && tmp[i] != '\n')
            info.buf.push_back(tmp[i]);
        }
        if(tmp[len - 2] == '\r' && tmp[len - 1] == '\n') break;
      }

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

    void pipe2(Shell::Info &info){
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
    
    void parse(Shell::Info &info){
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
    void run(Shell::Info &info){
      if(read_cmd(info) == 0) return; 

      strip(info.buf);
      
      pipe2(info);

      fileIO('>', info.buf, info.fileout);

      fileIO('<', info.buf, info.filein);

      parse(info);
/*
      cout << "Cmd : " << info.buf << endl;
      cout << "PP2 : ";
      for(auto i : info.pp2) cout << i << endl;
      cout << "FIN : ";
      for(auto i : info.filein) cout << i.first << i.second << endl;
      cout << "FOUT : ";
      for(auto i : info.fileout) cout << i.first << i.second << endl;
      cout << "argv 0 :";
      cout << info.argv[0][0] << endl;
*/
    }

  };

  class Process{
  private:
    int Ifd[2], Ofd[2], errfd[2], unfd[2];

    void printerror(Shell::Info &info, int idx){
      write(info.sockfd, "Unknown command: [", 18);
      for(int j = 0; info.argvs[idx][j]; j++){
        if(j == 0){
          write(info.sockfd, info.argvs[idx][j], strlen(info.argvs[idx][j]));
        }
        else{
          write(info.sockfd, " ", 1);
          write(info.sockfd, info.argvs[idx][j], strlen(info.argvs[idx][j]));
        }
      }
      write(info.sockfd, "].\n", 3);
      if(idx != 0) info.nowid--;
    }
    
    
    void call_exit(int sockfd){
      cout << "[Debug]" << sockfd << " Logout!" << endl;
      close(sockfd);
      exit(0);
    }
    
    void call_setenv(Shell::Info &info, int idx){
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

    void call_getenv(Shell::Info &info, int idx){
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

    bool call_special_function(Shell::Info &info, int idx){
        bool flag = 1;
        if(strcmp(info.argvs[idx][0], "exit") == 0) call_exit(info.sockfd);
        else if(strcmp(info.argvs[idx][0], "setenv") == 0) call_setenv(info, idx);
        else if(strcmp(info.argvs[idx][0], "printenv") == 0) call_getenv(info, idx);
        else flag = 0;

        return flag;
    }

    void read_inputfile(Shell::Info &info, int idx){
      cout << "[Debug]Read File!" << endl;
      string tmp;
      ifstream fin;
      fin.open(info.filein[idx]);
      while(getline(fin, tmp)){
        info.catchbuf[info.nowid] += (tmp + "\n");
      }
      fin.close();
    }

    void write_outputfile(Shell::Info &info, int idx){
      cout << "[Debug]Write File!" << endl;
      ofstream fout;
      fout.open(info.fileout[idx]);
      fout << info.catchbuf[info.nowid];
      info.catchbuf.erase(info.nowid);
    }

    void socket_output(Shell::Info &info){
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
    
    void create_process(Shell::Info &info, int idx){
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
          cout << "## " << info.catchbuf[info.nowid] << endl; 
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

    void create_pipe(Shell::Info &info){
      for(int i = 0; i < info.ppnum; i++){
        cout << "[Debug]Process " << info.nowid << "->" << info.nowid + info.pp2[i] << endl;
        
        if(call_special_function(info, i)) continue;
        cout << "[Debug]Other Command!" << endl;
        
        if(info.filein.find(i) != info.filein.end()) read_inputfile(info, i);
        
        four_pipe();       
        
        create_process(info, i);

        if(info.valid == 0) return;

        if(info.fileout.find(i) != info.fileout.end()) write_outputfile(info, i);

        if(info.pp2[i] == 0 && info.catchbuf.find(info.nowid) != info.catchbuf.end()) socket_output(info);

        info.nowid++;
      }
      cout << endl;
    }

  public:
    void run(Shell::Info &info){
      if(info.valid == 0) return;
      create_pipe(info);     
    }
  
  };

  class Login{
  private:
    void envsetting(){
      chdir("/net/cs/104/0416042/ras");
      clearenv();
      setenv("PATH", "bin:.", 1);
    }
    void welcome(int sockfd){
      string wel = "****************************************\n"
                   "** Welcome to the information server. **\n"
                   "****************************************\n"; 
      write(sockfd, wel.c_str(), wel.size());
    }

    void prompt(int sockfd){
      string prom = "% ";
      write(sockfd, prom.c_str(), prom.size());
    }

  public:
    Shell::Info info;

    void run(){
      envsetting();
      welcome(info.sockfd);
   
      Parser parser;
      Process process;
   
      while(1){
        prompt(info.sockfd);
        info.valid = 2;
        parser.run(info);
        process.run(info);
      }

    }

    Login(int sockfd){
      info.sockfd = sockfd;
      cout << "[Debug]" << sockfd << " login!" << endl;
    }
  };
  
};
