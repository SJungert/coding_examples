
#include "frameio.h"
#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <queue>
#include <map>

frameio net;             // gives us access to the raw network
message_queue ip_queue;  // message queue for the IP protocol stack
message_queue arp_queue; // message queue for the ARP protocol stack

//Function Prototypes
void sendARP(octet [], int);
void sendICMP_RequestLocal(octet ip[], int size);
void sendICMP_RequestGlobal(octet ip[], int size);

// IP of your machine to send arp messages [SIP (source IP)]
octet ip_range_1 = 192;
octet ip_range_2 = 168;
octet ip_range_3 = 1;
octet ip_range_4 = 60;

struct ether_frame       // handy template for 802.3/DIX frames
{
	octet dst_mac[6];     // destination MAC address
	octet src_mac[6];     // source MAC address
	octet prot[2];        // protocol (or length)
	octet data[1500];     // payload
};

struct arp_payload
{
	octet hardwaretype[2];	//hardware type
	octet protocoltype[2];	//protocol type
	octet hal;				//hardware address length
	octet pal;				//protocol address length
	octet opcode[2];		//opcode (request or reply)
	octet sha[6];			//sender's hardware address
	octet sip[4];			//sender's IP
	octet tha[6];			//target hardware address
	octet tip[4];			//target IP

};

struct cache_info
{
	octet ip[4];
	octet mac[6];

};

struct ip_header
{
       octet ipVersion[1]; 			//includes Header Length
       octet typeOfService[1];		
       octet datagramLength[2];
       octet packetIdentifier[2];
       octet flag_offset[2];		//first 3 bits are flag
       octet timeToLive[1];
       octet protocol[1];
       octet checksum[2];
       octet sip[4];
       octet tip[4];
};

struct imcp_payload
{
      octet type[1];
      octet code[1];
      octet checksum_imcp[2];
      octet identifier[2];
      octet sequencenumber[2];
      octet imcp_data[56];
};

//Cache Map for storing IP-MAC PAIRS
std::map<int, octet *> cache;

//Sequence Number
octet sequenceValue[2] = {0x00, 0x01};

// int chksum(octet *s, int bytes, int initial)
//
// compute the one's complement checksum used in most IP protocols.
//
// parameters:
//   s:       address of block (string) to be checksummed
//   bytes:   number of bytes to checksum (an odd number of bytes is allowed)
//   initial: checksum from a previous block (this allows a checksum to be
//            computed for a set of non-contiguous blocks)
// return value:
//   one's complement checksum
//
// Note: the checksum this function provides must usually
// be complemented before it is placed in the packet:
//
//    ip->header_checksum[0] = 0; // initial checksum value must be 0
//    ip->header_checksum[1] = 0;
//    int sum = chksum((octet *)ip,20,0);
//    ip->header_checksum[0] = ~sum >> 8;
//    ip->header_checksum[1] = ~sum & 0xff;
//   
int chksum(octet *s, int bytes, int initial)
{
   long sum = initial;
   int i;
   for ( i=0; i<bytes-1; i+=2 )
   {
      sum += s[i]*256 + s[i+1];
   }
   //
   // handle the odd byte
   //
   if ( i < bytes ) sum += s[i]*256;
   //
   // wrap carries back into sum
   //
   while ( sum > 0xffff ) sum = (sum & 0xffff) + (sum >> 16);
   return sum;
}


//
// This thread sits around and receives frames from the network.
// When it gets one, it dispatches it to the proper protocol stack.
//
void *protocol_loop(void *arg)
{
	ether_frame buf;
	while (1)
	{
		int n = net.recv_frame(&buf, sizeof(buf));
		if (n < 42) continue; // bad frame!
		switch (buf.prot[0] << 8 | buf.prot[1])
		{
		case 0x800:
			ip_queue.send(PACKET, buf.data, n);
			break;
		case 0x806:
			arp_queue.send(PACKET, &buf, n);
			break;
		}
	}
}

