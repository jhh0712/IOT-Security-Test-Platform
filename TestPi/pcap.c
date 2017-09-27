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

#define PORT port 7777		//
#define IPADDR "192.168.0.57" //IP,port 선언

#define Pay_length 50
#define Pac_length 116
#define Pac_length_ACK 66
#define Proto_RULE 3



struct ip *iph; // IP 헤더 구조체

struct tcphdr *tcph; // TCP 헤더 구조체
int counter =0;
int SYN_counter =0;
int DDos_check=0;

char *buff=NULL;//Payload 저장용


// 패킷을 받아들일경우 함수를 호출
void callback(u_char *useless, const struct pcap_pkthdr *pkthdr, const u_char *packet)
{
 
    static int count = 1;
    struct ether_header *ep; //이더넷 헤더 구조체 저장용
    unsigned short ether_type; //프로토콜 분석용
    int chcnt =0; 
    int length=pkthdr->len; //pcap라이브러리 내 패킷 길이 저장용 
    int index_i;  
    int k=0;
    int packet_length=0;
    int check_time;
    int Proto_time=0;
    int sec ;

    

    time_t curr;
    struct tm *d;

    curr = time(NULL);

    d = localtime(&curr);

    sec = d->tm_sec;

		printf("sec=%d \n",sec);

    buff = (char *)malloc(sizeof(char)*length);//동적 메모리 할당
	 
    ep = (struct ether_header *)packet; // 이더넷 헤더를 가져온다. 

    packet += sizeof(struct ether_header); // IP 헤더를 가져오기 위해서  이더넷 헤더 크기만큼 offset.

    ether_type = ntohs(ep->ether_type);  // 프로토콜 타입을 분석

    if (ether_type == ETHERTYPE_IP)   // IP 패킷일 경우 조건문 실행 
    {
	//printf("counter %d ,counter");
        // IP 헤더에서 데이터 정보를 출력
        iph = (struct ip *)packet;//들어온 패킷 저장 
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

			if(tcph->th_flags & TH_ACK)
				{
					printf("ACK ");
				}
			if(tcph->th_flags & TH_PUSH)
				{
					printf("PUSH ");
				}
			if(tcph->th_flags & TH_SYN)
				{
					printf("SYN ");
					SYN_counter++;
				}
			if(tcph->th_flags & TH_RST)
				{
					printf("RST ");
				}
			if(tcph->th_flags & TH_URG)
				{
					printf("URG ");
				}
			if(tcph->th_flags & TH_FIN)
				{
					printf("FIN ");
				}
			printf("\n\n");

			packet_length=length;
			index_i=0;

			printf("capture\n");
			while(length--)
       				{
          		  		printf("%02X  ", *(packet)); 
           				if ((++chcnt % 16) == 0) 
              				printf("\n");
									
					k++;
					if(k>52)
					{
							if(packet_length!=Pac_length_ACK)
							{
									buff[index_i] = *(packet);
							}
												
					index_i++;
					}
					
					*(packet)++;// 패킷 주소 1증가			
					
     				}	

				if(packet_length==Pac_length_ACK)
					for(index_i=0;index_i<packet_length;index_i++)
						{
							buff[index_i]=0;
						}

				printf("\n\n");
					


				chcnt=0;

				for(index_i=0;index_i<Pay_length;index_i++)
				{
					printf("%02X  ",buff[index_i]);
					if ((++chcnt % 16) == 0) 
              				printf("\n");
				}


				printf("\n\n");

				
				

				if(counter!=0)
					{
						Proto_time = sec - check_time;
					}


				DDos_check+=Proto_time;
				printf("Proto_time= %d\n",Proto_time);



				if(Proto_time==0 || Proto_time ==Proto_RULE)
					{
						printf("Protocol_time normal\n");
					}	

				else
					{
						printf("Adnormal Protocol_time Warning!\n");
					}




				check_time = sec;

				printf("%d  %d\n",counter,DDos_check);

				if(DDos_check<0 ||DDos_check>5 )
				{
					if(SYN_counter>1000)
						{
							printf("DDos_Attack_Warning!\n");

							
						}
					//DDos_check =0;
				}


				printf("Protocol normal\n");



				if(packet_length ==Pac_length || packet_length == Pac_length_ACK)
					{

						printf("Packet_length normal\n");
					}
				else
					{
						if(Proto_time!=0&&((packet_length)-52)==50)
							{
								printf("Adnormal Packet_length Warning!\n");
							}
						else
								printf("Encrypted packet detect\n");
							
						}



			
				{

				for(index_i=0;index_i<Pay_length;index_i++)
				{

						
						if((buff[index_i]==0)||(buff[index_i]<51))
							{
								if(index_i==Pay_length-1)
								printf("Payload normal\n");
							}

						
						
					else
						{
							if(Proto_time!=0&&((packet_length)-52)==50)
								{
									printf("Adnormal Payload Warning!\n");
									break;
									
								}
							else
								{
									printf("Encrypted packet detect\n");
									break;
								
								}
							
								
						}

				
				}
				}
		


					free(buff);
				counter++;
				break;
								
		case IPPROTO_UDP:
			printf("Protocol: UDP\n");
			printf("Adnormal Protocol Warning!\n");
			return;

		case IPPROTO_ICMP:

			printf("Protocol: ICMP\n");
			printf("Adnormal Protocol Warning!\n");
			return;

		case IPPROTO_IP:
			printf("Protocol: IP\n");
			printf("Adnormal Protocol Warning!\n");
			return;

		default:
			printf("Protocol: unknown\n");
			printf("Adnormal Protocol Warning!\n");
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
    char filter_exp[] = "port 7777"; //사용할 pcap 필터 내용 정의 

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