#include "client_list.h"

int respond(int,Msg_pkt *);
int read_unpack(int sd, Msg_pkt* incoming);

int main(int argc, char ** argv)
{
	int sd,server_port,temp,fdmax, r;
	bool Isidle = false;
	char *server_ip, *username, buf[MAX_SIZE];
	struct sockaddr_in server_addr;
	struct hostent* hret;
	struct timeval tv;

	fd_set fd; 
	Greeting g;   // contains all the constant strings 

	if(argc != 4)
	{
		printf("ERROR:- PLEASE use this format <./client username server_ip server_port>/n");
		return -1;
	}
	

	/* Reading the command line input and storing it*/
	username = argv[1];
	server_ip = argv[2];
	server_port = atoi(argv[3]);	
	
	/* creating the socket and binding it to the sever_ip provided*/
	sd = socket(AF_INET, SOCK_STREAM, 0);
	if(sd == -1)
	{
		printf("ERROR:- Unable to create a socket\n");
		return -1;
	}
	
	bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(server_port);     // convert it into network byte order
	hret = gethostbyname(server_ip); 
	memcpy(&server_addr.sin_addr.s_addr, hret->h_addr, hret->h_length); 

	/* Connecting to the server*/
	if(-1 == connect(sd, (struct sockaddr *)&server_addr, sizeof(server_addr)) )
	{
		printf("ERROR:- Unable to connect to the server %s\n", server_ip);
		return -1;
	}
	
	printf(" sd = %d Yeah...!! Connected to the server with address %s\n",sd, server_ip);
	
	/* Create and send the JOIN packet*/					
	Attr_pkt *a_outgoing = new Attr_pkt;
	a_outgoing->create_attr_pkt(ATT_UNAME,argv[1]);  //create attr;				
		
	Msg_pkt * outgoing = new Msg_pkt;
	outgoing->attr.push_back(a_outgoing);       // attach to msg pkt
	outgoing->create_msg_pkt(JOIN);		    // msg_pkt		
	outgoing->pack_pkt(); 			    // pack it and convert structure into char *

	temp = write(sd,outgoing->buf_send,MAX_SIZE);
	
	delete(a_outgoing);				// deleteing the outgoing messages
	delete(outgoing);
	
	
	if( -1 == temp )  // send message
	{
		printf("send failed\n");
		return -1;
	}
	
	/* For I/O Multiplexing...*/

	FD_ZERO(&fd);  // Initiliaze the client reading socket list.	
	FD_SET(0, &fd);	// STDIN
	FD_SET(sd, &fd); // Include the client sd into the reading set.
	fdmax = sd +1;

	tv.tv_sec= 10;  // Set the timeout value for idle case
	tv.tv_usec =0;

	while(true)
	{	
		Isidle = true;
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
			//write(1,"dadd\n",5);	
			/*Msg_pkt *outgoing = new Msg_pkt;
			outgoing->create_msg_pkt(IDLE);		    // msg_pkt	
			outgoing->pack_pkt(); 			    // pack it and convert structure into char *
						
			write(sd,outgoing->buf_send, MAX_SIZE);
			//write(1, outgoing->buf_send, outgoing->len);	
			
			delete(outgoing);*/	
		}
		else
		{	/* If something there is to read at sd, coming from the server*/		
			if(FD_ISSET(sd, &fd))  
			{
				/* Some data from server has come*/ 
				
				Msg_pkt *incoming = new Msg_pkt;	
				r= read_unpack(sd,incoming);
				if(r == -1)
				{
					write(1, g.quit, strlen(g.quit));
					close(sd);
					FD_CLR(sd, &fd);
					exit(1);
				}
	
				respond(sd,incoming);
				
				/* delete the incoming packet after responding*/
				for(unsigned int j=0;j<incoming->attr.size(); j++)
				{
					delete(incoming->attr[j]);  //delete the attrbute
				}
				delete(incoming);
			}
			/* If something there is to read at sd, coming from the keyboard - std in*/	
			else if(FD_ISSET(STDIN, &fd))
			{
				/* Start chatting..*/
				Isidle = false;
				
				memset(buf,'\0', sizeof(buf));
				read(0, buf,sizeof(buf));  // Will read from the std.input
						
				Attr_pkt *a_outgoing = new Attr_pkt;
				a_outgoing->create_attr_pkt(ATT_MSG,buf);  //create attr;
				
				Msg_pkt *outgoing = new Msg_pkt;
				outgoing->attr.push_back(a_outgoing);       // attach to msg pkt
				outgoing->create_msg_pkt(SEND);		    // msg_pkt	
				outgoing->pack_pkt(); 			    // pack it and convert structure into char *
						
				write(sd,outgoing->buf_send, MAX_SIZE);
			
				delete(a_outgoing);
				delete(outgoing);
			}
		}
	} // true loop
	
	close(sd);
	return 0;		 
}

