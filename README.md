# PingAServer
to learn how to run, run the command :: ./ping -h


 Author: Vishnu Banna
"email:  vbanna@purdue.edu"

"How to Run:" << endl; 

" if you use the Makefile to compile::"

" > make ping" << endl;

"\n running ::" << endl;

" > sudo ./ping [ipv4 ip or address] [ttl]" 

"\n if no ttl is provided, it defaults to 64" 


"NOTE:: This tool was built and tested on Linux 18.04 LTS"

"NOTE:: I collased the header file into ping.cpp for convenience"

"NOTE:: I noticed, that the return code from when a packet was sent is 69" 

  "This is is a restricted type for icmp packets " 
  
  "This is because the erver reviece sends an ipv4 packet, which is different from the icmp packet" 
  
  "I fixed this by creating a new container structure to read back the packet that was sent" 
  
