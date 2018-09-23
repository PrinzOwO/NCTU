#include "lan.h"
#include <typeinfo>


using namespace std;

const int MAXBUF = 1500;


NIC::NIC(){
    
    struct ifaddrs *ifap, *ifa;
    string addr;

    getifaddrs (&ifap);
    for (ifa = ifap; ifa; ifa = ifa->ifa_next) {
	
	// MAC
	if(ifa->ifa_addr->sa_family == AF_PACKET){
	    nic[ifa->ifa_name].MAC = (struct sockaddr_ll*)ifa->ifa_addr;
	    nic[ifa->ifa_name].name = ifa->ifa_name;
	    nic[ifa->ifa_name].idx = if_nametoindex(ifa->ifa_name);
	}

	// IP
        if (ifa->ifa_addr->sa_family == AF_INET) {
            nic[ifa->ifa_name].IPv4 = (struct sockaddr_in *) ifa->ifa_addr;
            nic[ifa->ifa_name].mask = (struct sockaddr_in *) ifa->ifa_netmask;
            nic[ifa->ifa_name].broadcast = (struct sockaddr_in *) ifa->ifa_broadaddr;
        }
    }
    
    freeifaddrs(ifap);
    show();

}

void NIC::show(){
    for(int i = 1; i <= int(nic.size()); i++){
	for(auto &obj : this->nic){
	    if(obj.second.idx == i){
		obj.second.show();
		break;
	    }
	}
    }
}

void NIC::sendPkt(int fd, string msg){
    for(auto &obj : nic)
	if(obj.second.name != "lo")
	    obj.second.sendPkt(fd, msg);
}

void NIC::recvPkt(int fd){
    char msg[2048];
    int len = read(fd, msg, sizeof(msg)); 
    
    cout << "<";
    for(int i = 6; i < 12; i++)
        cout << hex << setfill('0') << setw(2) << (unsigned int)(unsigned char)msg[i] << (i < 11 ? ":" : "> ");
    fflush(stdout);

    write(1, msg + 14, len - 14);
    cout << endl;
}

string Info::getMAC(){
    string ret;
    stringstream ss;
    for(int i = 0; i < 6; i++){
	ss << hex << setfill('0') << setw(2) << (int)MAC->sll_addr[i];
	if(i < 5) ss << ':';
    }
    ss >> ret;
    return ret;
}

string Info::getIPv4(){
    return inet_ntoa(this->IPv4->sin_addr);
}

string Info::getMask(){
    return inet_ntoa(this->mask->sin_addr);
}

string Info::getBroadcast(){
    return inet_ntoa(this->broadcast->sin_addr);
}


void Info::show(){
    cout << this->idx << " - ";
    cout << this->name << "\t";
    cout << this->getIPv4() << "\t";
    cout << this->getMask() << "\t";
    cout << this->getBroadcast() << "\t";
    cout << this->getMAC() << endl;
}

int Info::sendPkt(int fd, string msg){
    char buf[MAXBUF];
    memset(buf, 0, sizeof(buf));

    struct ethhdr *eth = (struct ethhdr *) buf;
    memcpy(eth->h_source, ifreq_c.ifr_hwaddr.sa_data, sizeof(eth->h_source));

    for(int i = 0; i < 6; i++)
	eth->h_dest[i] = (unsigned char)0xff;

    eth->h_proto = htons(ETH_P_IP + 1);
    
    strcpy(buf + 14, msg.c_str());    

    struct sockaddr_ll  sadr_ll;
    sadr_ll.sll_ifindex = this->idx;
    sadr_ll.sll_halen = ETH_ALEN;
    for(int i = 0; i < 6; i++)
	sadr_ll.sll_addr[0] = (unsigned char)0xff;

    int len = sendto(fd, buf, (14 + msg.size()), 0, (const struct sockaddr *) &sadr_ll, sizeof(struct sockaddr_ll));
    if(len < 0) cerr << "Send Error!\n";

    return len;
}


string setName(){
    string name;
    cout << "Enter your name: ";
    cin >> name;
    cout << "Welcome, '" << name << "'!" << endl;
    return name;
}

int CreateSocket(NIC &interface){
    int sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_IP + 1));
    if(sockfd < 0){
	cerr << "Socket create error!\n";
	exit(-1);
    }
    
    for(auto &obj : interface.nic){
	if(obj.second.name == "lo") continue;
	memset(&obj.second.ifreq_c, 0, sizeof(obj.second.ifreq_c));
	strncpy(obj.second.ifreq_c.ifr_name, obj.second.name.c_str(), IFNAMSIZ - 1);

	if(ioctl(sockfd, SIOCGIFHWADDR, &obj.second.ifreq_c) < 0){
	    cerr << "ioctl error\n";
	    exit(-1);
	}
    }

    return sockfd;
}

