
#This is the sample TFTP server based on Trivial file transfer protocol (RFC 1350) as a part of HW-2 of ECEN 602 class.
#Homework - 2
#Group -  16.
@Author - Mridul Malpani and Qian Ning.

This application does:-

		A) Establishes one server, which will connect with all the clients and transfer the file.		
		B) It supports only the read request (RRQ).
		C) Clients are not able to write the file available on server, but can read them.
		D) It is based on UDP protocol and client should know the complete file name with path.

-------------------------------------------------------------------------------------------------------------------------------------------------

How to run this application:-
1)Unzip the folder and open the terminal.
2)Run makefile. Type:- make -f makefile.
3)To run server - open a new terminal. Type - ./server <ip_address> <port> 

----------------------------------------------------------------------------------------------------------------------------------------------------

This application contains the following source files:-		
1)server.cpp
2)tftp_packet.h
3)def.h   
4)makefile.

1)server.cpp - This is the main server file, which will establishes the server, connects with all the client and respond to their 
	       request accordingly, send DATA and recieve ACK from clients. It uses stop and wait ARQ protocol.

2)tftp_packet.h - This is a header file, which defines the data structure of the type of SBCP message and atribute and all its 			  related functions . This packet structure is used in all over the program. This file include the creatae packet 
		 function which converts the data structure into the char buffer strictly following the TFTP protocol for message.

5)def.h  - This file defines all the global values, MACROS, strings and the debugging helper functions, used throughout the program.  

6)makefile - This is a makefile. 
		 
			