/* Based on the Incoming message, the client will respond accordingly
* It have differenrt behaviour for different type of messages 	
*/
int respond(int sd, Msg_pkt *incoming)
{	
	Greeting g;   // contains all the constant strings 
	
	switch(incoming->type)
	{
		case ACK: 
		{
			print_enter();			
			write(1, g.ack, strlen(g.ack));
			write(1,incoming->attr[0]->info, (incoming->attr[0]->len-4));
			print_enter();
			break;
		}
		/* Forward the message to everyone*/
		case NAK: 
		{			
			if(incoming->attr[0]->type == ATT_REASON)			
				write(1, g.reason, strlen(g.reason));
			
			return -1;
		}
		/* Create and broadcast IDLE*/
		case FWD: 
		{
			write(1, g.fwd, strlen(g.fwd));
			//write(1,incoming->attr[1]->info,((incoming->attr[1]->len)-4));
			print_enter();
			write(1,incoming->attr[0]->info,((incoming->attr[0]->len)-4));
			break;
		}
		case ONLINE:
		{
			write(1,incoming->attr[0]->info,(incoming->attr[0]->len-4));
			write(1, g.online, strlen(g.online));
			break;
		}
		case OFFLINE:
		{
			write(1,incoming->attr[0]->info,(incoming->attr[0]->len-4));
			write(1, g.offline, strlen(g.offline));
			break;
		}
		case IDLE:
		{
			write(1,incoming->attr[0]->info,(incoming->attr[0]->len-4));
			write(1, g.idle, strlen(g.idle));
			break;
		}
		default:
		{		
			break; //return -1;
		}
	}
	return 0;
}

/* It will read the SBCP packet (formed with all the rules of the protocol),
*  From the char buffer (the input packet), it will re-construct the datastructure
*  (unpack the packet) which is used in the program.
*/	

int read_unpack(int sd, Msg_pkt* incoming)
{
	unsigned long int len =4,hdr, *t;
	int nbytes;
	char input[MAX_SIZE], h[4];
	
	len = sizeof(unsigned int);
	nbytes = read(sd,input, MAX_SIZE); // reading message header
	

	memcpy(h, input, len);
	t = (unsigned long int *)h;
	hdr = ntohl(*t);

	if(nbytes == -1)
	{
		return -1;
	}
	/* Server has closed the connection */
	else if(0 == nbytes)
	{
		return 0;
	}

	/* Read data and construct the struct here*/
	else
	{
		incoming->unpack_msg_pkt(hdr);	// From hdr it will extract type, version and len of the packet.			
		
		/* true, if there is some attributes*/
		while(incoming->len > len)
		{
			Attr_pkt *a = new Attr_pkt;
			memcpy(h, input+len, 4);
			t = (unsigned long int *)h;
			hdr = ntohl(*t);
			len+=4;
						
			a->unpack_attr_pkt(hdr);
			
			memcpy(a->info, input+len, (a->len -4));
			len+= (a->len -4);
			
			incoming->attr.push_back(a);  //linking attr to the msg_packet
		}
	}
	return 1;
}	
			
			
