#include "server.h"

int Server::passiveTCP(int port, int qlen){
  int sockfd;
  struct sockaddr_in serv_addr;

  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    Error::error("Socket open fail");
    exit(-1);
  }
  memset((char *) &serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = htonl (INADDR_ANY);
  serv_addr.sin_port = htons (port);

  if (bind (sockfd, (const struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    Error::error("Cannot bind local address");
    exit(-1);
  }
  if (listen (sockfd, qlen) < 0) {
    Error::error("Listen failed");
    exit(-1);
  }
  return sockfd;
}

int Server::httpd(){
  
  Server::Header header;
  Server::readpkg(header);
  Server::set_envs(header);
  
  cout << "HTTP/1.1 200 OK\r\n";
  if(header.URI == "hw3.cgi")
    Server::exec_hw3();
  else if(header.URI.substr(header.URI.size() - 3, 3) == "cgi") 
    Server::exec_cgi();
  else if(header.URI.substr(header.URI.size() - 3, 3) == "htm")
    Server::exec_htm();
  return 0;
}

void Server::readpkg(Server::Header &header){
  char tmp[10005], *tok;
  read(0, tmp, sizeof(tmp));
  // Method
  tok = strtok(tmp, " \r\n");
  header.method = string(tok); 

  // URI
  tok = strtok(NULL, " \r\n");
  header.URI = string(tok);
  if(header.URI[0] == '/') header.URI.erase(0, 1);
  for(int i = 1; i < int(header.URI.size()); i++){
    if(header.URI[i - 1] == '?') {
      header.qstr = header.URI.substr(i, int(header.URI.size()) - i + 1);
      header.URI.erase(i - 1, int(header.URI.size()) - i + 2);
      break;
    }
  }

  // Protocol
  tok = strtok(NULL, "\r\n");
  header.proto = string(tok);

  // TotalPath
  header.abs_path += header.URI; 
}

void Server::set_envs(Server::Header &header){
  clearenv();
  setenv("REQUEST_METHOD", header.method.c_str(), 1);
  setenv("DOCUMENT_ROOT", prepath, 1);
  setenv("REDIRECT_STATUS", "200", 1);
  setenv("REQUEST_URI", header.URI.c_str(), 1);
  setenv("SCRIPT_NAME", header.URI.c_str(), 1);
  setenv("QUERY_STRING", header.qstr.c_str(), 1);
  setenv("SERVER_PROTOCOL", header.proto.c_str(), 1);
  setenv("SCRIPT_FULLNAME", header.abs_path.c_str(), 1);

}

void Server::exec_hw3(){
  cerr << "HW3" << endl;
  char filename[] = "/u/cs/104/0416042/NP/HW3/cgiclient";
  char *argv[] = { filename, NULL };
  cout << "Content-Type: text/plain\r\n\r\n";
//  fflush(stdout);
  execvp(argv[0], argv);
  exit(-1);
}

void Server::exec_cgi(){
  cout << "CGI" << endl;
  char *argv[] = { getenv("SCRIPT_FULLNAME"), NULL };
  cerr << getenv("SCRIPT_FULLNAME") << endl;
  execvp(argv[0], argv);
  exit(-1);
}

void Server::exec_htm(){
  cerr << "HTML" << endl;
  int filefd = open(getenv("SCRIPT_FULLNAME"), O_RDONLY);
  struct stat filestat;
  stat(getenv("SCRIPT_FULLNAME"), &filestat); 
  sendfile(1, filefd, 0, filestat.st_size);
  close(filefd);
  fflush(stdout);
  exit(0);
}
