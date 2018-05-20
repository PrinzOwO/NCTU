#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/sendfile.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "error.h"

using namespace std;

#ifndef __SERVER__
#define __SERVER__
namespace Server{

  int passiveTCP(int, int);
}
#endif
