#Enable g++ complier
CPP = g++
# Set Wall 
CPPFLAGS=-Wall -g
all: tftpserver

tftpserver: server.cpp tftp_packet.h def.h
	$(CPP) $(CPPFLAGS) server.cpp -o server
clean:
	rm -rf *o client server
