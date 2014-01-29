#include "client_list.h"

int respond(int,Msg_pkt *, List *);
int read_unpack(int sd, Msg_pkt* incoming);

int main(int argc, char ** argv)
{
	int server_port, max_client, new_sd= -1;
	int sd,i, fdmax =0,r=-1, len=0;
	unsigned int j;
	char *server_ip, buf[50];
	struct sockaddr_in server_addr, client_addr;
	struct hostent* hret;
	

	
	List client_list ;  // linked list - the server will maintain the list of the clients */
	fd_set clientfds, master_set;  // set of all the active client sockets from which server can recieve data
	Greeting g;   // contains all the constant strings 

	if(argc != 4)
	{
		printf("ERROR:- PLEASE use this format <./server server_ip server_port max_clents>/n");
		return -1;
	}
	
	/* Reading the command line input and storing it*/
	server_ip = argv[1];
	server_port = atoi(argv[2]);
	max_client = atoi(argv[3]);

	printf("IP = %s, port = %d, max = %d\n", server_ip, server_port, max_client); 		
	
	/* creating the socket and binding it to the sever_ip provided*/

	sd = socket(AF_INET, SOCK_STREAM, 0);   // sd is the listening socket
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
	
		
	if (-1 == bind(sd, (struct sockaddr *)&server_addr, sizeof(server_addr)) )
	{
		printf("ERROR: Unable to bind the socket to this ip = %s and port = %d\n", server_ip, server_port);
		return -1;
	}

	/* Max queue of clients request as per user input*/
	
	if( -1 == listen(sd, max_client))
	{
		printf("ERROR:- Request queue at server is above the allowed limit. Wait and try again. \n");
		return -1;
	}

	FD_ZERO(&clientfds);  // Initiliaze the client reading socket list.
	FD_ZERO(&master_set);
	FD_SET(sd, &master_set); // Include the server sd into the reading set.
	fdmax = sd;

      	/* Server will do I/O mulitplexing using select and will continously listen */
	
	while(true)
	{	
		clientfds = master_set;		

		if( -1 == select(fdmax+1, &clientfds, NULL, NULL, NULL))
		{
			printf("ERROR:- Select function failed\n");
			return -1;
		}
	
		for(i=0; i<= fdmax; i++)
		{
		
			if(FD_ISSET(i, &clientfds))   // Some one is trying to connect to the socket i or sending some data on this socket descriptor - 'i'
			{
				/*A new client wants to connect to the server through the listening socket */
				if(i == sd) 					
				{	
					len = sizeof(client_addr);
					new_sd = accept(sd, (struct sockaddr *)&client_addr, (socklen_t *)&len);
					if(new_sd == -1)
					{
						perror("ERROR:- Unable to connect to client\n");
					}	
					else
					{	
					 	client_list.addclient(new_sd);
						
						FD_SET(new_sd, &master_set);
				         	if(new_sd >= fdmax)
							fdmax = new_sd; 
					}


				}
				/* Some data is there to read at socket descriptor 'i' */ 
				/* Important - read and write should be made on same sd*/
				else 	
				{
					Msg_pkt *incoming = new Msg_pkt;					
					r = read_unpack(i, incoming);
					
					if(r == -1)
					{
						printf("Error: - Reading failed\n");
						return -1;
					}	
					/* Client has closed the connection */
					else if(0 == r)
					{
						/* Create and send OFFLINE*/
						memset(buf,'\0',50);
						client_list.getname(buf,i);

						write(1,buf,strlen(buf));
						write(1,g.closed, strlen(g.closed));					

						Attr_pkt *a_outgoing = new Attr_pkt;
						a_outgoing->create_attr_pkt(ATT_UNAME,buf);  //create attr;
				
						Msg_pkt *outgoing = new Msg_pkt;
						outgoing->attr.push_back(a_outgoing);       // attach to msg pkt
						outgoing->create_msg_pkt(OFFLINE);		    // msg_pkt	
						outgoing->pack_pkt(); 			    // pack it and convert structure into char *
						client_list.broadcast(outgoing->buf_send,i);         // send the char array.
						delete(a_outgoing);				// deleteing the outgoing messages
						delete(outgoing);

						client_list.removeclient(i);
						close(i);
						FD_CLR(i, &master_set);
					}
					/* There is some readable data from the active client*/
					else
					{
						respond(i,incoming,&client_list);
						
						/* delete the incoming packet after responding*/
						for(j=0;j<incoming->attr.size(); j++)
						{
							delete(incoming->attr[j]);  //delete the attrbute
						}
						delete(incoming);
					}	 
				}

			} /* if fd is set*/
		}
	}//always true
	
	close(sd);
	return 0;
		
}

/* Based on the Incoming message, the server will respond accordingly
* It have differenrt behaviour for different type of messages 	
*/

