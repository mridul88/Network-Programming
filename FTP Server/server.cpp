#include "tftp_packet.h"

int read_unpack(tftp_pkt* incoming);
int respond_send(int sd, tftp_pkt *incoming, map<int, Client_data *> &list);

int main(int argc,char **argv)
{
	int sd,new_sd,client_len,serverport,n,i,fdmax;
        char *serverip;
	short int temp_port;
	struct sockaddr_in server_addr,client_addr;
	struct hostent* hret;
	
        fd_set clientfds, master_set;  // set of all the active client sockets from which server can recieve data
        map<int, Client_data* > client_list;
 		
	
	if(argc < 3)
	{
		printf("Please input like this: ./server <server_ip> <server_port> \n");
		return -1;
	}
	
        serverip = argv[1];
	serverport = atoi(argv[2]);

	sd=socket(AF_INET,SOCK_DGRAM,0);
	if(sd<0)
	{
		printf("ERROR: when creat a socket\n");
		return -1;
	}
	
       
	memset(&server_addr,0,sizeof(server_addr));
	server_addr.sin_family=AF_INET;
	server_addr.sin_port=htons(serverport);
	hret = gethostbyname(serverip); 
	memcpy(&server_addr.sin_addr.s_addr, hret->h_addr, hret->h_length); 
 	
	if(bind(sd,(struct sockaddr *)&server_addr,sizeof(server_addr))==-1)
	{
		printf("ERROR:Can't bind name to socket\n");
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
		
			if(FD_ISSET(i, &clientfds))   // Some one is trying to connect to the server
			{
				
				tftp_pkt *incoming = new tftp_pkt;

				/*A new client sends RRQ */
				if(i == sd) 					
				{	
		                 
					//printf("RRQ_recieved\n");
		 			client_len=sizeof(client_addr);
					n=recvfrom(sd,incoming->rcvd,MAXSIZE,0,(struct sockaddr *)&client_addr,(socklen_t*)&client_len);
               				if(n == -1)
					{
        					printf("ERROR: when receive from client!\n");
						return -1;
					}
                			//printf("recieved\n");
					read_unpack(incoming);

					/* Create a new socket tid, for this client*/
					if(incoming->opcode == RRQ)
					{
						new_sd=socket(AF_INET,SOCK_DGRAM,0);

						if(new_sd<0)
						{
							printf("ERROR: when creat a socket\n");
							return -1;
						}
				
						FD_SET(new_sd, &master_set);
				         	if(new_sd >= fdmax)
							fdmax = new_sd; 
	        	       
                				temp_port=random();   
						printf("TID=%d\n",temp_port);     
						memset(&server_addr,0,sizeof(server_addr));
						server_addr.sin_family=AF_INET;
						server_addr.sin_port=htons(temp_port);
						server_addr.sin_addr.s_addr=htonl(INADDR_ANY);
	                
                				if(bind(new_sd,(struct sockaddr *)&server_addr,sizeof(server_addr))==-1)
						{
							printf("ERROR:Can't bind name to socket\n");
							return 1;
						}

						Client_data *new_client = new Client_data(incoming->file_name, client_addr);
						client_list[new_sd] = new_client;
						//printf("till here..");
						if(respond_send(new_sd,incoming, client_list)==1)
						{       
                                                        printf("Cannot find the file name\n");
							close(new_sd);
							FD_CLR(new_sd, &master_set); 

							//return -1;
						}
								
						
					}
				}//i==sd
				
				/* some Ack or error is coming from the client*/
				else
				{
					
					//printf("ACK_recieved\n");
		 			client_len=sizeof(client_addr);
					n=recvfrom(i,incoming->rcvd,DATASIZE,0,(struct sockaddr *)&client_addr,(socklen_t*)&client_len);
               				if(n <= 0)
					{
						return -1;
					}
                			//printf("recieved\n");

					read_unpack(incoming);
					respond_send(i,incoming, client_list);
				}
				
				delete(incoming);
			}
		}//closing for loop		
	}// forever loop
	
	close(sd);
	printf("Sever Shutting Down");
	return 0;
}


/* Based on the Incoming message, the server will respond accordingly
* It have differenrt behaviour for different type of messages 	
*/