//
// Toy function to print something interesting when an IP frame arrives
//
void *ip_protocol_loop(void *arg)
{
	//Orignal Variable Declirations
        octet buf[1500];
	octet buf2[64];
	event_kind event;
	int timer_no = 1;
	cache_info temp;
	
	//New Variable Declirations
	octet imcpCheck;		// Check if recievd ip frame is IMCP
	ether_frame sending;
	ip_header tosend;
	imcp_payload reply;
	unsigned int checkSumValue;		// Value of Checksum
	octet data_1[2];			// Concatinates Checksum -> 2 bytes
	octet data_2[2];			// Concatinates imcp_checksum -> 2 bytes
	

	// for fun, fire a timer each time we get a frame
	while (1)
	{
	      ip_queue.recv(&event, buf, sizeof(buf));
	      imcpCheck = buf[9];
	      if(imcpCheck == 1){
		
		//Update Sequence Number
		//printf("Buf[26]: 0x%02x %02x\n", buf[26], buf[27]);
		//printf("sequenceValue[0]: 0x%02x %02x\n", sequenceValue[0], sequenceValue[1]);
		  if(buf[27] == sequenceValue[1]){
		    if(sequenceValue[1] == 0xff){
		      sequenceValue[1] = 0x00;
		    }
		    else{
		
		      sequenceValue[1] += 1;
		      
		    }
		  }
		  
		if(buf[20] == 8){
		
		  
		printf("\nICMP packet recieved\n");
		for(int i = 0; i < 64; i++){
		  if(i == 32){
		   printf("%02x \n", buf[20+i]); 
		  }
		  else{
		  printf("%02x ", buf[20+i]);
		  }
		}
		
		// ip header setup
		tosend.ipVersion[0] = 0x45;
		tosend.typeOfService[0] = 0x00;
		tosend.datagramLength[0] = buf[2];
		tosend.datagramLength[1] = buf[3];
		tosend.packetIdentifier[0] = buf[4];
		tosend.packetIdentifier[1] = buf[5];
		//printf("\nIdentifer: 0x%02x%02x\n", tosend.packetIdentifier[0], tosend.packetIdentifier[1]);
		tosend.flag_offset[0] = buf[6];
		tosend.flag_offset[1] = buf[7];
		tosend.timeToLive[0] = 0x40;
		tosend.protocol[0] = 0x01;
		tosend.checksum[0] = 0x00;
		tosend.checksum[1] = 0x00;
		memcpy(tosend.tip, &buf[12], 4);
		memcpy(tosend.sip, &buf[16], 4);
		//printf("[+] tosend_targetIP:  %d %d %d %d \n", tosend.tip[0], tosend.tip[1], tosend.tip[2], tosend.tip[3]);
		
		//Making a IP packet payload of 20 bytes - encapsulating (Before calculating checksum)
		memcpy(sending.data, &tosend, sizeof(tosend));
		
		//Calculate Checksum 
		checkSumValue = ~ chksum(sending.data, 20, 0);
		//printf("\n Checksum Integer: \n", checkSumValue);
		data_1[0] = (checkSumValue >> 8) & 0xff;
		data_1[1] = checkSumValue & 0xff;
		//printf(" data_1[0] then [1]: 0x%02x%02x\n", data_1[0], data_1[1]);
		
		//Making a IP packet payload of 20 bytes - encapsulating (After calculating checksum)
		memcpy(tosend.checksum, &data_1, sizeof(data_1));	
		memcpy(sending.data, &tosend, sizeof(tosend));
		
		//IP Header Ready!
		/*
		printf("\nIP_Header Payload\n");
		for(int i = 0; i < 20; i ++){
		printf("\nElement: %d  sending data: 0x%02x  recieved data: 0x%02x\n", i, sending.data[i], buf[i]);
		}
		*/
		
		//Making IMCP portion
		reply.type[0] = 0x00;
		reply.code[0] = 0x00;
		reply.checksum_imcp[0] = 0x00;
		reply.checksum_imcp[1] = 0x00;
		memcpy(reply.identifier, &buf[24], 2);		//Copy Identifier
		//printf("\nIdentifer: 0x%02x%02x\n", reply.identifier[0], reply.identifier[1]);
		memcpy(reply.sequencenumber, &buf[26], 2);	//Copy Sequence Number
		//printf("Sequence : 0x%02x%02x\n", reply.sequencenumber[0], reply.sequencenumber[1]);
		memcpy(reply.imcp_data, &buf[28], 56);		//Copy Data
		
		//Making a IMCP packet payload of 64 bytes - encapsulating (Before calculating checksum)
		//memcpy(20 + sending.data, &reply, sizeof(reply));
		memcpy(&buf2, &reply, sizeof(reply));
		
		
		//printf("\nIP of me: %d %d %d %d\n", tosend.sip[0], tosend.sip[1], tosend.sip[2], tosend.sip[3]);
		/*
		printf("ICMP Payload\n");
		for(int i = 0; i < 64; i ++){
		printf("Element: %d  sending data: 0x%02x  recieved data: 0x%02x\n", i+20, buf2[i], buf[i+20]);
		}
		*/
	
		
		//Calculate Checksum 
		checkSumValue = ~ chksum(buf2, 64, 0);
		data_2[0] = (checkSumValue >> 8) & 0xff;
		data_2[1] = checkSumValue & 0xff;
		//printf(" data_2[0] then [1]: 0x%02x%02x\n", data_2[0], data_2[1]);
		//printf(" Sequence Number: 0x%02x%02x\n", reply.sequencenumber[0], reply.sequencenumber[1]); 
		
		//Making a IMCP packet payload of 64 bytes - encapsulating (After calculating checksum)
		memcpy(reply.checksum_imcp, &data_2, sizeof(data_2));
		memcpy(20 + sending.data, &reply, sizeof(reply));
		//printf("[+] checksum: 0x%02x%02x\n", sending.data[22], sending.data[23]);
		
		/*
		printf("\nIMCP Payload\n");
		for(int i = 20; i < 28; i ++){
		printf("\nElement: %d  sending data: 0x%02x  recieved data: 0x%02x\n", i, sending.data[i], buf[i]);
		}
		*/
		
		//Collecting IP frame variables
		memcpy(temp.ip, tosend.tip, 4);
		//printf("[+] tempIP:  %d %d %d %d \n", temp.ip[0], temp.ip[1], temp.ip[2], temp.ip[3]);
		
		//Calculate appropriate hash from source ip address
		unsigned int ip_Check_int = (int)(temp.ip[0]) + (int)(temp.ip[1]) + (int)(temp.ip[2]) + (int)temp.ip[3];
		//printf("[+] tempIP:  %d %d %d %d \n", temp.ip[0], temp.ip[1], temp.ip[2], temp.ip[3]);
		
		if (cache.find(ip_Check_int) == cache.end())
		{
		  //MAC is not stored
		  //printf("[+] IP MAC pair does not exist\n");
		  
		  sendARP(tosend.tip, 4);
		  sleep(1);
		  memcpy(sending.dst_mac, cache[ip_Check_int], sizeof(cache[ip_Check_int])); 	// Destination Hardware Address
		  memcpy(sending.src_mac, net.get_mac(), sizeof(net.get_mac()));		// Source Hardware Address
		 // printf("[+] destination mac:  %02x %02x %02x %02x %02x %02x\n", sending.dst_mac[0], sending.dst_mac[1], sending.dst_mac[2], sending.dst_mac[3], sending.dst_mac[4], sending.dst_mac[5]);
		 // printf("[+] source mac     :  %02x %02x %02x %02x %02x %02x\n", sending.src_mac[0], sending.src_mac[1], sending.src_mac[2], sending.src_mac[3], sending.src_mac[4], sending.src_mac[5]);
		  sending.prot[0] = 0x08; 							// Manually Set Protocol to 0x0800 or IPV4
		  sending.prot[1] = 0x00;
		  int n = net.send_frame(&sending, 98);
		  
		  //printf("[+] Sending  IMCP Reply to %d %d %d %d \n", tosend.tip[0], tosend.tip[1], tosend.tip[2], tosend.tip[3]);
		  
		}
		else
		{
		  //Mac is stored
		  //printf("[+] IP MAC pair exists!\n");
		  memcpy(sending.dst_mac, cache[ip_Check_int], sizeof(cache[ip_Check_int])); 	// Destination Hardware Address
		  memcpy(sending.src_mac, net.get_mac(), sizeof(net.get_mac()));		// Source Hardware Address
		 // printf("[+] destination mac:  %02x %02x %02x %02x %02x %02x\n", sending.dst_mac[0], sending.dst_mac[1], sending.dst_mac[2], sending.dst_mac[3], sending.dst_mac[4], sending.dst_mac[5]);
		 // printf("[+] source mac     :  %02x %02x %02x %02x %02x %02x\n", sending.src_mac[0], sending.src_mac[1], sending.src_mac[2], sending.src_mac[3], sending.src_mac[4], sending.src_mac[5]);
		  sending.prot[0] = 0x08; 							// Manually Set Protocol to 0x0800 or IPV4
		  sending.prot[1] = 0x00;
		  
		  
		  int flag = 0;

			//check if the target of received packet and sender of new frame is same or not.
			for (int i = 0; i < 4; i++)
			{
				// taking the difference between Sender address and Source address

				flag += (buf[12 + i] - tosend.tip[i]);
			}

			if (flag == 0)
			{
				int n = net.send_frame(&sending, 98);
				//printf("[+] Sending  IMCP Reply to %d %d %d %d \n", tosend.tip[0], tosend.tip[1], tosend.tip[2], tosend.tip[3]);
			}			
		}
		
		
	      } // End of ICMP stuf
	      
	      }
		
	}
}

