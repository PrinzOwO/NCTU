#include "command.h"
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <utime.h>

void command::Error(){
    cerr << "Error: " << strerror(errno) << endl;
}

string ReturnType(struct stat fileStat){
    if(S_ISREG(fileStat.st_mode)) return "-Regular file";
    else if(S_ISDIR(fileStat.st_mode)) return "dDirectory files";
    else if(S_ISBLK(fileStat.st_mode)) return "bBlock file";
    else if(S_ISCHR(fileStat.st_mode)) return "cCharacter device file";
    else if(S_ISFIFO(fileStat.st_mode)) return "pPipe file";
    else if(S_ISLNK(fileStat.st_mode)) return "lSymbolic link file";
    else if(S_ISSOCK(fileStat.st_mode)) return "sSocket file";
    else return "?Unknown file";
}

string ReturnPermission(struct stat fileStat){
    int mode = (fileStat.st_mode & (S_IRWXU | S_IRWXG | S_IRWXO)); 
    string msg;               
    for(int i = 0, j = 8 * 8; i < 3; i++, j /= 8){
        msg.push_back((mode & (4 * j)) ? 'r': '-');
        msg.push_back((mode & (2 * j)) ? 'w' : '-');
        msg.push_back((mode & (1 * j)) ? 'x' : '-');
    }
    return msg;
}

void command::Cat(vStr argv){
    if(argv.size() != 2){
        cout << "Usage: cat {file}" << endl;
        return;
    }
    ifstream fin(argv[1]);
    if(!fin.is_open()){
        cout << "cat: " << argv[1] << ": file doesn't exist." << endl;
        return;
    }
    string msg((istreambuf_iterator<char> (fin)), istreambuf_iterator <char>());
    cout << msg << endl;
}

void command::Cd(vStr argv){
    if(argv.size() != 2){
        cout << "Usage: cd {dir}" << endl;
        return;
    }
    if(chdir(argv[1].c_str()))
        command::Error();
}

void command::Chmod(vStr argv){
    if(argv.size() != 3){
        cout << "Usage: chmod {mode} {file/dir}-" << endl;
        return;
    }
    int tmp;
    stringstream ss;
    ss << argv[1];
    ss >> oct >> tmp;
    if(chmod(argv[2].c_str(), tmp))
        command::Error();
}

void command::Echo(vStr argv){
    if(argv.size() == 2)
        cout << argv[1] << endl;
    else if(argv.size() == 3){
        ofstream fout(argv[2].c_str(), ofstream::out | ofstream::app);
		if(!fout.is_open())
			cout << "Error: Permission deny" << endl;
		else
        	fout << argv[1] << endl;
    }
    else{
        cout << "Usage: echo {str} [filename]" << endl;
        return;
    }
}

void command::Exit(vStr argv){
    if(argv.size() != 1){
        cout << "Usage: exit" << endl;
        return;
    }
    exit(0);
}

void command::Find(vStr argv){
    if(argv.size() == 1 || argv.size() == 2){
        string path("./");
        if(argv.size() == 2) 
            path = argv[1] + "/";
        DIR *dirPtr = opendir(path.c_str());
        if(dirPtr == NULL)
            command:Error();
        else{
            struct dirent *filePtr;
            struct stat fileStat;
            while((filePtr = readdir(dirPtr)) != NULL){
				string newpath = path + filePtr->d_name;
                stat(newpath.c_str(), &fileStat);
                cout << ReturnType(fileStat)[0];
                cout << ReturnPermission(fileStat) << " ";
                cout << setw(10) << right << fileStat.st_size << left;
                cout << ' ';
                cout << setw(20) << filePtr->d_name << endl;
            }
        }
        closedir(dirPtr);
    }
    else
        cout << "Usage: find [dir]" << endl;
}

