#include <bits/stdc++.h>
#include <unistd.h>

using namespace std;

#ifndef __COMMAND__
#define __COMMAND__

typedef vector<string> vStr;

namespace command{
    void Error();
    void Cat(vStr);
    void Cd(vStr);
    void Chmod(vStr);
    void Echo(vStr);
    void Exit(vStr);
    void Find(vStr);
    void Help(vStr);
    void Id(vStr);
    void Mkdir(vStr);
    void Pwd(vStr);
    void Rm(vStr);
    void Rmdir(vStr);
    void Stat(vStr);
    void Touch(vStr);
    void Umask(vStr);
}
#endif
