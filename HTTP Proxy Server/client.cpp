#include "cache.h"


//int respond(int,Msg_pkt *);
//int read_unpack(int sd, Msg_pkt* incoming);

int main(int argc, char ** argv)
{
	int sd,server_port,temp,fdmax, r;
	int buf_len=0;
	char *server_ip, *url,buf[MAX_SIZE];
	struct sockaddr_in server_addr;
	struct hostent* hret;
	struct timeval tv;

	fd_set fd; 
	
	if(argc != 4)
	{
		printf("ERROR:- PLEASE use this format ./client <proxy address> <proxy port> <url to retrieve>/n");
		return -1;
	}
	

	/* Reading the command line input and storing it*/
	
	server_ip = argv[1];
	server_port = atoi(argv[2]);
	url = argv[3];
	
	/* creating the socket and binding it to the sever_ip provided*/
	
	
	Newconnect *connect2ser=new Newconnect(server_ip,server_port,sd);
	sd=connect2ser->create_newsd();
	/* Connecting to the server*/
	
	/* Create and send the get request msg*/					
	http_msg *request = new http_msg; 
	printf("URL YOU ENTERED IS :%s\n",url);
	request->create_msg(url,GET);	
	printf("GET_Request:\n%s\n",request->getreq);			
	temp=connect2ser->sendrq(sd,request->getreq,url,GETREQ);
        delete(request);
	if(temp<0)
	{	
		printf("Can't send the get request\n");
		return -1;
	}
	


	/* For I/O Multiplexing...*/

	FD_ZERO(&fd);  // Initiliaze the client reading socket list.	
	FD_SET(sd, &fd); // Include the client sd into the reading set.
	fdmax = sd +1;
	tv.tv_sec= 20;  // Set the timeout value for idle case
	tv.tv_usec =0;

	while(true)
	{	
		
		FD_ZERO(&fd);  // Initiliaze the client reading socket list.	
		FD_SET(0, &fd);	// STDIN
		FD_SET(sd, &fd); // Include the client sd into the reading set.
		fdmax = sd +1;
		
		temp = select(fdmax, &fd, NULL, NULL, &tv);
				
		if(temp == -1 )
		{
			printf("ERROR:- Select function failed\n");
			return -1;
		}
		/* Time out case - create and send IDLE packet */
		if(temp == 0)// && Isidle!= false)
		{	
			printf("No respond from the server\n");
			return 0;	
		}
		else
		{	
				
				memset(buf,'\0',MAX_SIZE);
				r = read(sd,buf,MAX_SIZE);
				if(r>0)
				{
				buf_len+=r;
				printf("%d bytes data recvd:\n\n%s\n",r,buf);
 					
			        }
				if(r==0)
				{
					printf("\n\nRecieving Data Successfully! Data size is: %d\n",buf_len);
					
					return 0;
				}
				
				
		}
	} // true loop
	
	delete connect2ser;
	close(sd);
	return 0;		 
}

/* Based on the Incoming message, the client will respond accordingly
* It have differenrt behaviour for different type of messages 	
*/

