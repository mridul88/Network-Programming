BEGIN{
	startTime=30;
	stopTime=180;
	recvdszie1=0;
      }
{
	time=$1;
	data1=$2;
	if(time>=30&&time<=180)
	{	
	  recvdsize1=recvdsize1+data1;
	}
	
	
}	
END{
	throughput1=recvdsize1/(stopTime-startTime);
	print "Throughput1:" throughput1 "Kbits/s";
}
