#include "SBCP_packet.h"

/* This class - a Linked List 
*  is used by the server to store the details of
*  every user (client), which are connected to server
*/

class List;

class node
{
	char u_name[16];
	int csd;
	struct node *next;
	
	friend class List;
};

class List
{
	private:
		class node *start, *last;
		
		
	public:
		int no_client;
		List()
		{
			start = NULL;
			last = NULL;
			no_client =0;
		}
		
		/* Destructor*/
		~List()
		{
			node* temp;  			
			if (start == NULL)
        			return;
 
  			while(start != NULL)
  			{
       				temp = start->next ;
     				delete temp;
       				start = temp;
  			}
		}
		
		/* If a new connection is accepted by the server, the server adds the client into the list*/		
		int addclient(int new_sd)
		{
			node *newnode, *temp;

			newnode = new node;
			newnode->csd = new_sd;
			memset(newnode->u_name,0,strlen(newnode->u_name));
			newnode->next = NULL;
			
			temp = start;
			if(temp == NULL)
     			{
    		 	  	start = newnode;
				return 0;
      			}
       			while(temp->next!=NULL)
      			{
				temp = temp->next;
			}
			temp->next = newnode;	

			return 0;
 		}
		
		/* After the client sends the JOIN, the server updates its client list*/
		int updateclient(int sd, char *name, unsigned int len)
		{
			node *temp = start;
			while(temp!=NULL)
			{
				if(!strcmp(name, temp->u_name))
					return -1;
				if(temp->csd == sd)
				{
					memset(temp->u_name,'\0',16);					
					strncpy(temp->u_name, name, len);
					no_client++;
					break;
				}
				temp = temp->next;
			}	
			return 0;
		}
		
		/*IF client closed the connection, server will remove its entry*/
		int removeclient(int sd)
		{
			node *temp = start, *prev;
			if(temp == NULL)
				return -1;

			if(temp->csd == sd)
			{
				start = temp->next;
				no_client--;
				delete(temp);
				return 0;
			}
			prev = start;
			while(temp->next!=NULL)
			{
				prev = temp;
				temp = temp->next;
				if(temp->csd == sd)
				{
					prev->next = temp->next;
					no_client--;
					delete(temp);
					return 0;
				}
			}
			
			return 0;
		}
		
		/* Return the username of the client*/
		int getname(char *buf, int sd)
		{
			node *temp;
			memset(buf,'\0',16);
			temp = start;
			while(temp!=NULL)
			{
				if(temp->csd == sd)
				{
					strcpy(buf,temp->u_name);
				}
				temp = temp->next;
			}
			return 0;
		}
		
		/* Broadcast the outgoing packet to all client except one*/  
		int broadcast(char* buf_send, int not_send_me)
		{
			node *temp;
			temp = start;
			while(temp!=NULL)
			{
				if(temp->csd != not_send_me)
				{ 
					write(temp->csd,buf_send, MAX_SIZE);	
				}	
				temp = temp->next;
			}
			return 0;
		}
		/* Again add all the client sds into the set*/
		int add_cds(int * max, fd_set *clientfds)
		{
			node *temp;
			while(temp!=NULL)
			{
				FD_SET(temp->csd,clientfds);
				if(*max <= temp->csd)
				{	
					*max = temp->csd +1;
				}

				temp = temp->next;
			}
			return 0;
		}
				
};


	
