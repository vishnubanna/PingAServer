#include <iostream>
#include <cstdio>
#include <string>
#include <cstring>
#include <ctime>
#include <unistd.h> 
#include <csignal>
#include <cstdlib> 

#include <sys/types.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <sys/socket.h> // Internet Protocal Header
#include <arpa/inet.h>
#include <netinet/ip_icmp.h>
#include <netdb.h> 
#include <fcntl.h>
#include <cerrno>
#include <netinet/udp.h>


//#include <sys/ioctl.h>

#define TYPESIZE 8
#define CODESIZE 8
#define CHECKSUMSIZE 16

#define PCT_SIZE 64
#define SLEEP_TIME 1000000
#define MSCONV 1000000.0

#define PORT_NUM 50000 // the port to bind to

using namespace std;

typedef struct icmp header;


typedef struct _ping_pct{
    header hdr;
    char msg[PCT_SIZE - sizeof(header)];
}ping_pct;


typedef struct _packet{
  uint8_t type; 
  uint8_t code; 
  uint16_t checksum; 
  uint16_t id; 
  uint16_t seq;
  char msg[64 - 2 * 4];
}packet;

typedef struct _retpack{
  unsigned short port; 
  unsigned short dport; 
  unsigned short len;
  unsigned short checksum;
}retpack;

typedef struct _ipv4{
  u_short header1; 
  u_short header2;
  u_short header3; 
  u_short header4;
  uint8_t ttl;
  uint8_t protcol;
  u_short chekcsum; 
  uint32_t sourceip;
  uint32_t destip;
  uint8_t msg[32 - 20];
}ipv4;

string getIPfromDomian(char * domain, struct sockaddr_in * item);
string reverse_ip(string ip);
void interupt_handler(int dummy);
void initSocket(int socketid, int ttl);
unsigned short checksumValue(void * messages, int len);
int send_ping(int socketid, struct sockaddr_in * socket_data, int ttl);



int signal_int = 1;
void interupt_handler(int dummy){
    // catches interupt signals
    signal_int = 0;
}

int main(int argc, char ** argv){
    string ip;
    string domain;
    string inp = argv[1];
    struct sockaddr_in socket_ref; 
    int socketsend;
    int ttl = 64;

    if (inp == "-h" || inp == "--help"){
        cout << "Author: Vishnu Banna" << endl;
        cout << "email:  vbanna@purdue.edu" << endl;
        cout << "How to Run:" << endl;
        cout << "" << endl;
        cout << " if you use the Makefile to compile::" << endl;
        cout << " > make ping" << endl;
        cout << "\n running ::" << endl;
        cout << " > sudo ./ping [ipv4 ip or address] [ttl]" << endl;
        cout << "\n if no ttl is provided, it defaults to 64" << endl;
        cout << endl;
        cout << "NOTE:: This tool was built and tested on Linux 18.04 LTS"<< endl << endl;
        cout << "NOTE:: I collased the header file into ping.cpp for convenience"<< endl << endl;
        cout << "NOTE:: I noticed, that the return code from when a packet was sent is 69" << endl;
        cout << "This is is a restricted type for icmp packets " << endl;
        cout << "This is because the erver reviece sends an ipv4 packet, which is different from the icmp packet" << endl;
        cout << "I fixed this by creating a new container structure to read back the packet that was sent" << endl;
        return EXIT_SUCCESS;
    }
    if (argc < 2){
        return EXIT_FAILURE;
    }

    if (argc == 3){
        ttl = strtol(argv[2], &argv[2], 10);
        cout << ttl << endl;
    }

    memset(&socket_ref, 0, sizeof(socket_ref));
    cout << endl << "domain : " << argv[1] << endl;

    ip = getIPfromDomian(argv[1], &socket_ref);
    if (ip == "error"){
        cout << "error while retriving domain"<< endl;
        return EXIT_FAILURE;
    }
    else{
        cout << "ip address : " << ip << endl;
    }

    domain = reverse_ip(ip.c_str());
    if (domain == "error"){
        cout << "error reversing ip"<< endl;
        return EXIT_FAILURE;
    }
    else{
        cout << "ip reverse search : " << domain << endl;
    }

    cout << "\nsocket break down ::" << endl;
    cout << "Port :: " << (long)socket_ref.sin_port << endl;
    cout << "Int Address ::" << (long)socket_ref.sin_addr.s_addr << endl;
    cout << "Family ::" << (long)socket_ref.sin_family << endl;
    cout << "AF_INET ::" << AF_INET << endl << endl;


    socketsend = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    //socketsend = socket(AF_INET, SOCK_STREAM, 0);

    //socketrcv = socket(AF_INET, SOCK_STREAM, 0);

    if (socketsend < 0){
        cout << "error initializing socket :: " << socketsend << endl;
        cout << "This implementation was designed to run with the command" << endl;
        cout << "run :: sudo ./ping [insert address]" << endl;
        cout << errno << endl;
        close(socketsend);
        return EXIT_FAILURE;
    }else{
        cout << "initializing socket :: " << socketsend << endl;
        cout << "error:: " << errno << endl;
    }

    signal (SIGINT, interupt_handler);
    
    //initSocket(socketrcv);

    //send_ping(socketsend, socketrcv, &socket_ref);
    send_ping(socketsend, &socket_ref, ttl);
    return EXIT_SUCCESS;
}

