#pragma once
class RSA_CODE
{
public:
	RSA_CODE();
	~RSA_CODE();

	bool Is_Primes(int max);
	int Create_Primes();
	int Select_e(int n);
	int Select_d(int e, int n);
	int Encrypt(int text);
	void Decrypt(int *temp, int text[16]);

	void FreeFunc(int *p);

	int pq;
	int n;
	int e;

};

