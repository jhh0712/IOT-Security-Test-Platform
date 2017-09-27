#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<errno.h>
#include<arpa/inet.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<mysql/mysql.h>
#include<AES/AES.h>
#include<RSA/RSA.h>
#include<malloc.h>


#define BUFF 1024
//DB 서버 정보
#define DB_HOST "192.168.0.76"	//DB 서버 주소 (키관리서버)
#define DB_USER "root"
#define DB_PASS "issi"
#define DB_NAME "issi"
///////////////////////////

int main(void)
{
	int client_socket;	//클라이언트 소켓
	
	int i, j, k=0;
	int rand_len;	//랜덤 길이 저장

	struct sockaddr_in server_addr;		//소켓 주소 저장 

	char *buff=NULL;	//전송 데이터 저장용

	unsigned char CipherKey[0x04][0x04];		//AES 암호화 키 저장
	unsigned char init_CipherKey[0x04][0x04];	//AES 암호키 초기화용
	
	int temp_key[16] ={0,};		//CipherKey 1차원배열에 임시 저장
	int enc_key[16]={0,};		//RSA 암호화 시킨 AES 키 저장

	char temp[10] ={0,};		//RSA 암호화 된 데이터 형변환 하기 위한 임시 배열
	char aes_key[100] = {0,};	//쿼리문에 넣기 위한 배열

	int query_state;	//쿼리문 확인용

	MYSQL *connection = mysql_init(NULL);	//MYSQL 초기화
	char query[512] = {0,};		//쿼리문

	
	
	if(connection == NULL)	//MYSQL 초기화 에러시
	{
		fprintf(stderr, "%s\n", mysql_error(connection));
		return 1;
	}


	if(!mysql_real_connect(connection, DB_HOST, DB_USER, DB_PASS, DB_NAME, 3306, NULL, 0))	//MYSQL 연결 에러시
	{
		fprintf(stderr, "Mysql connection error : %s\n", mysql_error(connection));
		return 1;
	}
	printf("Mysql_connect_ON\n\n");

	client_socket = socket(PF_INET, SOCK_STREAM, 0);	//소켓 생성

	if(client_socket == -1)	//소켓 생성 실패시
	{
		printf("Socket Create Error\n");
		exit(1);
	}

	
	memset(&server_addr, 0, sizeof(server_addr));	//소켓 주소 초기화
	server_addr.sin_family = AF_INET;	//IPv4 주소체계
	server_addr.sin_port = htons(7777);	//서버 포트번호	     	
	server_addr.sin_addr.s_addr = inet_addr("192.168.0.57");	//윈도우 서버 IP주소 


	if(connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr))==-1)	//연결요청 및 실패시
	{
		printf("Connect Error\n");
		exit(1);
	}
	
	srand((unsigned int)(time(NULL)));	//난수 생성
	

	while(1)
	{

		for(i = 0; i < 4; i++)
			for(j = 0; j < 4; j++)
				CipherKey[i][j] = rand() % 255;	//CipherKey 생성

		printf("CipherKey\n");
		for(i = 0; i < 4; i++){
			for(j = 0; j < 4; j++)
				printf("%d ", CipherKey[i][j]);
			printf("\n");
		}


		k=0;

		for(i = 0; i < 4; i++)
			for(j = 0; j < 4; j++)
				temp_key[k++] = CipherKey[i][j];	//temp_key에 CipherKey 대입


		RSA_Encrypt(enc_key, temp_key);	//RSA 암호화
		
		for(i = 0; i < 16; i++)
			printf("%d ", enc_key[i]);	

		printf("\n");

		for(i = 0; i < 16; i++)
		{
			sprintf(temp, "%d", enc_key[i]);	//RSA 암호화 된 int형 데이터 Char형으로 형변환
			strcat(aes_key, temp);	//aes_key에 내용 추가
			strcat(aes_key, "!");	//바이트 구분을 위해 ! 추가
		}

		printf("\n");
		
		sprintf(query, "insert into encryptkey values('%s', '%d', '%d', '%d', now());", aes_key, pq, n, e);	//쿼리문 저장
		
		query_state = mysql_query(connection, query);	//쿼리문 실행
		if(query_state != 0){	//쿼리문 에러시
			printf("mysql_query error\n");
		}
	

		rand_len = (rand() % 255)+1;	//send 데이터 길이설정 (원하는 길이 설정)

		buff = (char *)malloc(sizeof(char)*rand_len);	//동적 메모리할당
	
		//printf("len = %d\n", rand_len);

		for(i=0; i<rand_len; i++)
		{
	
			buff[i] = i+1;	//데이터 값 설정
			
		}
		
		k=0;
		
		int count = 0;
		int count2 = 0;
		int m = 0;

		int flag = 0;

		count = rand_len / 16; 	//3차원 배열 크기 지정 위해 사용
		count2 = rand_len % 16;	//3차원 배열 크기 지정 위해 사용

		if(count2 != 0)	//나머지가 있으면 배열크기 증가시켜야함
			count++;

		unsigned char state[1024][4][4] = {0,};	//AES 암호화 하기 위한 상태값 저장


		for(m=0; m<count; m++)
		{
			for(i=0; i<4; i++)
			{
				for(j=0; j<4; j++)
				{	
					state[m][i][j] = buff[k++];	//3차원 state 에 데이터값 대입

					if(k == rand_len)
					{	
						flag = 1;
						break;
					}
				}
				if(flag == 1)
					break;
			}
			if(flag == 1)
				break;
		}

				
		char buff2[1024][16] = {0,};
	

		for(i=0; i<4; i++)
				for(j=0; j<4; j++)
					init_CipherKey[i][j] = CipherKey[i][j];	//CipherKey가 변화하기 때문에 init_CipherKey에 저장

		for(m=0; m<count; m++)
		{
			Encrypt(buff2[m], state[m], CipherKey);	//AES 암호화

			for(i=0; i<4; i++)
				for(j=0; j<4; j++)
					CipherKey[i][j] = init_CipherKey[i][j];	//CipherKey를 원래 키로 다시 되돌려줌
		}

		
		printf("Send Data\n");
		
		for(i =0; i<count; i++)
		{
			for(j = 0; j < 16; j++){
				printf("%d ", buff2[i][j]);
			}
			printf("\n");
		}
		
		char buff3[8192] = {0,};

		k = 0;

		for(i =0; i<count; i++)
			for(j = 0; j < 16; j++)
				buff3[k++] = buff2[i][j];	//소켓통신을 위해 1차원 버퍼에 2차원 암호화 데이터 대입
						
		int socket_state = 0;

		//printf("Send_status = %d\n",send(client_socket, buff2, 16, 0));
		
		socket_state = send(client_socket, buff3, k, 0); //AES 암호화 시킨 데이터 전송
		
		if(socket_state == -1)
			break;

		printf("\n");
		for(i = 0; i < 4; i++)
			for(j = 0; j < 4; j++)
				CipherKey[i][j] = 0;	//CipherKey 초기화

		free(buff);	//메모리해제

		sleep(3);	//3초 딜레이
		
		memset(query, 0, 512);
		memset(aes_key, 0, 100);
		memset(temp, 0, 10);

	}

	mysql_close(connection);	//MYSQL 연결해제
	close(client_socket);		//소켓 해제
	
	return 0;
}