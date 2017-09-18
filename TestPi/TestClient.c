#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<errno.h>
#include<arpa/inet.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<malloc.h>




int main(void)
{
	int client_socket;	//클라이언트 소켓
	
	int i, j, k=0;
	int rand_len;	//랜덤 길이 저장

	struct sockaddr_in server_addr;		//소켓 주소 저장 

	char *buff=NULL;	//전송 데이터 저장용

	client_socket = socket(PF_INET, SOCK_STREAM, 0);	//소켓 생성

	if(client_socket == -1)	//소켓 생성 실패시
	{
		printf("Socket Create Error\n");
		exit(1);
	}

	
	memset(&server_addr, 0, sizeof(server_addr));	//소켓 주소 초기화
	server_addr.sin_family = AF_INET;	//IPv4 주소체계
	server_addr.sin_port = htons(7777);	//서버 포트번호	     	
	server_addr.sin_addr.s_addr = inet_addr("192.168.0.73");	//윈도우 서버 IP주소 


	if(connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr))==-1)	//연결요청 및 실패시
	{
		printf("Connect Error\n");
		exit(1);
	}
	
	srand((unsigned int)(time(NULL)));	//난수 생성
	

	while(1)
	{

		rand_len = (rand() % 255)+1;	//send 데이터 길이설정 (원하는 길이 설정)

		buff = (char *)malloc(sizeof(char)*rand_len);	//동적 메모리할당
	
		//printf("len = %d\n", rand_len);

		for(i=0; i<rand_len; i++)
		{
	
			buff[i] = i+1;	//데이터 값 설정
			
		}
		
		
		
		int socket_state = 0;

		printf("length=%d\n", rand_len);
		//printf("Send_status = %d\n",send(client_socket, buff2, 16, 0));
		printf("Send_data\n");

		for(i=0; i<rand_len; i++)
		{
			printf("%d\t", buff[i]);
			k++;

			if(k%8 ==0)
				printf("\n");
		}
		printf("\n");
		socket_state = send(client_socket, buff, rand_len, 0); //데이터 전송
		
		if(socket_state == -1)
			break;

		
		free(buff);	//메모리해제

		sleep(2);	//2초 딜레이

		
	}

	close(client_socket);		//소켓 해제
	
	return 0;
}