int respond_send(int sd, tftp_pkt *incoming, map<int, Client_data *> &list)
{
 	FILE *fp, *fend;
	map<int, Client_data *>::iterator it; 
	long int len;
	char errbuf[ERRSIZE];
	socklen_t client_len=sizeof(struct sockaddr_in);
	
	switch(incoming->opcode)
	{
		/*Open the file if exists, and send first DATA packet*/
		case RRQ: 
		{
			
			printf("\nRRQ request for %s file\n",incoming->file_name);
		        printf("filename:%s\n",incoming->file_name);
  			fp = fopen(incoming->file_name, "r");
			fend = fopen(incoming->file_name, "r");
			if (fp==NULL || fend == NULL) 
			{
				printf("Error in opening file\n"); //we need to creat a Error message;
 				/* Creat ErrPkt*/	
				struct error_pkt error1;
				error1.opcode=ERROR;
				error1.errcode=1;
				
				strncpy(error1.errmsg,"File not Found",strlen("File not Found"));

				memset(errbuf,'\0',ERRSIZE);	
				error1.opcode = htons(error1.opcode);
				memcpy(errbuf, &error1.opcode,2);	
				error1.errcode = htons(error1.errcode);
				memcpy(errbuf+2, &error1.errcode,2);
				memcpy(errbuf+4,error1.errmsg,sizeof(error1.errmsg));

			        if(sendto(sd,errbuf,ERRSIZE,0,(struct sockaddr *)&list[sd]->my_addr, client_len)!=-1)
				{	
				printf("send a Error message to client\n\n");
				}                                
				return 1; 
				break;
			}
			
			
	                else
			{
			printf("\nOpened File - %s", incoming->file_name);
			/* Create outgoing packet*/
			tftp_pkt *outgoing = new tftp_pkt(DATA);
			outgoing->block_no = 1;		
			
			fseek(fend, 0, SEEK_END);
			list[sd]->fend = fend;
			len = ftell(fend) - ftell(fp);
			//printf("%ld  %ld  %ld\n", ftell(fend), ftell(fp), len); 
			if(len >= DATASIZE)
			{
				len = DATASIZE;
			}
			else if(len < DATASIZE)
			{
				list[sd]->Islastsent = TRUE;
			}						
			
			outgoing->make_data_pkt(len,&fp);

			list[sd]->fp = fp; // updating the file pointer		
										
			sendto(sd,outgoing->buf,len+4,0,(struct sockaddr *)&list[sd]->my_addr, client_len);
			printf("\nsend data of len - %ld", len);
			
			delete(outgoing);
			}				// deleteing the outgoing message
			break;
			
		}    
		/*Now the server will send the next block of the data*/
		case ACK: 
		{
			list[sd]->block_no = incoming->block_no;	
			fp = list[sd]->fp;
			fend = list[sd]->fend;
			
			printf("\nRecieved ACk for this Block # %d\n", incoming->block_no);
			
			/* Create outgoing packet*/
			tftp_pkt *outgoing = new tftp_pkt(DATA);
			outgoing->block_no = list[sd]->block_no +1 ;	
			
			len = ftell(fend) - ftell(fp);
			//printf("%ld  %ld  %ld\n", ftell(fend), ftell(fp), len); 
			if(len >= DATASIZE)
			{
				len = DATASIZE;	
			}
			/* Received acknowledgment for the last packet*/
			else if(len == 0 && list[sd]->Islastsent == TRUE)
			{
				write(1,"Completed",9);				
				fclose(fp);		// close the file and delete the client details
				it= list.find(sd);
  				list.erase (it);
				break;
			}
			else if(len < DATASIZE)
			{
				list[sd]->Islastsent = TRUE;
			}
					
			outgoing->make_data_pkt(len,&fp);
			list[sd]->fp = fp; // updating the file pointer	
			
			sendto(sd,outgoing->buf,len+4,0,(struct sockaddr *)&list[sd]->my_addr, client_len);
			printf("\nsend data of len - %ld", len);
			
			delete(outgoing);
			break;
		}
		
		default:
		{	
			return -1;
		}
	}
	return 0;
}




/* It will read the incoming TFTP packets, coming from the clients,
*  From the char buffer (the input packet), it will re-construct the datastructure
*  (unpack the packet) which is used in the program.
*/

int read_unpack(tftp_pkt* incoming)
{
	uint16 * temp,len =0, i=0, j=0; //unit16 is unsigned short;	
	char c[2];

	memset(c,'\0',2);
	memcpy(c,incoming->rcvd,2);
	temp = (uint16 *)c;
	incoming->opcode = ntohs(*temp);
	len+=2;

		
	/* Reading Acknowledgment messages*/
	if(incoming->opcode == ACK)
	{
		memset(c,'\0',2);
		memcpy(c,incoming->rcvd+len,2);
		temp = (uint16 *)c;
		incoming->block_no = ntohs(*temp);
		len+=2;
		//printf("\n-- %u --\n", incoming->block_no);
	}
	
	/* Reading RRQ Request messages*/
	else if(incoming->opcode == RRQ)
	{
		for(i=2, j=0; incoming->rcvd[i]!='\0'; i++, j++)
		incoming->file_name[j] = incoming->rcvd[i];
		incoming->file_name[j] = '\0';   //last character of the string
	

		printf("%s\n",incoming->file_name);
			
		
	}
		
	/* Reading Error messages*/
	else if(incoming->opcode == ERROR)
	{
		memset(c,'\0',2);
		memcpy(c,incoming->rcvd+len,2);
		temp = (uint16 *)c;
		incoming->err_code = ntohs(*temp);
		len+=2;
		
		
			
	}
	return 1;
}	









	
