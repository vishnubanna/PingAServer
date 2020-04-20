WARNING = -Wall -Wshadow --pedantic
ERROR = -Wvla 
GCC = g++ -std=c++17 -g $(WARNING) $(ERROR)
#GCC = gcc -std=c99 -g $(WARNING) $(ERROR)
VALGRIND = valgrind --tool=memcheck --leak-check=full --verbose
TESTFLAGS =  

SRCS = ping.cpp
OBJS = $(SRCS:%.c=%.o)

ping: $(OBJS)
	$(GCC) $(TESTFLAGS) $(OBJS) -o ping

clean:
	rm ping

test1:
	make && sudo ./ping www.google.com 5

test2:
	make && sudo ./ping vishnubanna.github.io/

test3:
	make && sudo ./ping www.linkedin.com
	