//
// Toy function to print something interesting when an ARP frame arrives
//
void *arp_protocol_loop(void *arg)
{
	ether_frame buf;
	event_kind event;
	cache_info temp;
	ether_frame sending;
	arp_payload tosend;

	while (1)
	{
		arp_queue.recv(&event, &buf, sizeof(buf));
		// Checking if ARP Request.
		if (buf.data[7] == 1)
		{
			memcpy(sending.dst_mac, buf.src_mac, sizeof(buf.src_mac));
			memcpy(sending.src_mac, net.get_mac(), sizeof(net.get_mac()));
			memcpy(sending.prot, buf.prot, sizeof(buf.prot));

			tosend.hardwaretype[0] = 0x00; 	// hardwaretype first two bits 0
			tosend.hardwaretype[1] = 0x01; 	// hardwaretype next two bits 1 - Ethernet
			tosend.protocoltype[0] = 0x08; 	// IPV4
			tosend.protocoltype[1] = 0x00;
			tosend.hal = 0x6; 				// Hardware Adddress Length
			tosend.pal = 0x4; 				// Protocol address length
			tosend.opcode[0] = 0x0; 		//Opcodes
			tosend.opcode[1] = 0x2; 		//Reply (2)

			//Copy system hardware address to Sender hardware address
			memcpy(tosend.sha, net.get_mac(), sizeof(net.get_mac()));

			//Sender IP is set
			tosend.sip[0] = ip_range_1;	//192
			tosend.sip[1] = ip_range_2;	//168
			tosend.sip[2] = ip_range_3;	//1
			tosend.sip[3] = ip_range_4;	//60

			//Target hardware address would be the sender mac address of the received packet
			memcpy(tosend.tha, buf.src_mac, sizeof(buf.src_mac));

			// Target IP is set
			memcpy(tosend.tip, &buf.data[14], 4);

			//Making a ARP packet payload of 28 bytes- encapsulating
			memcpy(&sending.data[0], &tosend, 28);

			int flag = 0;

			//check if the target of received packet and sender of new frame is same or not.
			for (int i = 0; i < 4; i++)
			{
				// taking the difference between Sender address and Source address

				flag += (buf.data[24 + i] - tosend.sip[i]);


			}

			if (flag == 0)
			{
				int n = net.send_frame(&sending, 42);
				//printf("[+] Sending Reply to %d %d %d %d \n", tosend.tip[0], tosend.tip[1], tosend.tip[2], tosend.tip[3]);
			}
		}

		// Caching Starts
		//cache_info temp;

		// Copying MAC and IP into a temp cache element to check if exists or not.
		memcpy(temp.mac, &buf.data[8], 6);
		memcpy(temp.ip, &buf.data[14], 4);

		// Checking ip_Check_int hash in the cache
		unsigned int ip_Check_int = (int)(temp.ip[0]) + (int)(temp.ip[1]) + (int)(temp.ip[2]) + (int)temp.ip[3];

		// Check IP - MAC exists in the cache
		if (cache.find(ip_Check_int) == cache.end())
		{
			//printf("[+] Adding IP MAC pair: %d %d %d %d\n", temp.ip[0], temp.ip[1], temp.ip[2], temp.ip[3]);
			cache[ip_Check_int] = new octet;
			memcpy(cache[ip_Check_int], temp.mac, sizeof(temp.mac));
			//printf("[+] Adding IP MAC pair: %02x %02x %02x %02x %02x %02x\n", temp.mac[0], temp.mac[1], temp.mac[2], temp.mac[3], temp.mac[4], temp.mac[5]);
			//memcpy(temp.mac, cache[ip_Check_int], sizeof(cache[ip_Check_int]));			
		}
		else
		{
			//printf("[+] Already Stored IP MAC pair: %d %d %d %d\n", temp.ip[0], temp.ip[1], temp.ip[2], temp.ip[3]);
			//printf("[+] IP MAC pair alreay exists\n");
		}
	}
}

