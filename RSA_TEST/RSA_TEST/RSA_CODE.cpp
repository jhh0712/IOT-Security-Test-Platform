#include "stdafx.h"
#include "RSA_CODE.h"
#define MAX 1000

RSA_CODE::RSA_CODE()
{
}


RSA_CODE::~RSA_CODE()
{
}

 int pq = 0;
 int n = 0;
 int e = 0;

bool RSA_CODE::Is_Primes(int max)
{
	int i = 0;
	bool t = 0;
	for (i = 2; i < max; i++)
	{
		if (max % i == 0)
		{
			t = false;
			break;
		}
		else
		{
			t = true;
		}
	}
	return t;
}

int RSA_CODE::Create_Primes()
{
	int p, q = 0;
	int n = 0;
	srand((unsigned int)time(NULL));

	while (1)
	{
		while (1)
		{
			p = (rand() % MAX) + 100;

			if (Is_Primes(p))
			{
				break;
			}
		}
		while (1)
		{
			q = (rand() % MAX) + 100;

			if (Is_Primes(q))
			{
				break;
			}
		}
		if (p != q)
			break;

	}
	
	p = 659;
	q = 919;

	pq = p*q;
	n = (p - 1) * (q - 1);
	p = 0;
	q = 0;
	
	return n;
}

int RSA_CODE::Select_e(int n)
{
	int x = 0;
	int count = 0;
	int select = 0;
	int u,v,e,t = 0;
	

	srand((unsigned)time(NULL));
	
	for (x = 2; x < n; x++)
	{
		u = n;
		v = x;
		
		while (v)
		{
			t = u % v;
			u = v;
			v = t;
		}
		if (u == 1)
		{
			count++;
		}
	}

	select = (rand() % count)+1;

	count = 0;
	v = 0;
	for (x = 2; x < n; x++)
	{
		u = n;
		v = x;
		
		while (v)
		{
			t = u % v;
			u = v;
			v = t;
		}
		if (u == 1)
		{
			count++;
			if (select == count)
			{
				e = x;
				break;
			}
		}
	}
	//e = 120439;
	return e;
}

int RSA_CODE::Select_d(int e, int n)
{
	
	//mod
	int d = 0;
	
	while (1)
	{
		d++;

		if ((e * d) % n == 1)
		{
			break;
		}
	}

	return d;

}

 int RSA_CODE::Encrypt(int text)
{		
	int temp = 1;
	int count = 0;

	n = Create_Primes();

	e = Select_e(n);
	
	while (1)
	{
		temp *= text;
		temp %= pq;

		count++;

		if (count == e)
			break;
	}
		
  	return temp;
}

int RSA_CODE::Decrypt(int text)
{
	int d = 0;
	
	unsigned long long temp = 1;
	int count = 0;

	d = Select_d(e, n);
	n = 0;

	while (1)
	{
		temp *= text;
		temp %= pq;

		count++;

		if (count == d)
			break;
	}

	return temp;
}