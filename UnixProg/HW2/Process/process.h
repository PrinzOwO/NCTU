#include <bits/stdc++.h>

using namespace std;

#ifndef __PROC__
#define ROOTDIR "/proc/"

class Proc{
public:
    string pid;
    string uid;
    string gid;
    string ppid;
    string pgid;
    string sid;
    string tty; 
    string St;
    string img;
    string cmd;

    void show();
    Proc(string);
};

int sortByPid(Proc &, Proc &);
int sortByPpid(Proc &, Proc &);
int sortByPgid(Proc &, Proc &);
int sortBySid(Proc &, Proc &);

class Table{
public:
    vector<Proc> entry;
    map<unsigned int, string> m;
    int *flags;

    void show();
    void findDevices(string, string);
    Table(int *);
};
#endif
