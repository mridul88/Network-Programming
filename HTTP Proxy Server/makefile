#Enable g++ complier
CPP = g++
# Set Wall 
CPPFLAGS=-Wall -g
all: httpserver httpclient 

httpserver: server.cpp HTTP_packet.h connect.h cache.h def.h
	$(CPP) $(CPPFLAGS) server.cpp -o server

httpclient: client.cpp HTTP_packet.h connect.h cache.h def.h
	$(CPP) $(CPPFLAGS) client.cpp -o client

clean:
	rm -rf *o client server