void command::Help(vStr argv){       
    if(argv.size() != 1){
        cout << "Usage: help" << endl;
        return;
    }
    string msg = "cat {file}:              Display content of {file}.\n"
                 "cd {dir}:                Switch current working directory to {dir}.\n"
                 "chmod {mode} {file/dir}: Change the mode (permission) of a file or directory.\n"
                 "                         {mode} is an octal number.\n"
                 "echo {str} [filename]:   Display {str}. If [filename] is given,\n"                                                                    
                 "                         open [filename] and append {str} to the file.\n"
                 "exit:                    Leave the shell.\n"
                 "find [dir]:              List files/dirs in the current working directory\n"
                 "                         or [dir] if it is given.\n"
                 "                         Minimum outputs contatin file type, size, and name.\n"
                 "help:                    Display help message.\n"
                 "id:                      Show current euid and egid.\n"
                 "mkdir {dir}:             Create a new directory {dir}.\n"
                 "pwd:                     Print the current working directory.\n"
                 "rm {file}:               Remove a file.\n"
                 "rmdir {dir}:             Remove an empty directory.\n"
                 "stat {file/dir}:         Display detailed information of the given file/dir.\n"
                 "touch {file}:            Create {file} if it does not exist,\n"
                 "                         or update its access and modification timestamp.\n"
                 "umask {mode}:            Change the umask of the current session.\n";
    cout << msg << endl;
}                    

void command::Id(vStr argv){
    if(argv.size() != 1){
        cout << "USage: id" << endl;
        return;
    }
    cout << "euid=" << geteuid() << '\t' << "egid:" << getegid() << endl;
}

void command::Mkdir(vStr argv){
    if(argv.size() != 2){
        cout << "USage: mkdir {dir}" << endl;
        return;
    }
    if(mkdir(argv[1].c_str(), 0777))
        command::Error();
}

void command::Pwd(vStr argv){
    if(argv.size() != 1){
        cout << "Usage: pwd" << endl;
        return;
    }
    cout << get_current_dir_name() << endl;
}

void command::Rm(vStr argv){
    if(argv.size() != 2){
        cout << "Usage: rm {file}" << endl;
        return;
    }
	struct stat fileStat;
	if(stat(argv[1].c_str(), &fileStat))
		command::Error();
	else if(S_ISDIR(fileStat.st_mode))
		cout << "Error: " << argv[1] << " is directory" << endl;
    else
		if(remove(argv[1].c_str()))
        	command::Error();
}

void command::Rmdir(vStr argv){
    if(argv.size() != 2){
        cout << "Usage: rmdir {dir}" << endl;
        return;
    }
	if(rmdir(argv[1].c_str()))
    	command::Error();
}

void command::Stat(vStr argv){
    if(argv.size() != 2){ 
        cout << "Usage: stat {file/dir}" << endl;
        return;
    }
    struct stat fileStat;
    if(stat(argv[1].c_str(), &fileStat))
        command::Error();
    else{
        string type  = ReturnType(fileStat);
        cout << "File: " << argv[1] << endl;
        cout << "Size: " << setw(10) << left << fileStat.st_size;
        cout << "Blocks: " << setw(10) << left << fileStat.st_blocks;
        cout << "IO Block: " << setw(10) << left << fileStat.st_blksize;
        cout << type.substr(1, type.size() - 1) << endl;
        cout << "Access: (" << type[0] << ReturnPermission(fileStat) << ")  ";
        cout << "Uid: " << setw(10) << left << fileStat.st_uid;
        cout << "Gid: " << setw(10) << left << fileStat.st_gid;
        cout << endl;
        cout << "Access: " << ctime(&fileStat.st_atime);
        cout << "Modify: " << ctime(&fileStat.st_mtime);
        cout << "Change: " << ctime(&fileStat.st_ctime);
    }
}

void command::Touch(vStr argv){
    if(argv.size() != 2){
        cout << "Usage: touch {file}" << endl;
        return;
    }
    ofstream fout(argv[1].c_str(), ios::out | ios::app);
	if(!fout.is_open())
		cout << "Error: Permission deny" << endl;
	else{
	    fout.close();
    	struct utimbuf ut;
    	ut.actime = time(NULL);
    	ut.modtime = time(NULL);
    	utime(argv[1].c_str(), &ut);
	}
}

void command::Umask(vStr argv){
    if(argv.size() != 2){
        cout << "Usage: umask {mode}" << endl;
        return;
    }
    stringstream ss;
    int mask;
    ss << argv[1];
    ss >> oct >> mask;
    umask(mask);
    ss >> dec;
}
