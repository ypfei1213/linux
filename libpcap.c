#include <stdio.h>
#include <stdlib.h>
#include <pcap.h> 
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include <arpa/inet.h>




#define ETHER_ADDR_LEN 6
#define ETHERTYPE_IP  0x0800 
#define ETHERTYPE_ARP  0x0806 
#define ETHERTYPE_REVARP 0x8035 
#define ETHERTYPE_VLAN  0x8100 
#define ETHERTYPE_IPV6  0x86dd 
#define ETHERTYPE_LOOPBACK 0x9000



//IP地址格式
struct ip_header{  
	#ifdef WORKS_BIGENDIAN
        u_int8_t ip_version:4,  //version:
        ip_header_length:4; //IP协议首部长度
    #else
        u_int8_t ip_header_length:4,
        ip_version:4;
    #endif
//  u_int8_t ip_version: 4,ip_header_length: 4;  //IP协议首部长度
	u_int8_t ip_tos;  //TOS服务质量 
	u_int16_t ip_length;  //总长度
	u_int16_t ip_id;  //标识
	u_int16_t ip_off;  //偏移
	u_int8_t ip_ttl;  //生存时间
	u_int8_t ip_protocol; //协议类型  
	u_int16_t ip_checksum;  //校验和
	struct in_addr ip_source_address; //源IP  
	struct in_addr ip_destination_address;  //目的IP
};


//关于tcp头部的定义
struct tcp_header{  
	u_int16_t sport;  //源端口
	u_int16_t dport;  //目的端口
	u_int32_t sn;     //序号
	u_int32_t ack;    //应答号
	//  u_int16_t other;
	#if BYTE_ORDER == LITTLE_ENDIAN  
	u_char th_x2:4,th_off:4;  
	#endif  
	#if BYTE_ORDER == BIG_ENDIAN
	u_char th_off:4,th_x2:4;  
	#endif
	u_char th_flags;  
	#define TH_FIN 0x01
	#define TH_SYN 0x02  
	#define TH_RST 0x04  
	#define TH_PUSH 0x08  
	#define TH_ACK 0x10  
	#define TH_URG 0x20  
 
	u_int16_t win_size;  
	u_int16_t checksum;  
	u_int16_t urg_ptr;  
};


//关于udp头部的定义
struct udp_header{  
	uint16_t sport;       //源端口
	uint16_t dport;       //目的端口 
	uint16_t udp_length;  //总长度
	uint16_t udp_sum;     //校验和  
};



u_char etherbroadcastaddr[6] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };


//以太网的协议格式
struct   ether_header{
	u_char    ether_dhost[ETHER_ADDR_LEN]; //目的以太地址
	u_char    ether_shost[ETHER_ADDR_LEN]; //源以太网地址
	u_short   ether_type; //以太网类型
};


//udp包分析函数
void udp_protocol_packet_callback(u_char *param,const struct pcap_pkthdr *header,const u_char *pkt_data){
	struct   udp_header *udp_protocol;
	uint16_t sourport;
	uint16_t destport;
	uint16_t _udplength;
	uint16_t checksum;

	udp_protocol=(struct udp_header*)(pkt_data + 14 + 20);
	sourport   = ntohs(udp_protocol->sport);
	destport   = ntohs(udp_protocol->dport);
	_udplength = ntohs(udp_protocol->sport);
	checksum   = ntohs(udp_protocol->sport);
	printf("UDP包----------------------------");
	printf("源端口   %d\n",sourport);
	printf("目的端口 %d\n",destport);
	printf("长度     %d\n",sourport);
	printf("校验码   %d\n",sourport);

	printf("完整包------------------------\n");
	printf("链路层------------------------\n");
	for(int i=0;i<header->len;i++){
		printf("%02x ",*(pkt_data+i));
		if((i==5)||i==11)
			printf("\n");
		if(i==13)
			printf("\nIP层----------------------\n");
		if(i==17||i==21||i==25||i==29||i==37)
			printf("\n");
		if(i==33)
			printf("\nUDP层----------------------\n");
		if(i==41)
			printf("\n内容-----------------------\n");
	}
	printf("\n");

}


