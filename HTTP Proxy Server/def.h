/*This file contains all the global defintions and 
* constant strings..
*/

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
//#include<hash_map>
#include<vector>
#include <time.h>
using namespace std;

/* Defining global variables*/
#define GET  	"GET "
#define HEAD    "HEAD "
#define HTTP     " HTTP/1.0\r\n"
#define HOST     "Host: "
#define CONNECTION 	"\r\nConnection: close\r\n\r\n"

#define GETREQ 0  //request type
#define HEADREQ 1
#define GETRQ 01
#define RSPN  02 //GET RESPOND FROM WEB SERVER



#define MAX_SIZE 2000000
#define MSGSIZE 300
#define CACHE_SIZE 10
struct cmp_str
{
   bool operator()(char const *a, char const *b)
   {
      return std::strcmp(a, b) < 0;
   }
};


int checkdate(char *date)
{
	time_t now,exp;
	struct tm tm1,*nowtime;
	if(strptime(date,"%d %b %Y %T %Z",&tm1) == NULL)
            printf("\nstrptime failed\n"); 
	exp=mktime(&tm1);
	time(&now);
	nowtime=gmtime(&now);
	nowtime->tm_hour=(nowtime->tm_hour-1)%24;
	now=mktime(nowtime);
	printf ("The expire time and date: %s\n", asctime(&tm1));
  	printf ("Current local time and date: %s\n", asctime(nowtime));
	if(difftime(exp,now)>0)
	return 1;
	else
	return -1;
}		
		
