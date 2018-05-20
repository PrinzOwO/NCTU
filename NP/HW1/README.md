# NP Hw1 #
## ENVIRONMENT ##
* LINUX ONLY 

## notice ## 
* ** all cout in shell.h are using debug **

## TCP Server ##
### Socket ###
```C
#include <sys/socket.h>
int socket(int domain, int type, int protocol);

struct sockaddr_in {
    short            sin_family;   // domain type, ex:AF_INIT
    unsigned short   sin_port;     // set port
    struct in_addr   sin_addr;     // set ip
    char             sin_zero[8];  // Not used, must be zero */
};
struct in_addr {
    unsigned long s_addr;          // load with inet_pton()
};
bzero( (char*)&serv_addr, sizeof(serv_addr) ); // init
serv_addr.sin_family = AF_INET;
serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
serv_addr.sin_port = htons(SERV_TCP_PORT);
```

* domain
	* AF_INIT : IPV4
	* AF_INET6 : IPV6
* type 
	* SOCK_STREAM : TCP
	* SOCK_DGRAM : UDP
* protocol 
	* 0 : default protocol
* Socket return 
	* if create fail return -1
* INADDR_ANY : equal to  host ip
* inet_addr("{ip}") : transport string ip to integer

### Bind ###
```C
int bind(int sockfd, struct sockaddr* addr, int addrlen);
```

* sockfd : return value of the socket function
* addr : serv_addr after setting 
* addrlen : sizeof(serv_addr)

### Listen ###
```C
int listen(int sockfd, int backlog);
```
* backlog : maximum number of the connection to server
* Return value
	* 0 : success
	* -1 : fail
	
### Accept ###
```C
int accept(int sockfd, struct sockaddr addr, socklen_t addrlen);
```
* addr : client infomation
* addrlen : addr
* Return value
	* -1 : fail
	
