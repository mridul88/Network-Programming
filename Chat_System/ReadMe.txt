#This is the sample chat server and client application based on Simple Brodcast Chat Protocol (SBCP) as a aprt of HW-1 of ECEN 602 class.
#Homework - 1 
#Group -  16.
@Author - Mridul Malpani and Qian Ning.

This application does:-

		A) Establishes one server, which will connect with all the users and cop-ordinate with them to chat.		
		B) One client can chat with all the other connected persons.Broadcast chatting. No one to one chatting.
		C) The client will be notified whenever a new user will join or will left the chat server. 
		D) The client also gets acknowledgment (positive or negative) about his request to join the server.

-------------------------------------------------------------------------------------------------------------------------------------------------

How to run this application:-
1)Unzip the folder and open the terminal.
2)Run makefile. Type:- make -f makefile.
3)To run server - open a new terminal. Type - ./server <ip_address> <port> <max_client>
4)To run client - open a new termianl. Type - ./client <username> <server_ip_address> <server_port>   

----------------------------------------------------------------------------------------------------------------------------------------------------

This application contains the following source files:-		
1)server.cpp
2)client.cpp
3)client_list.h
4)SBCP_packet.h
5)def.h   
6)makefile.

1)server.cpp - This is the main server file, which will establishes the server, connects with all the client and respond to their 
	       different messages accordingly. This file sends  messages to client and recieves messages from them.

2)client.cpp - This is the main client file, which will start a clinet program, connects with the server. Through server, the client will
	       chat with other users. This file create and sends messages to server and receives messages from the server. 

3)client_list.h - This is a header file, which defines a linked list type data structure and all its related functions which is used in the 
		  server.cpp by the server to maintain a list of client connected to it with all their information.
		  

4)SBCP_packet.h - This is a header file, which defines the data structure of the type of SBCP message and atribute and all its related functions . 
		  This packet structure is used in all over the program. This file also includes the pack and unpack functions which converts the
		  data structure into the char buffer strictly following the SBCP protocol for message passing.

5)def.h  - This file defines all the global values, MACROS, strings and the debugging helper functions, used throughout the program.  

6)makefile - This is a makefile. 
		 
			
