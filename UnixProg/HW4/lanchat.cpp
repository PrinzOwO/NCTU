#include "lan.h"
#include <pthread.h>
#include <linux/if_ether.h>

using namespace std;

NIC nic;
string name;

void *loopRead(void *sockfd){
    while(1){
	nic.recvPkt(*(int *)sockfd);
    } 
}

int main(){

    pthread_t rThread;

    cout << "Enumerated network interfaces:" << endl;
    
    name = setName();

    int sockfd = CreateSocket(nic);

    int rthead_id = pthread_create(&rThread, NULL, loopRead, (void *) &sockfd);
    if(rthead_id) {
	cerr << "rthread Create Error" << endl;
	exit(0);
    } 

    string msg;
    getline(cin, msg);
    while(1){
	cout << ">>> ";
	fflush(stdout);
	getline(cin, msg);
	if(msg.size() == 0) continue;
	nic.sendPkt(sockfd, string("[" + name + "]: " + msg));
    }
}
