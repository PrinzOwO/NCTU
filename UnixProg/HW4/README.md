# Chat in LAN

## Homework Description
+ In this homework, you have to implement a simple chat program in LAN using Linux packet(7) socket. Instead of sending IP packets, you have to broadcast chat messages using layer 2 broadcast packets. The requirement of this homework is listed as follows.
    1. Enumerate all Ethernet compatible network interfaces.
    2. Ask the user to provide his/her username.
    3. Repeatedly ask the user to enter his/her message. The message is then broadcasted to all enumerated Ethernet-compatible network interfaces.
    4. At the same time, your program should receive messages broadcasted by other host in connected LANs.
    
+ In addition to the required features, the score of your program will also be graded based on the compiled (and stripped) binary size. The smaller the size, the higher the score.

## Environment
+ Linux 
+ g++ -std=c++14


## How to Use
```
make clean && make
sudo ./lanchat
```

## notice
+ If Ethernet compatible network interfaces doesn't have IP, netmask or broadcast, the process may be segmentation fault
+ If you want to close the process, send SIGINT to the process

## Demo
### User 10.0.2.1
```
Enumerated network interfaces:
1 - lo	127.0.0.1	255.0.0.0	127.0.0.1	00:00:00:00:00:00
2 - enp0s3	10.0.2.15	255.255.255.0	10.0.2.255	08:00:27:d8:6f:e9
3 - enp0s8	10.0.2.1	255.255.255.0	10.0.2.255	08:00:27:bb:04:39
Enter your name: 10.0.2.1
Welcome, '10.0.2.1'!
>>> Hi, 10.0.1.1
>>> <08:00:27:49:42:ba> [10.0.1.1]: Hello, 10.0.2.1
wwwwww
>>> <08:00:27:49:42:ba> [10.0.1.1]: bye
bye
>>> ^C
```
### User 10.0.1.1
```
Enumerated network interfaces:
1 - lo	127.0.0.1	255.0.0.0	127.0.0.1	00:00:00:00:00:00
2 - enp0s3	10.0.2.15	255.255.255.0	10.0.2.255	08:00:27:90:69:f6
3 - enp0s8	10.0.1.1	255.255.255.0	10.0.1.255	08:00:27:49:42:ba
Enter your name: 10.0.1.1
Welcome, '10.0.1.1'!
>>> <08:00:27:bb:04:39> [10.0.2.1]: Hi, 10.0.1.1
Hello, 10.0.2.1
>>> <08:00:27:bb:04:39> [10.0.2.1]: wwwwww
bye
>>> ^C
```
