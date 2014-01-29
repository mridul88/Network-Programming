#Enable g++ complier
CPP = g++
# Set Wall 
CPPFLAGS=-Wall -g
all: sbcpclient sbcpserver
sbcpclient: client.cpp client_list.h SBCP_packet.h def.h
	$(CPP) $(CPPFLAGS) client.cpp -o client

sbcpserver: server.cpp client_list.h SBCP_packet.h def.h
	$(CPP) $(CPPFLAGS) server.cpp -o server
clean:
	rm -rf *o client server
