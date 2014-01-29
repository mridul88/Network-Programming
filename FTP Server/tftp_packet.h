#define TFTP_H

#include "def.h"

/* This is the class to store the TFTP Message packet 
*/
class tftp_pkt
{		
	public:
		char *buf, rcvd[MAXSIZE];	
		uint16 opcode, block_no, err_code;
		char file_name[20], mode[10]; 
		

		/* Constructor*/
		tftp_pkt()
		{
			// Do nothing
		}

		tftp_pkt(uint16 op)
		{
			opcode = op;
		}	 		
	
		/* Packs the packet in the char * and into the proper TFTP message format used to send*/
		int make_data_pkt(int len, FILE **fp)
		{
			uint16 tl=0, size=4;
			size += len;
			buf = new char[size];

			memset(buf,'\0',size);	
			
			/* Copy data*/ 
			opcode = htons(opcode);
			memcpy(buf, &opcode,2);	
			tl = 2;
		
			block_no = htons(block_no);
			memcpy(buf+tl, &block_no,2);	
			tl += 2;

			fread (buf+tl,1,len,*fp); // copy the file details into the buf to send		
	
			return 0;		
		}

};

	



		
	
