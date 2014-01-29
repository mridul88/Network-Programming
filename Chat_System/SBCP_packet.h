#define SBCP_H

#include "def.h"

/* This is the class to store the Attribute packet 
*  header - will store both the type and len
*/
class Attr_pkt
{
	private:
		/* Convert type and len into one int header - used for sending*/
		unsigned int code_header(unsigned int type,unsigned int len)
		{
			unsigned int hdr =0;
			type = type<<16;
			hdr = len | type;
			return hdr;
		}

		int decode_header(unsigned int hdr,unsigned  int * type, unsigned int *len)
		{
			unsigned int temp = hdr;
			*type = temp>>16;
		
			temp = hdr;
			temp = temp<<16;
			*len = temp>>16;
			return 0;
		}	
				
	public:
		unsigned int header;
		unsigned int type, len;
		char info[512];           // payload

		/*It will create the SBCP attribute packet as per the specifications*/
		int create_attr_pkt(int msg_type, char * data)  
		{
			memset(info,'\0',512);			
			strncpy(info,data,strlen(data));
		
			len = strlen(info)+4; //for the header
			switch(msg_type)
			{	
				case ATT_REASON: 
				{
  					type = ATT_REASON;
					break;
				}
				case ATT_CCOUNT: 
				{
  					type = ATT_CCOUNT;
					break;
				}
				case ATT_UNAME: 
				{
  					type = ATT_UNAME;
					break;
				}
				case ATT_MSG: 
				{
  					type = ATT_MSG;
					break;
				}
				default:
				{
					return -1;
				}
			}
			header = code_header(type, len);
			return 0;
		}
		
		int unpack_attr_pkt(int input)
		{
			decode_header(input, &type, &len);
			return 0;
		}		
					
};

/* This is the class to store the Message packet 
*  header - will store both the type and len and version
*  It will store a vector pointer of attributes
*/
class Msg_pkt
{		
	private:
		unsigned int code_header(unsigned int vrsn,unsigned int type,unsigned int len)
		{
			unsigned int hdr =0;
			vrsn = vrsn<<23;
			type = type<<16;
			hdr = len | vrsn | type;
			return hdr;
		}

		int decode_header(unsigned int hdr,unsigned int*vrsn,unsigned int * type,unsigned int *len)
		{
			int temp = hdr;
			*vrsn = temp>>23;
			
			temp = temp<<9;
			*type = temp>>25;
		
			temp = hdr;
			temp = temp<<16;
			*len = temp>>16;
			return 0;
		}	
			
	public:	
		unsigned int header;
		unsigned int vrsn, type, len;	
		vector <Attr_pkt*> attr;  // A dynamic array of attr_packet pointers 
		char buf_send[MAX_SIZE]; /*used for packing and sending the data*/
	
	
		/*It will create the SBCP message packet as per the specifications*/
		int create_msg_pkt(int msg_type)  
		{
			unsigned int i;			
			vrsn = 3;
			len =4;
			for(i=0; i< attr.size(); i++)
			{
				len += attr[i]->len;
			}
	
			switch(msg_type)
			{
				case JOIN: 
				{
  					type = JOIN;
					break;
				}
				case FWD: 
				{
  					type = FWD;
					break;
				}
				case SEND: 
				{
  					type = SEND;
					break;
				}
				case NAK: 
				{
  					type = NAK;
					break;
				}
				case OFFLINE: 
				{
  					type = OFFLINE;
					break;
				}
				case ACK: 
				{
  					type = ACK;
					break;
				}
				case ONLINE: 
				{
  					type = ONLINE;
					break;
				}
				case IDLE: 
				{
  					type = IDLE;
					break;
				}
				default:
				{
					return -1;
				}
			}
			
			header = code_header(vrsn, type, len);			
			return 0;
		}

		/* Packs the packet in the char * and into the proper message format used to send*/
		int pack_pkt()
		{
			unsigned int l,tl=0, hdr_size =4, t;
			//hdr_size = sizeof(unsigned int);
			
			memset(buf_send,'\0', MAX_SIZE);	
			
			/* Copy msg header*/ 
			header = htonl(header);
			memcpy(buf_send, &header, hdr_size);
			
			tl = hdr_size;
			/* Copy all attributes*/
			for(unsigned int i=0; tl< len && i< attr.size(); i++)
			{
				t = htonl(attr[i]->header);
				memcpy(buf_send+tl, &t, hdr_size);
				tl+= hdr_size;

				l = attr[i]->len - hdr_size;
				memcpy(buf_send+tl,attr[i]->info, l);
				tl+=l;
			} 
				
			return 0;
			
		}

		/*Reads packet data and set the object data members with information*/
		int unpack_msg_pkt(int input)
		{
			decode_header(input, &vrsn, &type, &len);
			return 0;
		}	
		
};

	



		
	
