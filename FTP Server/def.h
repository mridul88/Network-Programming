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
#include<map>
#include<vector>

using namespace std;

#define DATASIZE 512
#define MAXSIZE 516
#define ERRSIZE 100


#define RRQ 1
#define WRQ 2
#define DATA 3
#define ACK 4
#define ERROR 5
#define uint16 unsigned short int
#define TRUE 1
#define FALSE 0


#define random() (rand()%1001+2000)

class Client_data
{
	public:	
	char file_name[20];
	int  block_no;
	struct sockaddr_in my_addr;
	FILE *fp, *fend;
	bool Islastsent;

	/*Constructor*/
	Client_data(char* file, struct sockaddr_in addr)
	{
		strcpy(file_name, file);
		block_no =0;
		my_addr = addr;
		Islastsent = FALSE;
	}
};

struct  error_pkt{
	uint16 opcode;
	uint16 errcode;
	char   errmsg[20];
	};
		
	
	
