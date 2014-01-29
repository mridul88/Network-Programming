
#This is the sample HTTP Proxy server and a client based on HTTP-1.0 protocol as a part of HW-3 of ECEN 602 class.
#Homework - 3
#Group -  16.
@Author - Qian Ning and Mridul Malpani.

This application does:-

		A) Establishes one server, which will connect with all the clients and fecth the requested website.		
		B) It supports the cache and also chekc the expiry time of cache.
		C) Clients are able to send HTTP request to the proxy server. 
		D) It is based on HTTP -1.0 protocol and client should know the URL.

-------------------------------------------------------------------------------------------------------------------------------------------------

How to run this application:-

1)Unzip the folder and open the terminal.
2)Run makefile. Type:- make -f makefile.
3)To run server - open a new terminal. Type - ./server <ip_address> <port> <max_client>
4)To run client - open a new terminae. Type - ./client <server_ip> <server_port> <URL or address> 

----------------------------------------------------------------------------------------------------------------------------------------------------

This application contains the following source files:-		
1)server.cpp
2)HTTP_packet.h
3)def.h  
4)client.cpp
5)connect.h
6)cache.h 
7)makefile.

1)server.cpp - This is the main server file, which will establishes the proxy server, connects with all the client and respond 		       to their request accordingly, fetch url or get it from cache and send it back.It uses HTTP-1.0 protocol.

2)HTTP_packet.h - This is a header file, which defines the data structure of the type of HTTP message and atribute and all its 			  related functions . This packet structure is used in all over the program. This file include the creatae packet 
		 function which converts the data structure into the char buffer strictly following the HTTP protocol for message.

3)def.h  - This file defines all the global values, MACROS, strings and the debugging helper functions, used throughout the program.  

4)client.cpp - This is the client file, which will establishes a connection with the HTTP proxy server.

5)connect.h - 

6)cache.h - 

7)makefile - This is a makefile. 
		 
			