void sendARP(octet ip[], int size){
  //variables
  ether_frame sending;
  arp_payload tosend;
  
  //Setting up ethernet frame
  //destination hardware address
  for(int i = 0; i < 6; i++){
    sending.dst_mac[i] = 0xff;
  }
  
  //source hardware address
  memcpy(sending.src_mac, net.get_mac(), sizeof(net.get_mac()));
  
  //protocol (i.e. 0x0806 --> ARP)
  sending.prot[0] = 0x08; 
  sending.prot[1] = 0x06;
 

 tosend.hardwaretype[0] = 0x00; 	// hardwaretype first two bits 0
 tosend.hardwaretype[1] = 0x01; 	// hardwaretype next two bits 1 - Ethernet
 tosend.protocoltype[0] = 0x08; 	// IPV4
 tosend.protocoltype[1] = 0x00;
 tosend.hal = 0x6; 				// Hardware Adddress Length
 tosend.pal = 0x4; 				// Protocol address length
 tosend.opcode[0] = 0x0; 		//Opcodes
 tosend.opcode[1] = 0x1; 		//Request (1)

 //Copy system hardware address to Sender hardware address
 memcpy(tosend.sha, net.get_mac(), sizeof(net.get_mac()));

 //Sender IP is set
 tosend.sip[0] = ip_range_1;	//192
 tosend.sip[1] = ip_range_2;	//168
 tosend.sip[2] = ip_range_3;	//1
 tosend.sip[3] = ip_range_4;	//60

 //Target hardware address would be the sender mac address of the received packet
 for(int i = 0; i < 6; i++){for(int i = 0; i < 6; i++){
    sending.dst_mac[i] = 0xff;
  }
    tosend.tha[i] = 0xff;
   }

  // Target IP is set
 memcpy(tosend.tip, ip, sizeof(ip));
 
  
 //Making a ARP packet payload of 28 bytes- encapsulating
 memcpy(&sending.data[0], &tosend, 28);			

 int n = net.send_frame(&sending, 42); 
// printf("\n[+] Sending Request to %d %d %d %d \n", tosend.tip[0], tosend.tip[1], tosend.tip[2], tosend.tip[3]);
}

