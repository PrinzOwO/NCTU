#include "Process/process.h"
#include <getopt.h>
#include <unistd.h>

using namespace std;

int main(int argc, char *argv[]){

    char c;
    int flag[7];
    memset(flag, 0, sizeof(flag));

    while ((c = getopt(argc, argv, "axpqrst")) != EOF){
        switch(c){
            case 'a':
                flag[0] = 1;
                break;
            case 'x':
                flag[1] = 1;
                break;
            case 'p':
                flag[2] = 1;
                break;
            case 'q':
                flag[3] = 1;
                break;
            case 'r':
                flag[4] = 1;
                break;
            case 's':
                flag[5] = 1;
                break;
            case 't':
                flag[6] = 1;
                break;
        }
    }

    Table t(flag);
    t.show();
    return 0;
}