//look up domain 
//init socket 
//send echo request 
    //wait and time
//recieve echo message 
//read packet and determine if responce is solid



string getIPfromDomian(char * domain, struct sockaddr_in * item){
    struct hostent * value;
    
    value = gethostbyname(domain);
    if (value == NULL){
        return "error";
    }
    char * ip_hldr = new char[INET_ADDRSTRLEN];//INET_ADDRSTRLEN = ipv4 size
    //ip = inet_ntoa(*(in_addr *)value ->h_addr_list[0]);
    if (inet_ntop(AF_INET, value ->h_addr_list[0], ip_hldr, INET_ADDRSTRLEN) == NULL){
        return "error";
    } // AF_INET = ipv4 AF_INET6 = ipv6 

    item -> sin_family = value -> h_addrtype;
    item -> sin_addr.s_addr = *(long *)value -> h_addr_list[0]; // socket adress to bind to
    item -> sin_port = htons(PORT_NUM); // port to listen on

    string ip = ip_hldr;
    delete [] ip_hldr;
    return ip;
}

string reverse_ip(string ip){
    int val = 0;
    in_addr address;
    val = inet_pton(AF_INET, ip.c_str(), &address);
    if (val == 0){
        cout << "invalid address" << endl;
        return "error";
    }
    else if(val == - 1){
        cout << "error" << endl;
        return "error";
    }

    sockaddr_in socket_data;
    socket_data.sin_family = AF_INET; 
    socket_data.sin_addr.s_addr = address.s_addr;
    char name[NI_MAXHOST] = {0};

    //cout << address.s_addr << endl;

    if (getnameinfo((struct sockaddr *) &socket_data, sizeof(socket_data), name, sizeof(name), NULL, 0, NI_NAMEREQD)){
        return "error";
    }
    string nl = name;
    return nl;
}

void initSocket(int socketid, int ttl){ 
    //timespec timeout;
    //timeout.tv_sec = 60;
    //timeout.tv_nsec = 0;

    cout << "\nsetting_opts for socket ::" << socketid << endl;
    if (setsockopt(socketid, IPPROTO_IP, IP_TTL, &ttl, (socklen_t)sizeof(ttl)) != 0) {
        cout << "error setting TTL:: errno ::" << errno << endl;
    }
    else{
        cout << "setting TTL Sucessfull!:: errno ::" << errno << endl;
    }

    const int flags = fcntl (socketid, F_GETFL, 0);
    fcntl(socketid, F_SETFL, flags | O_NONBLOCK);


    // if (setsockopt(socketid, SOL_SOCKET, SO_RCVTIMEO, &timeout, (socklen_t)sizeof(timeout)) != 0) {
    //     cout << "error setting Socket Receive Time Out:: errno ::" << errno << endl;
    // }
    // else{
    //     cout << "setting Socket Receive Time Out Successful:: errno ::" << errno << endl;
    // }

    return;
}


void printip(int ip){
    unsigned char bytes[4];
    bytes[0] = ip & 0xff;
    bytes[1] = (ip >> 8) & 0xff;
    bytes[2] = (ip >> 16) & 0xff;
    bytes[3] = (ip >> 24) & 0xff;

    cout << int(bytes[0]) << "." << int(bytes[1]) << "." << int(bytes[2]) << "." << int(bytes[3]) << endl;
}

unsigned short checksumValue(void * messages, int len){
    unsigned short * data = (unsigned short *)messages;//checksum is 16 bits
    unsigned int sum = 0; 
    unsigned short answer; 
    int i = 0; 

    for(i = len; i > 1; i-=2){
        sum += *data++;
    }
    if (i == 1){
        sum += *((unsigned char *)data);
    }
    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    answer = ~sum;
    return answer;
}