void sendICMP_RequestLocal(octet ip[], int size){
  //Variables
  octet buf2[64];
  octet data_fluff[48];
  cache_info temp;
  ether_frame sending;
  ip_header tosend;
  imcp_payload reply;
  unsigned int checkSumValue;		// Value of Checksum
  octet data_1[2];			// Concatinates Checksum -> 2 bytes
  octet data_2[2];			// Concatinates imcp_checksum -> 2 bytes
  unsigned int ip_Check_int;
  
  //Setting up ethernet frame 
  // Target IP is set
  memcpy(tosend.tip, ip, sizeof(ip));
  ip_Check_int = (int)(tosend.tip[0]) + (int)(tosend.tip[1]) + (int)(tosend.tip[2]) + (int)tosend.tip[3];
  
  // Destination Hardware Address
  memcpy(sending.dst_mac, cache[ip_Check_int], sizeof(cache[ip_Check_int]));
  
  // Source Hardware Address
  memcpy(sending.src_mac, net.get_mac(), sizeof(net.get_mac()));
  
  // Manually Set Protocol to 0x0800 or IPV4
  sending.prot[0] = 0x08; 
  sending.prot[1] = 0x00;
  
  
  // Setting ip header setup
  tosend.ipVersion[0] = 0x45;
  tosend.typeOfService[0] = 0x00;
  tosend.datagramLength[0] = 0x00;
  tosend.datagramLength[1] = 0x54;
  tosend.packetIdentifier[0] = 0x00;
  tosend.packetIdentifier[1] = 0x00;
  //printf("\nIdentifer: 0x%02x%02x\n", tosend.packetIdentifier[0], tosend.packetIdentifier[1]);
  tosend.flag_offset[0] = 0x40;
  tosend.flag_offset[1] = 0x00;
  tosend.timeToLive[0] = 0x40;
  tosend.protocol[0] = 0x01;
  tosend.checksum[0] = 0x00;
  tosend.checksum[1] = 0x00;
  
  //Sender IP is set
  tosend.sip[0] = ip_range_1;	//192
  tosend.sip[1] = ip_range_2;	//168
  tosend.sip[2] = ip_range_3;	//1
  tosend.sip[3] = ip_range_4;	//60
  
  memcpy(tosend.tip, ip, size);
  //printf("[+] tosend_targetIP:  %d %d %d %d \n", tosend.tip[0], tosend.tip[1], tosend.tip[2], tosend.tip[3]);
  //Making a IP packet payload of 20 bytes - encapsulating (Before calculating checksum)
  memcpy(sending.data, &tosend, sizeof(tosend));
  
  //Calculate Checksum 
  checkSumValue = ~ chksum(sending.data, 20, 0);
  //printf("\n Checksum Integer: \n", checkSumValue);
  data_1[0] = (checkSumValue >> 8) & 0xff;
  data_1[1] = checkSumValue & 0xff;
  //printf(" data_1[0] then [1]: 0x%02x%02x\n", data_1[0], data_1[1]);
		
  //Making a IP packet payload of 20 bytes - encapsulating (After calculating checksum)
  memcpy(tosend.checksum, &data_1, sizeof(data_1));	
  memcpy(sending.data, &tosend, sizeof(tosend));
		
  //IP Header Ready!
  //Making IMCP portion
  reply.type[0] = 0x08;
  reply.code[0] = 0x00;
  reply.checksum_imcp[0] = 0x00;
  reply.checksum_imcp[1] = 0x00;
  reply.identifier[0] = 0x00;
  reply.identifier[1] = 0x00;
  reply.sequencenumber[0] = 0x00;
  reply.sequencenumber[1] = sequenceValue[1];
  
  //Make data, all zeroes
  for(int i = 0; i < 48; i++){
    data_fluff[i] = 0x00;
  }
  
  //Copy data_fluff
  memcpy(reply.imcp_data, &data_fluff, 48);		//Copy Data
		
 //Making a IMCP packet payload of 64 bytes - encapsulating (Before calculating checksum)
 //memcpy(20 + sending.data, &reply, sizeof(reply));
 memcpy(&buf2, &reply, sizeof(reply));
		
		
 //Calculate Checksum 
 checkSumValue = ~ chksum(buf2, 64, 0);
 data_2[0] = (checkSumValue >> 8) & 0xff;
 data_2[1] = checkSumValue & 0xff;
 //printf(" data_2[0] then [1]: 0x%02x%02x\n", data_2[0], data_2[1]);
 //printf(" Sequence Number: 0x%02x%02x\n", reply.sequencenumber[0], reply.sequencenumber[1]); 
		
 //Making a IMCP packet payload of 64 bytes - encapsulating (After calculating checksum)
 memcpy(reply.checksum_imcp, &data_2, sizeof(data_2));
 memcpy(20 + sending.data, &reply, sizeof(reply));
 //printf("[+] checksum: 0x%02x%02x\n", sending.data[22], sending.data[23]);
		
  int n = net.send_frame(&sending, 98);
  //printf("[+] Sending  IMCP Request to %d %d %d %d \n", tosend.tip[0], tosend.tip[1], tosend.tip[2], tosend.tip[3]);
  
  
}

