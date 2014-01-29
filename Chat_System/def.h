/*This file contains all the global defintions and 
* constant strings..
*/

#define DEF_H

#include<cstdio>
#include<iostream>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<cstring>
#include<arpa/inet.h>
#include<netdb.h>
#include<sstream>
#include<string>
#include<vector>

/* Defining global variables*/
#define JOIN 	2
#define FWD 	3
#define SEND 	4
#define NAK	5
#define OFFLINE 6
#define ACK	7
#define ONLINE	8
#define IDLE	9
#define ATT_REASON 1
#define ATT_CCOUNT 3		
#define ATT_UNAME  2
#define ATT_MSG    4
#define MAX_SIZE  2048
#define STDIN 0

using namespace std;

class Greeting
{
	public:
	static char* closed;
	static char* ack; 
	static char* reason; 
	static char* recd; 
	static char* wrongtype; 
	static char* fwd;  
	static char* join;	
	static char* send;
	static char*quit;
	static char*online;
	static char*offline;
	static char*idle;
};

char* Greeting::closed = " - has gone offline. Cleaning up the server.\n";
char* Greeting::ack = "Rcvd ACK from server. \n---------Feeling Bored...Welcome to the Free Chat Server-----------\n No. of Client = ";
char* Greeting::reason = "NAK: This username is already in use. So, Try again with some other username\nClosing Connection..";
char* Greeting::recd = "Recieved the following message from client:- \n";
char* Greeting::wrongtype = "Client has send the message using wrong type.\n";
char* Greeting::fwd = " FWD from other client - ";
char* Greeting::join =" Rcvd JOIN request from - ";
char* Greeting::send ="Rcvd a SEND msg - ";
char* Greeting::quit = "Closing Chat. Thank you for your interset\n";
char* Greeting::online = " is now online.\n";
char* Greeting::offline = " went offline.\n";
char* Greeting::idle =" is idle";

void int2char(char *cstr, unsigned int num)
{
	stringstream ss;	
	ss << num;
	string str = ss.str();
	strcpy(cstr, str.c_str());
}

void print_enter()
{
	write(1,"\n", 1);
}

/*int print(int num)
{
	char temp[10];		
	write(1,"value- ",7);
	memset(temp,0,10);
	int2char(temp, num);
	write(1,temp,8);
	write(1,"\n",1);
	return 0;
}

int print(char *buf, int n)
{
	write(1,buf,n);
	print_enter();
	return 0;
}*/
