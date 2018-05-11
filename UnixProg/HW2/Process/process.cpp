#include "process.h"
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>


void Proc::show(){
    cout << setw(5) << pid;
    cout << setw(6) << uid;
    cout << setw(6) << gid;
    cout << setw(6) << ppid;
    cout << setw(6) << pgid;
    cout << setw(6) << sid;
    cout << setw(9) << tty;
    cout << setw(3) << St;
    
    cout << " ";
    cout << img << " ";
    cout << cmd << endl;
}

Proc::Proc(string pid = ""){
    string tmp;

    ifstream statfin(ROOTDIR + pid + "/stat");
    if(!statfin.is_open()){
        cerr << "PID: " << pid << " is running?" << endl;
        return;
    }
    vector<string> para;
    para.clear();
    for(int i = 0, flag = 0; i < 7; i++){
        statfin >> tmp;
        if(!flag) para.push_back(tmp);
        else {
            i--;
            para.back() += (" " + tmp);
        }
        if(tmp[0] == '(') flag = 1;
        if(tmp.back() == ')') flag = 0;
    }
    statfin.close();
    this->pid = para[0];
    this->ppid = para[3];
    this->pgid = para[4];
    this->sid = para[5];
    this->tty = para[6];
    this->St = para[2];
    this->img = para[1];
    
    ifstream statusfin(ROOTDIR + pid + "/status");
    while(statusfin >> tmp){
        if(tmp == "Uid:"){
            statusfin >> tmp;
            this->uid = tmp;
        }
        if(tmp == "Gid:"){
            statusfin >> tmp;
            this->gid = tmp;
            break;
        }
    }
    statusfin.close();
    
    ifstream cmdlinefin(ROOTDIR + pid + "/cmdline");
    while(cmdlinefin >> tmp)
        this->cmd += (tmp + " ");
    for(int i = 0; i < this->cmd.size(); i++)
        if(this->cmd[i] == 0)
            this->cmd[i] = ' ';
    cmdlinefin.close();
}

void Table::show(){
    if(this->flags[3])
        sort(this->entry.begin(), this->entry.end(), sortByPpid);
    else if(this->flags[4])
        sort(this->entry.begin(), this->entry.end(), sortByPgid);
    else if(this->flags[5])
        sort(this->entry.begin(), this->entry.end(), sortBySid);
    else if(this->flags[6]){
        cout << "Comming soon ~~" << endl;
        return;
    }
    else 
        sort(this->entry.begin(), this->entry.end(), sortByPid);

    cout << "  pid   uid   gid  ppid  pgid   sid      tty St (img) cmd" << endl;
    for(auto &info : this->entry)
        info.show();
}

void Table::findDevices(string path, string name = ""){
    DIR *dirPtr = opendir(path.c_str());
    if(dirPtr == NULL) return;
    struct dirent *filePtr;
    struct stat fileStat;
    while((filePtr = readdir(dirPtr)) != NULL){
        string dName = filePtr->d_name;
        stat(string(path + dName).c_str(), &fileStat);
        if(S_ISCHR(fileStat.st_mode))
            this->m[fileStat.st_rdev] = name + dName;
    }
    closedir(dirPtr);
}

Table::Table(int *flags){

    this->flags = flags;

    this->findDevices("/dev/");
    this->findDevices("/dev/pts/", "pts/");
    this->m[0] = "-";

    string myuid = to_string(getuid());
    Proc myproc(to_string(getpid()));
    string mytty = m[atoi(myproc.tty.c_str())];

    DIR *dirPtr = opendir("/proc");
    struct dirent *filePtr;
    while((filePtr = readdir(dirPtr)) != NULL){
        string dName = filePtr->d_name;
        bool flag = true;
        for(int i = 0; i < dName.size(); i++)
            if(dName[i] < '0' || dName[i] > '9'){
                flag = false;
                break;
            }
        if(flag) {
            this->entry.push_back(dName);
            unsigned int tty;
            stringstream ss;
            ss << this->entry.back().tty;
            ss >> tty;
            this->entry.back().tty = m[tty];

            if(flags[1] == 0){
                if(this->entry.back().tty == "-"){
                    this->entry.pop_back();
                    continue;
                }
            }
            if(flags[0] == 0){
                if(this->entry.back().uid != myuid){
                    this->entry.pop_back();
                    continue;
                }
                if(flags[1] == 0 && this->entry.back().tty != mytty){
                    this->entry.pop_back();
                    continue;
                }
            }
        }
    }
    closedir(dirPtr);
}

int sortByPid(Proc &l, Proc &r){
    return atoi(l.pid.c_str()) < atoi(r.pid.c_str());
}

int sortByPpid(Proc &l, Proc &r){
    return atoi(l.ppid.c_str()) < atoi(r.ppid.c_str());
}
int sortByPgid(Proc &l, Proc &r){
    return atoi(l.pgid.c_str()) < atoi(r.pgid.c_str());
}
int sortBySid(Proc &l, Proc &r){
    return atoi(l.sid.c_str()) < atoi(r.sid.c_str());
}
