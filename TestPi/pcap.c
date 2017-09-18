#include <sys/time.h>
#include <netinet/in.h>
#include <net/ethernet.h>
#include <pcap/pcap.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <netinet/ip_icmp.h>
#include<time.h>
#include<sys/socket.h>

#define PROMISCUOUS 1
#define NONPROMISCUOUS 0

#define PORT 8000		//
#define IPADDR "220.69.207.111" //IP,port 선언


struct ip *iph; // IP 헤더 구조체


struct tcphdr *tcph; // TCP 헤더 구조체


// 패킷을 받아들일경우 함수를 호출
void callback(u_char *useless, const struct pcap_pkthdr *pkthdr, 
                const u_char *packet)
{
    static int count = 1;
    struct ether_header *ep; //이더넷 헤더 구조체 저장용
    unsigned short ether_type; //프로토콜 분석용
    int chcnt =0; 
    int length=pkthdr->len; //패킷 길이 저장용 
    int index_i;  
 
    ep = (struct ether_header *)packet; // 이더넷 헤더를 가져온다. 

    
  
    packet += sizeof(struct ether_header); // IP 헤더를 가져오기 위해서  이더넷 헤더 크기만큼 offset.

  
    ether_type = ntohs(ep->ether_type);  // 프로토콜 타입을 분석

 
    if (ether_type == ETHERTYPE_IP)   // IP 패킷일 경우 조건문 실행 
    {


        // IP 헤더에서 데이터 정보를 출력
        iph = (struct ip *)packet;//들어온 패킷 저장 
        printf("IP 패킷\n");
        printf("Version     : %d\n", iph->ip_v); 
        printf("Header Len  : %d\n", iph->ip_hl);
        printf("Ident       : %d\n", ntohs(iph->ip_id));
        printf("TTL         : %d\n", iph->ip_ttl); 
        printf("Src Address : %s\n", inet_ntoa(iph->ip_src));
        printf("Dst Address : %s\n", inet_ntoa(iph->ip_dst));

	printf("Src MAC address : ");  
 		for(index_i=0; index_i<ETH_ALEN; index_i++) //src_ip의 mac 주소 출력
 		{
  			printf("%02X", ep->ether_shost[index_i]);
  
  			if(index_i < (ETH_ALEN-1))
   			printf(":");
  			else
  			printf("\n");
 		}
 
 	printf("Dst MAC address : ");  
 		for(index_i=0; index_i<ETH_ALEN; index_i++)//dst_ip의 mac 주소 출력
 		{
  			printf("%02X", ep->ether_dhost[index_i]);
  			if(index_i < (ETH_ALEN-1))
   			printf(":");
  			else
   			printf("\n");
 		}

	switch(iph->ip_p){//패킷의 프로토콜 분석하여 정보 출력

		case IPPROTO_TCP:
			tcph = (struct tcphdr *)(packet + iph->ip_hl * 4);
			printf("Protocol: TCP\n");
            		printf("Src Port : %d\n" , ntohs(tcph->source));
           		printf("Dst Port : %d\n" , ntohs(tcph->dest));
			printf("packet Length : %d\n", length);
			int k = 0;
			printf("capture\n");
			  while(length--)
       				{
          		  		printf("%02x  ", *(packet)); 
           				if ((++chcnt % 16) == 0) 
              				printf("\n");
					*(packet)++;// 패킷 주소 1증가
					k++;
					
     				}
				break;
					
	

				
		case IPPROTO_UDP:
			printf("Protocol: UDP\n");
			return;

		case IPPROTO_ICMP:
			printf("Protocol: ICMP\n");
			return;

		case IPPROTO_IP:
			printf("Protocol: IP\n");
			return;

		default:
			printf("Protocol: unknown\n");
			return;

	}	

  	 printf("\n\n");
	}  
		
	}
	
int main(int argc, char **argv)
{
    char *dev;	//사용하는 디바이스 이름 저장용
    char *net;	//사용하는 네트워크 정보 저장용
    char *mask;	//사용하는 마스크 정보 저장용
    char filter_exp[] = "port 8000"; //사용할 pcap 필터 내용 정의 

    bpf_u_int32 netp; //network 디바이스의 네트워크 주소가 저장
    bpf_u_int32 maskp;//network 디바이스의 넷마스크 주소가 저장
    char errbuf[PCAP_ERRBUF_SIZE];//에러메세지 저장 
    int ret;
    struct pcap_pkthdr hdr;
    struct in_addr net_addr, mask_addr; // 
    struct ether_header *eptr;//
    const u_char *packet; // 실제 패킷 저장용 

    struct bpf_program fp;     

    pcap_t *pcd;  // packet capture descriptor

    
    dev = pcap_lookupdev(errbuf); // 사용중인 디바이스 이름을 얻어온다. 
    if (dev == NULL)
    {
        printf("%s\n", errbuf);
        exit(1);
    }
    printf("DEV : %s\n", dev);


   
    ret = pcap_lookupnet(dev, &netp, &maskp, errbuf);  // 디바이스 이름에 대한 네트웍/마스크 정보를 얻어온다. 
    if (ret == -1)
    {
        printf("%s\n", errbuf);
        exit(1);
    }
	printf("Filter expression: %s\n", filter_exp);

   
    net_addr.s_addr = netp;  // network 정보  출력을 위해 변환. 
    net = inet_ntoa(net_addr);// network 정보인 32비트값을 Dotted Decimal Notation 주소값으로 변환
    printf("NET : %s\n", net);

    mask_addr.s_addr = maskp; // mask 정보 출력을 위해 변환
    mask = inet_ntoa(mask_addr);// mask 정보인 32비트값을 Dotted Decimal Notation 주소값으로 변환
    printf("MSK : %s\n", mask);
    printf("=======================\n");

      
    pcd = pcap_open_live(dev, BUFSIZ,  NONPROMISCUOUS, -1, errbuf); // 디바이스 dev 에 대한 packet capture descriptor 를얻어온다. 
    if (pcd == NULL)
    {
        printf("%s\n", errbuf);
        exit(1);
    }    

    
    if (pcap_compile(pcd, &fp, filter_exp, 0, netp) == -1)// 컴파일 옵션 설정/ 패킷필터 및 반복 횟수 등을 설정 .
    {
        printf("compile error\n");    
        exit(1);
    }
    
    if (pcap_setfilter(pcd, &fp) == -1) // 컴파일 옵션대로 패킷필터 룰을 세팅하는 함수.
    {
        printf("setfilter error\n");
        exit(0);    
    }

         // 지정된 횟수만큼 패킷캡쳐를 한다. 
    // pcap_setfilter 을 통과한 패킷이 들어올경우 
    // callback 함수를 호출하도록 한다. 
    pcap_loop(pcd, 0, callback, NULL);



}