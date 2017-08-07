#include "stdafx.h"
#include "RSA_CODE.h"

#define MAX 100

RSA_CODE::RSA_CODE()
{
}


RSA_CODE::~RSA_CODE()
{
}

int pq = 0;
int n = 0;
int e = 0;

// �Ҽ����� �Ǵ��ϴ� �Լ�
bool RSA_CODE::Is_Primes(int max)
{
	int i = 0;
	bool t = 0;
	for (i = 2; i < max; i++)
	{
		if (max % i == 0) // 2���� �Է� ���� ������ �������� �������� 0�� ���
		{
			t = false;	// �� ������ �������ٴ� ���� �Ҽ��ƴ�
			break;	
		}
		else
		{
			t = true;	// ������ �������� ������ ����� 1�� �Էµ� �� �̹Ƿ� �Ҽ�
		}
	}
	return t;	
}

// �������� �Ҽ������ �Լ�
int RSA_CODE::Create_Primes()
{
	int p, q = 0;
	int n = 0;
	srand((unsigned int)time(NULL));

	while (1)
	{
		while (1)
		{
			p = (rand() % MAX) + 10;	//10�ڸ� �̻��� p�� ����

			if (Is_Primes(p))	//p�� �Ҽ����� �Ǵ�
			{
				break;	//�Ҽ��̸� ��������
			}
		}
		while (1)
		{
			q = (rand() % MAX) + 10;	//10�ڸ� �̻��� q�� ����

			if (Is_Primes(q))	//q�� �Ҽ����� �Ǵ�
			{
				break;	//�Ҽ��̸� ��������
			}
		}
		if (p != q)	//p�� q�� ��ġ�����ʴ� ���
			break;	//�ݺ������� ��������

	}

	pq = p*q;	//pq�� p, q�� ��
	n = (p - 1) * (q - 1);	//n�� ���Ϸ� ���Լ� n���� ����
	p = 0;
	q = 0;	//p,q �ʱ�ȭ

	return n;	
}

//e �� ���ϴ� �Լ�
int RSA_CODE::Select_e(int n)
{
	int x = 0;
	int count = 0;
	int select = 0;
	int u, v, e, t = 0;


	srand((unsigned)time(NULL));

	for (x = 2; x < n; x++)	
	{
		u = n;
		v = x;

		while (v) //��Ŭ���� ȣ������ �̿��� �ִ����� ���ϴ� �˰���
		{
			t = u % v;
			u = v;
			v = t;
		}
		if (u == 1)	//�ִ� ������� 1�̸� ���μ��̹Ƿ� ī��Ʈ�� ����
		{
			count++;
		}
	}

	select = rand() % count;	//���μ��� �ϳ��� �������� �����ϱ� ���� ī��Ʈ�� ���Ƿ� ����

	count = 0;
	v = 0;
	for (x = 2; x < n; x++)	//�ٽ� ���μҸ� ����
	{
		u = n;
		v = x;

		while (v)	//��Ŭ���� ȣ���� �˰���
		{
			t = u % v;
			u = v;
			v = t;
		}
		if (u == 1)	//���μ��� ���
		{
			count++;
			if (select == count)	//���Ƿ� ������ ���� ī��Ʈ�� ������
			{
				e = x;	//e���� �����ϰ� ��������
				break;
			}
		}
	}

	return e;
}

//d �� ���ϴ� �Լ�
int RSA_CODE::Select_d(int e, int n)
{

	//mod
	int d = 0;

	while (1)
	{
		d++;

		if ((e * d) % n == 1)	// ��ⷯ ������ ���� d�� ���� ( ed(mod(���Ϸ� ���Լ�)) = 1 �� d�� ���ϴ� ���� )
		{
			break;
		}
	}
	return d;

}

//��ȣȭ �Լ�
int RSA_CODE::Encrypt(int text)
{
	int temp = 1;
	int count = 0;

	n = Create_Primes();	// ���Ϸ� ���Լ� ����

	e = Select_e(n);	// e�� ����

	while (1)
	{
		temp *= text;	// �� ����
		temp %= pq;	//��ⷯ ����

		count++;

		if (count == e)	// e��ŭ �����ϸ� ��������
			break;
	}

	return temp;
}

//��ȣȭ �Լ�
int RSA_CODE::Decrypt(int text)
{
	int d = 0;

	unsigned long long temp = 1;
	int count = 0;

	d = Select_d(e, n);	//d�� ����
	n = 0;

	while (1)
	{
		temp *= text;	// ��ȣ�� ����
		temp %= pq;	// ��ⷯ ����

		count++;

		if (count == d)	// d��ŭ �����ϸ� ��������
			break;
	}

	return temp;
}