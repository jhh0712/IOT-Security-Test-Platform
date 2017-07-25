#pragma once
class AES
{
public:
	AES(void);
	~AES(void);

	void SubBytes(unsigned char state[0x04][0x04], unsigned char S_BOX[0x10][0x10]); // S_BOX 적용
	void ShiftRows(unsigned char state[0x04][0x04]);
	void MixColumns(unsigned char state[0x04][0x04]);
	void AddRoundKey(unsigned char state[0x04][0x04], unsigned char RoundKey[0x04][0x04]); // PlainData ^ CipherKey
	void KeySchedule(unsigned char CipherKey[0x04][0x04], unsigned char S_BOX[0x10][0x10], int i); // 매 Round마다 CipherKey를 다르게 적용

	void Inv_SubBytes(unsigned char state[0x04][0x04], unsigned char S_BOX[0x10][0x10]);
	void Inv_ShiftRows(unsigned char state[0x04][0x04]);
	void Inv_MixColumns(unsigned char state[0x04][0x04]);
	void Inv_AddRoundKey(unsigned char state[0x04][0x04], unsigned char RoundKey[0x04][0x04]);
	void Inv_KeySchedule(unsigned char CipherKey[0x04][0x04], unsigned char S_BOX[0x10][0x10], int i);
	void Inv_initCipher(unsigned char CipherKey[0x04][0x04]); // Round마다 변화된 CipherKey를 연산한 후 초기상태의 CipherKey로 되돌림

	CString Encrypt(unsigned char state[0x04][0x04]);
	CString Decrypt(unsigned char state[0x04][0x04]);

	unsigned char a[0x04][0x04];
	unsigned char b[0x04][0x04];


	/*
	MYSQL *connection, conn;
	MYSQL_RES *sql_result;
	MYSQL_ROW sql_row;

	int query_state;
	char query[1024];

	RSA_CODE *m_rsa;

	CString temp_dec[16];

	int temp_int[16];
	*/

};