int respond(int sd, Msg_pkt *incoming, List * client_list)
{
	int r,i;	
	char buf[512];
	Greeting g;   // contains all the constant strings 
	
	Msg_pkt *outgoing = new Msg_pkt;
	
	switch(incoming->type)
	{
		/* Storing the username in the list and will send the acknowledment*/
		case JOIN: 
		{
  			memset(buf, 0, sizeof(buf));
			strncpy(buf,incoming->attr[0]->info, incoming->attr[0]->len-4);
			r = client_list->updateclient(sd,incoming->attr[0]->info, ((incoming->attr[0]->len)-4));
			
			write(1, g.join, strlen(g.join));
			write(1,buf,strlen(buf));
			print_enter();

			Attr_pkt *a_outgoing = new Attr_pkt;

			/* Create and send NAK*/
			if(r == -1) 
			{
				memset(buf, 0, sizeof(buf));
				strcpy(buf,g.reason);
				
				a_outgoing->create_attr_pkt(ATT_REASON,buf);  //create attr;
		
				outgoing->attr.push_back(a_outgoing);       // attach to msg pkt
				outgoing->create_msg_pkt(NAK);		    // msg_pkt	
				outgoing->pack_pkt(); 			    // pack it and convert structure into char *
	
				write(sd,outgoing->buf_send,  MAX_SIZE);	
			}
			else	
			{
				/* Create and send ACK */
				memset(buf, 0, sizeof(buf));
				int2char(buf,client_list->no_client);
					
				a_outgoing->create_attr_pkt(ATT_CCOUNT,buf);  //create attr;
		
				outgoing->attr.push_back(a_outgoing);       // attach to msg pkt
				outgoing->create_msg_pkt(ACK);		    // msg_pkt	
				outgoing->pack_pkt(); 			    // pack it and convert structure into char *
	
				write(sd,outgoing->buf_send,  MAX_SIZE);	

				/* Create and broadcast ONLINE*/
				a_outgoing->create_attr_pkt(ATT_UNAME,incoming->attr[0]->info); //create attr;
				outgoing->attr.push_back(a_outgoing);       // attach to msg pkt
				outgoing->create_msg_pkt(ONLINE);		// msg_pkt	
				outgoing->pack_pkt(); 			    // pack it and convert structure into char *
				
				client_list->broadcast(outgoing->buf_send,sd);
				
			}
			delete(a_outgoing);				// deleteing the outgoing message
			break;
		}
		/* Forward the message to everyone*/
		case SEND: 
		{
  			memset(buf, 0, sizeof(buf));
			strncpy(buf,incoming->attr[0]->info, incoming->attr[0]->len-4);
			
			write(1, g.send, strlen(g.send));
			write(1,incoming->attr[0]->info, incoming->attr[0]->len-4);		

			Attr_pkt *a_outgoing = new Attr_pkt;
			a_outgoing->create_attr_pkt(ATT_MSG,incoming->attr[0]->info);  //create attr;

			memset(buf, 0, sizeof(buf));
			client_list->getname(buf,i);
			Attr_pkt *b_outgoing = new Attr_pkt;
			b_outgoing->create_attr_pkt(ATT_UNAME,buf);  //create second attr;
			
			outgoing->attr.push_back(a_outgoing);       // attach to msg pkt
			outgoing->attr.push_back(b_outgoing);
			outgoing->create_msg_pkt(FWD);		// msg_pkt	
			outgoing->pack_pkt(); 			    // pack it and convert structure into char *
				
			client_list->broadcast(outgoing->buf_send,sd);			
			
			delete(a_outgoing);
			delete(b_outgoing);
			break;
		}
		/* Create and broadcast IDLE*/
		case IDLE: 
		{
  			memset(buf, 0, sizeof(buf));
			strncpy(buf,incoming->attr[0]->info, incoming->attr[0]->len-4);
			
			write(1,buf,strlen(buf));
			write(1, g.idle, strlen(g.idle));

			Attr_pkt *a_outgoing = new Attr_pkt;
			a_outgoing->create_attr_pkt(ATT_UNAME,buf);  //create attr;
		
			outgoing->attr.push_back(a_outgoing);       // attach to msg pkt
			outgoing->create_msg_pkt(IDLE);		// msg_pkt	
			outgoing->pack_pkt(); 			    // pack it and convert structure into char *
				
			client_list->broadcast(outgoing->buf_send,sd);			
			delete(a_outgoing);
			break;
		}
		default:
		{	
			return -1;
		}
	}

	delete(outgoing);
	return 0;
}

/* It will read the SBCP packet (formed with all the rules of the protocol),
*  From the char buffer (the input packet), it will re-construct the datastructure
*  (unpack the packet) which is used in the program.
*/

int read_unpack(int sd, Msg_pkt* incoming)
{
	unsigned long int len,hdr, *t;
	int nbytes;
	char input[MAX_SIZE], h[4];
	
	len = sizeof(unsigned int);
	nbytes = read(sd,input,  MAX_SIZE);	// reading message header
	
	len =4;
	memcpy(h, input, len);
	t = (unsigned long int *)h;
	hdr = ntohl(*t);	

	if(nbytes == -1)
	{
		return -1;
	}
	/* Client has closed the connection */
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
				
			a->unpack_attr_pkt(hdr);  // From hdr it will extract type, version and len of the packet.
			
			memcpy(a->info, input+len, a->len-4);
			len+= a->len -4;
			
			incoming->attr.push_back(a);  //linking attr to the msg_packet
			
		}
	}
	return 1;
}	
			
			


			
			
		


