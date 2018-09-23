#include <bits/stdc++.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <sys/socket.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <unistd.h>

using namespace std;

#ifndef __LAN__
#define __LAN__


class Info{
public:
    int idx;
    string name;
    struct sockaddr_ll *MAC;  
    struct sockaddr_in *IPv4;
    struct sockaddr_in *mask;
    struct sockaddr_in *broadcast;
    struct ifreq ifreq_c;
    
    string getMAC();
    string getIPv4();
    string getMask();
    string getBroadcast();
    void show();
    int sendPkt(int, string);
};

class NIC{
public:
    map<string, Info> nic;

    void show();
    void sendPkt(int, string);
    void recvPkt(int);
    NIC();
};

string setName();

int CreateSocket(NIC &);

#endif
