#include "Command/command.h"

using namespace std;

void Init(char **argv){
    string path = "/home/user/tmproot";
    clearenv();
    chdir(path.c_str());
    if(setegid(atoi(argv[2])))
		perror(strerror(errno));
    if(seteuid(atoi(argv[1])))
		perror(strerror(errno));

}

vector<string> parser(string argv){
    string s;
    vStr tmp;
    stringstream ss;
    ss << argv;
    while(ss >> s)
        tmp.push_back(s);
    return tmp;
}

void exec(vStr argv){
    if(argv.empty()) 
        return;
    else if(argv[0] == "cat")
        command::Cat(argv);
    else if(argv[0] == "cd")
        command::Cd(argv);
    else if(argv[0] == "chmod")
        command::Chmod(argv);
    else if(argv[0] == "echo")
        command::Echo(argv);
    else if(argv[0] == "exit")
        command::Exit(argv);
    else if(argv[0] == "find")
        command::Find(argv);
    else if(argv[0] == "help")
        command::Help(argv);
    else if(argv[0] == "id")
        command::Id(argv);
    else if(argv[0] == "mkdir")
        command::Mkdir(argv);
    else if(argv[0] == "pwd") 
       command::Pwd(argv);
    else if(argv[0] == "rm") 
       command::Rm(argv);
    else if(argv[0] == "rmdir")
       command::Rmdir(argv);
    else if(argv[0] == "stat")
        command::Stat(argv);
    else if(argv[0] == "touch")
        command::Touch(argv);
    else if(argv[0] == "umask")
        command::Umask(argv);
    else
        cout << "sish: command not found: " << argv[0] << endl;
}

int main(int argc, char *argv[]){
    string s;
    Init(argv);
    while(1){
        cout << "[XD] % ";
        getline(cin, s);
        exec(parser(s));
    }
    return 0;

}