void sendICMP_RequestGlobal(octet ip[], int size){
  //Variables
  octet buf2[64];
  octet data_fluff[48];
  octet gatewayHA[6];
  cache_info temp;
  ether_frame sending;
  ip_header tosend;
  imcp_payload reply;
  unsigned int checkSumValue;		// Value of Checksum
  octet data_1[2];			// Concatinates Checksum -> 2 bytes
  octet data_2[2];			// Concatinates imcp_checksum -> 2 bytes
  unsigned int ip_Check_int;
  
  //Setting up ethernet frame
  gatewayHA[0] = 0x00;
  gatewayHA[1] = 0x1C;
  gatewayHA[2] = 0x10;
  gatewayHA[3] = 0xf5;
  gatewayHA[4] = 0x0C;
  gatewayHA[5] = 0xac;
  
  
  // Destination Hardware Address
  memcpy(sending.dst_mac, &gatewayHA, sizeof(gatewayHA));
  
  // Source Hardware Address
  memcpy(sending.src_mac, net.get_mac(), sizeof(net.get_mac()));
  
  // Manually Set Protocol to 0x0800 or IPV4
  sending.prot[0] = 0x08; 
  sending.prot[1] = 0x00;
  
  
  // Setting ip header setup
  tosend.ipVersion[0] = 0x45;
  tosend.typeOfService[0] = 0x00;
  tosend.datagramLength[0] = 0x00;
  tosend.datagramLength[1] = 0x54;
  tosend.packetIdentifier[0] = 0x00;
  tosend.packetIdentifier[1] = 0x00;
  //printf("\nIdentifer: 0x%02x%02x\n", tosend.packetIdentifier[0], tosend.packetIdentifier[1]);
  tosend.flag_offset[0] = 0x40;
  tosend.flag_offset[1] = 0x00;
  tosend.timeToLive[0] = 0x40;
  tosend.protocol[0] = 0x01;
  tosend.checksum[0] = 0x00;
  tosend.checksum[1] = 0x00;
  
  //Sender IP is set
  tosend.sip[0] = ip_range_1;	//192
  tosend.sip[1] = ip_range_2;	//168
  tosend.sip[2] = ip_range_3;	//1
  tosend.sip[3] = ip_range_4;	//60
  
  memcpy(tosend.tip, ip, size);
  //printf("[+] tosend_targetIP:  %d %d %d %d \n", tosend.tip[0], tosend.tip[1], tosend.tip[2], tosend.tip[3]);
  //Making a IP packet payload of 20 bytes - encapsulating (Before calculating checksum)
  memcpy(sending.data, &tosend, sizeof(tosend));
  
  //Calculate Checksum 
  checkSumValue = ~ chksum(sending.data, 20, 0);
  //printf("\n Checksum Integer: \n", checkSumValue);
  data_1[0] = (checkSumValue >> 8) & 0xff;
  data_1[1] = checkSumValue & 0xff;
  //printf(" data_1[0] then [1]: 0x%02x%02x\n", data_1[0], data_1[1]);
		
  //Making a IP packet payload of 20 bytes - encapsulating (After calculating checksum)
  memcpy(tosend.checksum, &data_1, sizeof(data_1));	
  memcpy(sending.data, &tosend, sizeof(tosend));
		
  //IP Header Ready!
  //Making IMCP portion
  reply.type[0] = 0x08;
  reply.code[0] = 0x00;
  reply.checksum_imcp[0] = 0x00;
  reply.checksum_imcp[1] = 0x00;
  reply.identifier[0] = 0x00;
  reply.identifier[1] = 0x00;
  reply.sequencenumber[0] = 0x00;
  reply.sequencenumber[1] = sequenceValue[1];
  
  //Make data, all zeroes
  for(int i = 0; i < 48; i++){
    data_fluff[i] = 0x00;
  }
  
  //Copy data_fluff
  memcpy(reply.imcp_data, &data_fluff, 48);		//Copy Data
		
 //Making a IMCP packet payload of 64 bytes - encapsulating (Before calculating checksum)
 //memcpy(20 + sending.data, &reply, sizeof(reply));
 memcpy(&buf2, &reply, sizeof(reply));
		
		
 //Calculate Checksum 
 checkSumValue = ~ chksum(buf2, 64, 0);
 data_2[0] = (checkSumValue >> 8) & 0xff;
 data_2[1] = checkSumValue & 0xff;
 //printf(" data_2[0] then [1]: 0x%02x%02x\n", data_2[0], data_2[1]);
 //printf(" Sequence Number: 0x%02x%02x\n", reply.sequencenumber[0], reply.sequencenumber[1]); 
		
 //Making a IMCP packet payload of 64 bytes - encapsulating (After calculating checksum)
 memcpy(reply.checksum_imcp, &data_2, sizeof(data_2));
 memcpy(20 + sending.data, &reply, sizeof(reply));
 //printf("[+] checksum: 0x%02x%02x\n", sending.data[22], sending.data[23]);
		
  int n = net.send_frame(&sending, 98);
 // printf("[+] Sending  IMCP Request to %d %d %d %d \n", tosend.tip[0], tosend.tip[1], tosend.tip[2], tosend.tip[3]);
}

