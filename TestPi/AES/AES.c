#include<stdio.h>
#include<stdlib.h>
#include<AES/AES.h>






int main()
{

	char *temp = (char *)malloc(sizeof(char)* 16);

	unsigned char a[0x04][0x04];
	unsigned char b[0x04][0x04];

	int i, j;
	int k = 0;

	char *enc;
	//char *dec = (char *)malloc(sizeof(char)* 16);
	char *dec;

	printf("INPUT:");
	scanf("%s",temp);
	

	for (i = 0; i<4; i++)
	{
		for (j = 0; j<4; j++)
		{
			a[i][j] = 0;
			b[i][j] = 0;
		}
	}



	for (i = 0; i<4; i++)
	{
		for (j = 0; j<4; j++)
		{
			a[i][j] = temp[k++];
			b[i][j] = 0;
		}
	}


	printf("문자:%s\n", temp);

	enc = Encrypt(a);
	printf("Encrypt : %s\n", enc);

	k = 0;

	for (i = 0; i<4; i++)
	{
		for (j = 0; j<4; j++)
		{
			a[i][j] = 0;
			b[i][j] = enc[k++];
		}
	}

	printf("\n");

	dec = Decrypt(b);

	printf("Decrypt : %s\n", dec);
/*
	printf("Decrypt :");
	for (i = 0; i<4; i++)
	{
		for (j = 0; j<4; j++)
		{
			printf("%c  ", b[i][j]);
		}
	}
	*/

	free(enc);
	free(dec);
	free(temp);


	system("pause");
	return 0;

}