//tcp数据包分析函数
void tcp_protocol_packet_callback(u_char *param,const struct pcap_pkthdr *header,const u_char *pkt_data){
	struct tcp_header *tcp_protocol;  //tcp协议变量
	tcp_protocol=(struct tcp_header*)(pkt_data + 14 + 20);

	u_short    source_port      = ntohs(tcp_protocol->sport);  //源端口
	u_short    destination_port = ntohs(tcp_protocol->dport);  //目的端口
	u_int      sequence			= ntohl(tcp_protocol->sn);  //序列号
	u_int      acknowledgement  = ntohl(tcp_protocol->ack);  //确认号
	u_char     flags			= tcp_protocol->th_flags; //标记  
	u_int16_t  win_size			= ntohl(tcp_protocol->win_size);  //窗口大小
	u_int16_t  checksum			= ntohl(tcp_protocol->checksum);  //校验和

	printf("TCP包---------------------------");
	printf("Tcp Source Port:%d\n",source_port);  
	printf("Tcp Destination Port:%d\n",destination_port);  
	printf("Sequence Number:%u\n",sequence);  
	printf("Acknowledgement Number:%u\n",acknowledgement);  
	printf("Flags:%02x ",flags);  
	if(flags & 0x08)   printf("PSH ");  
	if(flags & 0x10)   printf("ACK ");  
	if(flags & 0x02)   printf("SYN ");  
	if(flags & 0x20)   printf("URG ");  
	if(flags & 0x01)   printf("FIN ");  
	if(flags & 0x04)   printf("RST ");  
	printf("\n"); 

	printf("完整包------------------------\n");
	printf("链路层------------------------\n");
	for(int i=0;i<header->len;i++){
		printf("%02x ",*(pkt_data+i));
		if((i==5)||i==11)
			printf("\n");
		if(i==13)
			printf("\nIP层----------------------\n");
		if(i==17||i==21||i==25||i==29||i==37||i==41||i==45|i==49||i==57||i==61||i==65||i==69)
			printf("\n");
		if(i==33)
			printf("\nTCP层----------------------\n");
		if(i==53)
			printf("\n内容-----------------------\n");
	}
	printf("\n");
}


//ip数据包分析函数
void ip_protocol_packet_callback(u_char *param,const struct pcap_pkthdr *header,const u_char *pkt_data){
	struct ip_header *ip_protocol;
	u_int header_len;
	u_int offset;
	u_char tos;
	u_int16_t checksum;

	ip_protocol=(struct ip_header *)(pkt_data+14);
	checksum=ntohs(ip_protocol->ip_checksum);  
	header_len = ip_protocol->ip_header_length * 4;  
	tos=ip_protocol->ip_tos;                       
	offset=ntohs(ip_protocol->ip_off);

	printf("IP Version:%d\n",ip_protocol->ip_version);  
	printf("Header length:%d\n",header_len);    
	printf("TOS:%d\n",tos);                        
	printf("Total length:%d\n",ntohs(ip_protocol->ip_length));  
	printf("Identification:%d\n",ntohs(ip_protocol->ip_id));  
	printf("Offset:%d\n",(offset &0x1fff) *8);     
	printf("TTL:%d\n",ip_protocol->ip_ttl);        
	printf("Protocol:%d\n",ip_protocol->ip_protocol);  
	switch (ip_protocol->ip_protocol)              
	{                                              
		case 6:                                      
			printf("The Transport Layer Protocol is TCP\n");  
			break;                                     
		case 17:                                     
			printf("The Transport Layer Protocol is UDP\n");  
			break;                                     
		case 1:                                      
			printf("The Transport Layer Protocol is ICMP\n");  
			break;                                     
		default:                                     
			break;                                     
	}                                              
	printf("Header checksum:%d\n",checksum);       
	printf("Source address:%s\n",inet_ntoa(ip_protocol->ip_source_address));  
	printf("Destination address:%s\n",inet_ntoa(ip_protocol->ip_destination_address));  
	switch(ip_protocol->ip_protocol){  
		case 6:  
			tcp_protocol_packet_callback(param,header,pkt_data);  
			break;
		case 17:
			udp_protocol_packet_callback(param,header,pkt_data); 
		default:  
			break;  
	}  
}



