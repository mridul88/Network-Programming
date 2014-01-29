#define CACHE_H

#include "connect.h"

//using namespace stdext;

template<class K,class T>
struct CacheEntry
{
	K key;
	T data;
	int data_len;
	char expire[50];
	char modify[50];
	CacheEntry* prev;
	CacheEntry* next;
};


template<class K,class T>
class Cache
{
	private:
		map<K,CacheEntry<K,T>*,cmp_str> 	_mapping;
		vector<CacheEntry<K,T>*>   	 _freeEntries;
		CacheEntry<K,T> *	head;
	        CacheEntry<K,T> *	tail;
	        CacheEntry<K,T> *	entries; 

	public:
		Cache(size_t size)
		{
			entries=new CacheEntry<K,T>[size];
			for(int i=0;i<size;i++) _freeEntries.push_back(entries+i);//each time add new data into cache we will take one entry from this vector
			head = new CacheEntry<K,T>;
			tail = new CacheEntry<K,T>;
			head->prev = NULL;
			head->next = tail;
			tail->next = NULL;
			tail->prev = head;

		}
		
		~Cache()
		{
			delete head;
			delete tail;
			delete [] entries; //
		}	
		
		void put(K key,T data,int len)
		{
			
			CacheEntry<K,T>* node = _mapping[key]; //if _mapping[key] doesn't exist, node=NULL;
			if(node)   //if exits,we need to updata
			{
				detach(node);
				node->data = data;
				node->data_len=len;
				attach(node);  //put it to the first;
				
			}
			else
			{
				if(_freeEntries.empty()) //the cache is full now
				{
					node=tail->prev;
					detach(node);
					_mapping.erase(node->key);
					node->data=data;
					node->data_len=len;
					node->key=key;
					attach(node);
				}
				else
				{
					node = _freeEntries.back(); //get a entry from vector freeentries
					_freeEntries.pop_back();
					node->key = key;
					node->data = data;
					node->data_len=len;
					

					_mapping[key] = node;
					attach(node);
				}
			}
		}
		
		T get(K key,int *len)
		{
			CacheEntry<K,T>* node=_mapping[key];
			if(node)
			{
				detach(node);
				attach(node);
				*len=node->data_len;
				return node->data;
			}
			else return NULL;
		}
		char* get_mod(K key)
		{

			CacheEntry<K,T>* node=_mapping[key];
			if(node)
			{
				detach(node);
				attach(node);
				return node->modify;
			}
			else return NULL;
		}
		char* get_exp(K key)
		{

			CacheEntry<K,T>* node=_mapping[key];
			if(node)
			{
				detach(node);
				attach(node);
				return node->expire;
			}
			else return NULL;
		}
		void getall()
		{		
			CacheEntry<K,T>* temp=head->next;			
			for(int i=1;temp!=tail;i++)
			{
				printf("The data in cache:\nPosition %d URL= %s Data size is %d\nLasted_modified:%s\nExpires:%s\n",i,temp->key,(int)temp->data_len,temp->modify,temp->expire);
				temp=temp->next;
			}
		}
		int update_time(K key,char* mod,char *exp,int flag)
		{
                        CacheEntry<K,T>* node=_mapping[key];
			if(node)
			{
				memset(node->expire,'\0',50);
				memset(node->modify,'\0',50);
				strncpy(node->modify,mod,strlen(mod));
				strncpy(node->expire,exp,strlen(exp));
				if(flag==1)  //we also need to update the date in content
				{	
					for(int i=0;i!=-1;)
					{	
						if(node->data[i]=='E'&&node->data[i+7]==':')  
						{	
							strncpy(node->data+i+14,exp,24);
							i=-1; //break the loop
						}
						else i++;			

					}			

				}	
			}
			else return -1;
		}

		private:
			void detach(CacheEntry<K,T>* node)
			{
				node->prev->next = node->next;
				node->next->prev = node->prev;
			}
			void attach(CacheEntry<K,T>* node)
			{
				node->next = head->next;
				node->prev = head;
				head->next = node;
				node->next->prev = node;
			}
};


















				