int send_ping(int socketid, struct sockaddr_in * socket_data, int ttl){
    packet data; 
    ipv4 d2;
    int size = sizeof(struct sockaddr_in);
    timespec start_time, stm; 
    timespec stop_time, stp;
    int sent = 0;
    int rcvd = 0;
    int sflag = 1;
    int loss = 0;
    double timetotal = 0;

    initSocket(socketid, ttl);

    int constat = connect(socketid, (sockaddr *)socket_data, size);
    if (constat<0){
        cout << "connection failed :: " << errno << endl;
        return 0;
    }else{
        cout << "connection successful :: " << constat << " :: " << errno << endl;
    }

        
    if (clock_gettime(CLOCK_MONOTONIC_RAW, &stm) == -1){
        cout << "error" << endl;
    }
    while(signal_int){ 
        sflag = 1;
        errno = 0;

        memset(&data, 0, sizeof(packet));
        memset(&d2, 0, sizeof(ipv4));
        data.type = ICMP_ECHO;
        data.code = 0;//'0';
        data.id = getpid();
        for (unsigned int i = 0; i < sizeof(data.msg) - 1; i++){
            data.msg[i] += i;
        }
        data.msg[sizeof(data.msg)-1] = '0';
        data.seq = sent++;
        data.checksum = checksumValue(&data, sizeof(data));
        
        cout << "\npacket send info::" << endl;
        cout << "type: " << (long)data.type << endl;
        cout << "code: " << (long)data.code << endl;
        cout << "id: " << (long)data.id << endl;
        cout << "sequence: " << (long)data.seq << endl;
        cout << "checksum: " << (long)data.checksum << endl;
        
        
        if (clock_gettime(CLOCK_MONOTONIC_RAW, &start_time) == -1){
            cout << "error" << endl;
            break;
        }
        usleep(SLEEP_TIME);
        rcvd = send(socketid, &data, sizeof(data), 0);
        if (errno == 11 || rcvd <= 0){ 
            rcvd = send(socketid, &data, sizeof(data), 0);
        }
        if (rcvd < 0 || errno != 0){
            sflag = 0;
            cout << "\npacket not sent :: bytes sent ::" << rcvd << ":: errno ::" << errno << endl;
        }else{
            cout << "\nsent " << rcvd << " bytes :: " << endl;
        }

        size = sizeof(struct sockaddr_in);
        if (sent > 1){
            rcvd = recv(socketid, &d2, sizeof(d2), 0);
            if (errno == 11){
                rcvd = recv(socketid, &d2, sizeof(d2), 0);
                cout << rcvd << " ";
                cout << errno << endl;
            }
            if (clock_gettime(CLOCK_MONOTONIC_RAW, &stop_time) == -1){
                cout << "error" << endl;
                break;
            }
            if (rcvd >= 0){
                if (d2.ttl > 0 && sflag){
                    cout << "\npacket recieve info::" << endl;
                    cout << "ipv4 ret size: " << (unsigned short)d2.header2 << endl;
                    cout << "ttl: " << (unsigned int)d2.ttl << endl;
                    cout << "protocol: " << (unsigned int)d2.protcol << endl;
                    cout << "ipv4 checksum: " << (unsigned short)d2.chekcsum << endl;
                    cout << "source ip :";
                    printip((unsigned int)d2.sourceip);
                    cout << "dest ip :";
                    printip((unsigned int)d2.destip);
                }else if(((packet *)&d2) -> type == 69 || ((packet *)&d2) -> type == 0){
                    cout << "recieve successful" << endl; 
                }
                else{
                    cout << "\nfailed to recieve" << endl;
                    loss ++;
                }
                cout << "\nRTT: " << double(stop_time.tv_nsec - start_time.tv_nsec)/1000000.0 + (stop_time.tv_sec - start_time.tv_sec) << " msec" << endl;
                
            }
            else{
                loss ++;
                cout << "\nfailed to recieve" << endl;
                cout << "SYSTEM TIMEOUT" << endl;
            }

        }
    }

    if (clock_gettime(CLOCK_MONOTONIC_RAW, &stp) == -1){
        cout << "error" << endl;
    }
    timetotal += double(stp.tv_nsec - stm.tv_nsec)/1e9 + (stp.tv_sec - stm.tv_sec);
    cout << "\n\npackets sent :: " << sent << " packets received ::" << sent - (loss) << endl;
    cout << "total loss :: " << float(loss) * 100/sent<< "%" << endl;
    cout << "total time :: " << timetotal << " seconds" << endl;
    close(socketid);
    return 0; 
}