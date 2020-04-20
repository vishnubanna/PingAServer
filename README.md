# PingAServer
 Author: Vishnu Banna
"email:  vbanna@purdue.edu" \n
"How to Run:" << endl; \n

" if you use the Makefile to compile::" \n
" > make ping" << endl;\n
"\n running ::" << endl;\n
" > sudo ./ping [ipv4 ip or address] [ttl]" \n
"\n if no ttl is provided, it defaults to 64" \n

"NOTE:: This tool was built and tested on Linux 18.04 LTS"\n
"NOTE:: I collased the header file into ping.cpp for convenience"\n
"NOTE:: I noticed, that the return code from when a packet was sent is 69" \n
  "This is is a restricted type for icmp packets " \n
  "This is because the erver reviece sends an ipv4 packet, which is different from the icmp packet" \n
  "I fixed this by creating a new container structure to read back the packet that was sent" \n
