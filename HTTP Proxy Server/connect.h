#define CONNECT_H

#include "HTTP_packet.h"


//create a new connect between proxy server and web server

class Newconnect

{	
	private:
		struct sockaddr_in server_addr;
		struct hostent* hret;
		int rc1,rc2,sd;

	public:
		int csd;
		char buf[MAX_SIZE];
		int buf_len;
		char url[300];
		char expire[50];
		char modify[50];
		int req_type;
		Newconnect(char *hostname,int server_port,int client_sd)
		{       
			req_type=-1;
			memset(buf,'\0',MAX_SIZE);
			csd=client_sd;
			buf_len=0;
			rc1=-1;
			rc2=-1;
			bzero(&server_addr, sizeof(server_addr));
			server_addr.sin_family = AF_INET;
			server_addr.sin_port = htons(server_port);     // convert it into network byte order
			hret = gethostbyname(hostname); 
			memcpy(&server_addr.sin_addr.s_addr, hret->h_addr, hret->h_length); 
			memset(expire,'\0',50);
			memset(modify,'\0',50);
			memset(url,'\0',300);
			
		}

		int create_newsd()
		{
			sd = socket(AF_INET, SOCK_STREAM, 0);
			if(sd == -1)
			{
				printf("ERROR:- Unable to create a socket\n");
				return -1;
			}
			return sd;
		}
		int sendrq(int newsd,char *msg,char *url1,int type)
		{       
			req_type=type;
						
			strncpy(url,url1,strlen(url1));
			printf("URL------%s\n",url1);
                        rc1=connect(newsd, (struct sockaddr *)&server_addr, sizeof(server_addr));
			if(rc1<0)
			{	
				printf("Cannot connect to the server\n");
				return -1;
			}
			
			printf("Socket %d Connected to the server with proxy server %s\n",newsd, inet_ntoa(server_addr.sin_addr));
			rc2 = write(newsd,msg,strlen(msg));
			if(rc2<0)
			{	
				
				printf("Cannot send req to the server\n");
				return -1;
			}
			printf("SEND THE FOLLOWING REQUEST TO SERVER:\n%s\n",msg);
			return 1;
		}
		
};