void callback(u_char *param, const struct pcap_pkthdr *header, const u_char *pkt_data){
	struct tm *ltime;
	u_char *mac;
	u_short ethernet_type;
	char timestr[16];
	time_t local_tv_sec;
	struct ether_header *protocol;
	//抓取时间
	local_tv_sec = header->ts.tv_sec;
	printf("%s getlen %d len:%d\n", ctime(&local_tv_sec),header->caplen, header->len);

	printf("以太包头---------------------------");
	protocol=(struct ether_header *)pkt_data;
	mac = (u_char *)protocol->ether_shost;
	ethernet_type=ntohs(protocol->ether_type);
	switch(ethernet_type){
		case ETHERTYPE_IP:
			printf("The network layer is IP protocol\n");
			break;
		case ETHERTYPE_ARP:
			printf("The network layer is ARP protocol\n");
			break;
		case ETHERTYPE_REVARP:
			printf("The network layer is RARP protocol\n");
			break;
		default:
			//printf("The network layer is %04x\n",ethernet_type);
			break;
	}
	printf("源标mac %02x %02x %02x %02x %02x %02x",*mac,*(mac+1),*(mac+2),*(mac+3),*(mac+4),*(mac+5));
	mac = (u_char *)protocol->ether_dhost;
	printf("源标mac %02x %02x %02x %02x %02x %02x\n",*mac,*(mac+1),*(mac+2),*(mac+3),*(mac+4),*(mac+5));
	switch(ethernet_type){
		case 0x0800:
			ip_protocol_packet_callback(param,header,pkt_data);
			break;
	}
}



int main(int argc, char **argv){
	char errBuf[PCAP_ERRBUF_SIZE], * device;
	char *net;
	char *mask;
	bpf_u_int32 netp;      //IP
	bpf_u_int32 maskp;     //网络掩码
	struct in_addr addr;   //表示32位的IPV4
	struct pcap_pkthdr hdr; //包的信息
	const u_char *packet;
	pcap_t *descr;  //捕获网络数据包的数据包捕获描述字  打开网卡
	int ret;
	struct ether_header *eptr;
	char filter_buf[]="port 9010";
	struct bpf_program filter;

	device= pcap_lookupdev(errBuf); //查找网络设备
    ret = pcap_lookupnet(device,&netp,&maskp,errBuf); //获取网络号和掩码
	if(ret==-1){
		printf("look up net error ");
	}
	addr.s_addr=netp;
	net = inet_ntoa(addr);
	if(device){  
		printf("success: device: %s\n", device);  
		printf("net %s\n",net);
	}
	else{  
		printf("error: %s\n", errBuf);  
	}
	addr.s_addr=maskp;
	mask = inet_ntoa(addr);
	printf("mask %s\n",mask);
	
	descr=pcap_open_live(device,BUFSIZ,1,0,errBuf); //获取数据包捕获描述字
    if(descr==NULL){
		printf("error");
	}
	//过滤
	pcap_compile(descr, &filter, "port 9010", 1, 0); //编译字串至过滤程序
	pcap_setfilter(descr, &filter); //指定过滤程序

	pcap_loop(descr,1,callback,NULL); //捕获和处理数据包
	if(packet==NULL){
		printf("error");
	}
	pcap_close(descr); //关闭
	return 0;
}


