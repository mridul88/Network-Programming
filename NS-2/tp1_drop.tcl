#Create a simulator object
set ns [new Simulator]

#open output files
set f0 [open tp1_drop_out0.tr w]
set f1 [open tp1_drop_out1.tr w]
#set f2 [open tp1_drop_out2.tr w]


#Define different colors for data flows
$ns color 1 Red
$ns color 2 Green

#Open the nam trace file
set nf [open out.nam w]
$ns namtrace-all $nf


#Define a 'finish' procedure
proc finish {} {
        global ns nf f0 f1 
        $ns flush-trace
	#Close the output files
	close $f0
        close $f1
	#Close the trace file
        close $nf
	#Execute nam on the trace file
        exec nam out.nam 
	exec xgraph tp1_drop_out0.tr tp1_drop_out1.tr -geometry 800x400 &
        exit 0
}

#Define a procedure which records the bytes received by the
#two traffic sinks sink0/1 and writes it to the three files f0/1.
proc record {} {
        global sink0 sink1  f0 f1
	#Get an instance of the simulator
	set ns [Simulator instance]
	#Set the time after which the procedure should be called again
        set time 1
	#How many bytes have been received by the traffic sinks?
        set bw0 [$sink0 set bytes_]
        set bw1 [$sink1 set bytes_]
	#Get the current time
        set now [$ns now]
	#Calculate the bytes recevied (in Kbits/s) and write it to the files
        puts $f0 "$now [expr $bw0*8/1000]"
	puts $f1 "$now [expr $bw1*8/1000]"
   	#reset the values
        $sink0 set bytes_ 0
        $sink1 set bytes_ 0
	#Re-schedule the procedure
        $ns at [expr $now+$time] "record"
}

#Create four nodes
set n0 [$ns node]
set n1 [$ns node]
set n2 [$ns node]
set n3 [$ns node]
set n4 [$ns node]
set n5 [$ns node]

#Create links between the nodes
$ns duplex-link $n0 $n2 10Mb 1ms    DropTail
$ns duplex-link $n1 $n2 10Mb 1ms DropTail
$ns duplex-link $n2 $n3 1Mb  10ms    DropTail
$ns duplex-link $n3 $n4 10Mb 1ms    DropTail
$ns duplex-link $n3 $n5 10Mb 1ms DropTail

$ns duplex-link-op $n0 $n2 orient right-down
$ns duplex-link-op $n1 $n2 orient right-up
$ns duplex-link-op $n2 $n3 orient right
$ns duplex-link-op $n3 $n4 orient right-up
$ns duplex-link-op $n3 $n5 orient right-down

#set queuse size
$ns queue-limit $n2 $n3 20

#Monitor the queue for the link between node 2 and node 3
#$ns duplex-link-op $n2 $n3 queuePos 0.5


#Create a TCP agent (SACK Version) and attach it to node n0 GREEN
set tcp0 [new Agent/TCP/Reno]
$tcp0 set class_ 1
$ns attach-agent $n0 $tcp0

# Create a FTP traffic source and attach it to tcp0
set ftp0 [new Application/FTP]
$ftp0 attach-agent $tcp0


#Create a TCP agent (SACK Version) and attach it to node n1
set tcp1 [new Agent/TCP/Reno]
$tcp1 set class_ 2
$ns attach-agent $n1 $tcp1

# Create a FTP traffic source and attach it to tcp1
set ftp1 [new Application/FTP]
$ftp1 attach-agent $tcp1

#Create a Sink agent (a TCP SACK Sink) and attach it to node n4
set sink0 [new Agent/TCPSink]
$ns attach-agent $n4 $sink0

#Connect the sources (tcp0, tcp1) with the TCP sink n4 (sink0)
$ns connect $tcp0 $sink0  

#Create a Sink agent (a TCP SACK Sink) and attach it to node n5
set sink1 [new Agent/TCPSink]
$ns attach-agent $n5 $sink1

#Connect the sources (tcp0, tcp1) with the TCP sink n5 (sink1)
$ns connect $tcp1 $sink1

#Schedule events for the CBR agents
$ns at 0.0   "record"
$ns at 0.0   "$ftp0 start"
$ns at 0.0   "$ftp1 start"
$ns at 180.0  "$ftp0 stop"
$ns at 180.0  "$ftp1 stop"

#Call the finish procedure after 5 seconds of simulation time
$ns at 181.0 "finish"

#Run the simulation
$ns run
