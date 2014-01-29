#define HTTP_H

#include "def.h"

/* This is the class to store the Attribute packet 
*  header - will store both the type and len
*/
class http_msg
{	
		
	public: 
		char hostname[70];
		char url[200];
		char uri[100];
		char getreq[MSGSIZE];           // request msg
		char rcvd[MAX_SIZE];
                int  type;
		int  status;
		char errmsg[100];
		char expire[50];
		char modify[50];
		
		/*It will create the SBCP attribute packet as per the specifications*/
		http_msg()
		{
			memset(hostname,'\0',70);
			memset(url,'\0',200);		
			memset(uri,'\0',100);
			memset(getreq,'\0',MSGSIZE);
			memset(rcvd,'\0',MAX_SIZE);
			memset(errmsg,'\0',100);
			memset(expire,'\0',50);
			memset(modify,'\0',50);
			type=0;	
			status=0;
		}

		void create_msg(char *url1,const char* req_type)  
		{       
			char temp[8];
			char *temp1;
			const char *http="http://";
			char *u,*h;
			memset(temp,'\0',8);
			strncpy(temp,temp1,7);
			if(*temp==*http)
			{
				url1=url1+7;
			}
			temp1=new char[strlen(url1)+1];
			memset(temp1,'\0',strlen(url1)+1);
			strncpy(temp1,url1,strlen(url1));
			//memset(hostname,'\0',70);
			memset(url,'\0',200);		
			//memset(uri,'\0',100);

			strncpy(url,url1,strlen(url1));
			
			printf("CREATE A MESSAGE WITH URL:%s",url);
			
			if((u=strstr(url1,"/"))!=NULL)
			{
				strncpy(uri,u,strlen(u));
				h=strtok(url1,"/");
				strncpy(hostname,h,strlen(h));
				
			}
			else
			{ 
				strncpy(uri,"/",1);
				strncpy(hostname,url1,strlen(url1));
			}	
			
			strncpy(url1,temp1,strlen(temp1));
			printf("\n\nurl=%s\nuri=%s\nhostname=%s\n",url,uri,hostname);

			
			strncpy(getreq,req_type,strlen(req_type));
			strncpy(getreq+strlen(req_type),uri,strlen(uri));
			strncpy(getreq+strlen(req_type)+strlen(uri),HTTP,strlen(HTTP));
			strncpy(getreq+strlen(req_type)+strlen(uri)+strlen(HTTP),HOST,strlen(HOST));
			strncpy(getreq+strlen(req_type)+strlen(uri)+strlen(HOST)+strlen(HTTP),hostname,strlen(hostname));
			strncpy(getreq+strlen(req_type)+strlen(uri)+strlen(HOST)+strlen(hostname)+strlen(HTTP),CONNECTION,strlen(CONNECTION));	
			delete(temp1);
			
		}
		
		void getdate()
		{
			
			for(int i=0;i!=-1;)
			{
				if(rcvd[i]=='L'&&rcvd[i+13]==':')
				{	
				
					strncpy(modify,rcvd+i+20,24);
				
				}
				if(rcvd[i]=='E'&&rcvd[i+7]==':')  
				{	
		
					strncpy(expire,rcvd+i+14,24);
					i=-1; //break the loop
				}
				else i++;			

			}
		}		
};


	



		
	