void *user_input_loop(void *arg){
    int choice = 0;
    octet tosendip[4];
    unsigned int ip_Check_int;
  
    printf("----Main Menu-----\n");
    printf("Select Number Below:\n");
    printf("[1] Send ICMP Request\n");
    printf("[2] Send ARP Request\n");
    scanf("%d", &choice);
    
    switch(choice){
      case 1:
	 choice = 0;
	 printf("ICMP Request Menu\n");
	 printf("Type [1] for a LOCAL ICMP request\n");
	 printf("Otherwise type any number [2]-[9] for a GLOBAL ICMP request\n"); 
	 scanf("%d", &choice);
	 
	 if(choice == 1){
	   //local ICMP request
	   printf("ICMP Local Menu\n");
	   printf("Insert the desired IP address with each 3 digit address followed by an enter:\n");
	   printf("For Example: 192 Enter 168 Enter... so on\n");
	   scanf("%d", &tosendip[0]);
	   scanf("%d", &tosendip[1]);
	   scanf("%d", &tosendip[2]);
	   scanf("%d", &tosendip[3]);
	   printf("[+] Input ip: %d %d %d %d\n", tosendip[0], tosendip[1], tosendip[2], tosendip[3]);
	   
	   ip_Check_int = (int)(tosendip[0]) + (int)(tosendip[1]) + (int)(tosendip[2]) + (int)tosendip[3];
	   if (cache.find(ip_Check_int) == cache.end())
	   {
	      sendARP(tosendip, 4);
	      sleep(1);
	      for(int i = 0; i < 5; i++){
	      sendICMP_RequestLocal(tosendip, 4);
	      sleep(1);
	      }
	   }
	   else
	   {	
	     
	      sendICMP_RequestLocal(tosendip, 4);
	      sleep(1);
	     }
	   }
	      //end ICMP local
	 else{
	   //global ICMP request
	   printf("global\n");
	   printf("ICMP Global Menu\n");
	   printf("Insert the desired IP address with each 3 digit address followed by an enter:\n");
	   printf("For Example: 192 Enter 168 Enter... so on\n");
	   scanf("%d", &tosendip[0]);
	   scanf("%d", &tosendip[1]);
	   scanf("%d", &tosendip[2]);
	   scanf("%d", &tosendip[3]);
	   printf("[+] Input ip: %d %d %d %d\n", tosendip[0], tosendip[1], tosendip[2], tosendip[3]);
	   
	   sendARP(tosendip, 4);
	   sleep(1);
	  for(int i = 0; i < 10; i++){
	   sendICMP_RequestGlobal(tosendip, 4);
	   sleep(1);
	  }
	 }
	 break;
      case 2:
	// send arp packet to input address
	printf("ARP Request Menu\n");
	printf("Insert the desired IP address with each 3 digit address followed by an enter:\n");
	printf("For Example: 192 Enter 168 Enter... so on\n");
	scanf("%d", &tosendip[0]);
	scanf("%d", &tosendip[1]);
	scanf("%d", &tosendip[2]);
	scanf("%d", &tosendip[3]);
	printf("[+] Input ip: %d %d %d %d\n", tosendip[0], tosendip[1], tosendip[2], tosendip[3]);
	
	sendARP(tosendip, 4);
	sleep(1);
	break;
      
    }
    
    
  
  
  
}


//
// if you're going to have pthreads, you'll need some thread descriptors
//
pthread_t loop_thread, arp_thread, ip_thread, user_thread;

//
// start all the threads then step back and watch (actually, the timer
// thread will be started later, but that is invisible to us.)
//
int main()
{
	net.open_net("enp3s0");
	pthread_create(&loop_thread, NULL, protocol_loop, NULL);
	pthread_create(&arp_thread, NULL, arp_protocol_loop, NULL);
	pthread_create(&ip_thread, NULL, ip_protocol_loop, NULL);
	pthread_create(&user_thread, NULL, user_input_loop, NULL);
	for (; ; )
		sleep(1);
}
