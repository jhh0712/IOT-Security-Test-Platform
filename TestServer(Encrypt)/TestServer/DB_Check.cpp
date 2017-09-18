// DB_Check.cpp : implementation file
//

#include "stdafx.h"
#include "TestServer.h"
#include "DB_Check.h"
#include "afxdialogex.h"

#define SQL_SELECT2 "select date_format(date, '%Y-%m-%d') ymd from data group by date_format(date, '%Y%m%d') order by date desc"


// CDB_Check dialog

IMPLEMENT_DYNAMIC(CDB_Check, CDialogEx)

CDB_Check::CDB_Check(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DBCHECK, pParent)
{
	for (int i = 0; i < 16; i++)
		temp_dec[i] = NULL;	//CString �迭 �ʱ�ȭ

	for (int i = 0; i < 4096; i++)
		temp_dec2[i] = NULL;	//CString �迭 �ʱ�ȭ
}

CDB_Check::~CDB_Check()
{
}

void CDB_Check::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, DB_LIST);
	DDX_Control(pDX, IDC_DBDATE, DB_DATE);
}


BEGIN_MESSAGE_MAP(CDB_Check, CDialogEx)
	ON_BN_CLICKED(IDC_DB, &CDB_Check::OnBnClickedDb)
	ON_CBN_SELCHANGE(IDC_DBDATE, &CDB_Check::OnCbnSelchangeDbdate)
END_MESSAGE_MAP()


BOOL CDB_Check::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here

	click_count = 0;	//Ŭ�� ī��Ʈ �ʱ�ȭ


	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			CipherKey[i][j] = 0;	//CipherKey�� �ʱ�ȭ


	for (int m = 0; m < 1024; m++)
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				State[m][i][j] = 0;		//State�� �ʱ�ȭ


	m_aes = new AES();//(AES*)AfxGetApp();
	m_rsa = new RSA_CODE();

	connection = NULL;
	mysql_init(&conn);	//Mysql �ʱ�ȭ

	connection = mysql_real_connect(&conn, DB_HOST, USER_NAME, USER_PASS, DB_NAME, 3306, (char *)NULL, 0);	//Mysql DB������ ����õ�

	if (connection == NULL)	//������� �� �����޽��� ���
	{
		AfxMessageBox(_T("DB CONNECTION ERROR"));
	}

	connection2 = NULL;
	mysql_init(&conn2);	//Mysql �ʱ�ȭ

	connection2 = mysql_real_connect(&conn2, DB_HOST2, USER_NAME2, USER_PASS2, DB_NAME2, 3306, (char *)NULL, 0);	//Mysql DB������ ����õ�

	if (connection2 == NULL)	//������� �� �����޽��� ���
	{
		AfxMessageBox(_T("KEY MANAGEMENT DB CONNECTION ERROR"));
	}


	DB_LIST.InsertColumn(1, _T("DATA"), LVCFMT_LEFT, 650);		//List Control �ʱ�ȭ
	DB_LIST.InsertColumn(2, _T("DATE"), LVCFMT_LEFT, 120);	//List Control �ʱ�ȭ

	p1 = NULL;
	p2 = NULL;	//������ ������ �ʱ�ȭ

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


// CDB_Check message handlers

UINT CDB_Check::ThreadFirst(LPVOID _mothod)
{
	CDB_Check *fir = (CDB_Check*)_mothod;

	fir->ThreadFunc();	//�����忡�� ������ �Լ�
	
	return 0;

}

UINT CDB_Check::ThreadSecond(LPVOID _mothod)
{
	CDB_Check *fir = (CDB_Check*)_mothod;

	fir->ThreadFunc2();	//�����忡�� ������ �Լ�

	return 0;

}

void CDB_Check::ThreadFunc()
{
	DB_LIST.DeleteAllItems();

	int k = 0;

	dec[16] = { 0, };	//AES Ű ��ȣȭ��

	dec2[4096] = { 0, }; //AES ��ȣ ������ ��ȣȭ��

	int Decrypt_RSA[16] = { 0, };	//RSA ��ȣȭ�� ���� �迭
	char Decrypt_AES[1024][16] = { 0, };	//AES ��ȣȭ�� ���� 2���� �迭

	query_state = mysql_query(connection, SQL_SELECT2);	//���� ����

	if (query_state != 0)	//������ ���� �����޽��� ���
		AfxMessageBox("Query Select2 Error!");

	sql_result = mysql_store_result(connection);	//���� ������ ����

	if (click_count == 0)	//ù ��ư Ŭ����
	{
		while ((sql_row = mysql_fetch_row(sql_result)) != NULL)	//���� ������ �� ������
		{
			DB_DATE.AddString(sql_row[0]);	//�޺��ڽ��� �����߰�
		}
		DB_DATE.SetCurSel(0);
		click_count++;	//ī��Ʈ ����
	}

	CString str = "";
	int n;
	n = DB_DATE.GetLBTextLen(DB_DATE.GetCurSel());	//��Ʈ�� ���̸� ����
	DB_DATE.GetLBText(DB_DATE.GetCurSel(), str.GetBuffer(n));	//�޺� �ڽ��� ���� ��Ʈ�� ������
	str.ReleaseBuffer();	//��������

	query_state = mysql_query(connection, "SELECT * FROM data WHERE date LIKE '" + str + "%'");	//������ �޺��ڽ� ������ �Է� �� ����

	if (query_state != 0)	//������ ���� �����޽��� ���
		AfxMessageBox("Query Select3 Error!");

	sql_result = mysql_store_result(connection);	//���� ������ ����

	int list_count = 0;	//����Ʈ ���� ī��Ʈ

	while ((sql_row = mysql_fetch_row(sql_result)) != NULL)	//���� ������ �� ������
	{
		for (int i = 0; i < 16; i++)
		{
			temp_dec[i] = new CString();
			*temp_dec[i] = "";	//CString �ʱ�ȭ
		}

		CString date = "";
		date = sql_row[1];	//������ date �� ����

		query_state2 = mysql_query(connection2, "SELECT * FROM encryptkey WHERE date LIKE '" + date + "%'");	//date �� �̿��� RSA ��ȣȭ�� Ű�� ���� ���� ����

		sql_result2 = mysql_store_result(connection2);	//���� ������ ����

		while ((sql_row2 = mysql_fetch_row(sql_result2)) != NULL)	//���� ������ �� ������
		{
			temp_aes = sql_row2[0];	//!���� ��ȣȭ�� AESŰ

			temp_pq = sql_row2[1];	//RSA pq��
			temp_n = sql_row2[2];	//RSA n��
			temp_e = sql_row2[3];	//RSA e��
		}


		//int������ ����ȯ//////////
		m_rsa->pq = _ttoi(temp_pq);
		m_rsa->n = _ttoi(temp_n);
		m_rsa->e = _ttoi(temp_e);
		//////////////////////////

		char* st = LPSTR(LPCTSTR(temp_aes));	//!�и��ϱ� ���� char�� �迭�� ����ȯ
		k = 0;
		for (int i = 0; i < temp_aes.GetLength(); i++)
		{
			if (st[i] != '!')	//�迭���� !�� ������
			{
				*temp_dec[k] += st[i];	//CString�迭�� �� �߰�
			}
			else	//!�� ������
			{
				if (i == 0)
					k = 0;
				else
					k++;	//k ����
			}
		}

		for (int i = 0; i < 16; i++)
			dec[i] = _ttoi(*temp_dec[i]);	//int�� �迭�� CString���� �и��� �����Ͱ� ����ȯ �� ����

		m_rsa->Decrypt(Decrypt_RSA, dec);	//RSA ��ȣȭ

		k = 0;

		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				CipherKey[i][j] = Decrypt_RSA[k++];	//��ȣȭ�� Ű���� CipherKey�� ����

		int count2 = 0;	//@���� (3���� �迭 ���鰳��)
		int count3 = 0; //@�� !���� (CString �迭 ���鰳��)

		CString enc_data = "";
		enc_data = sql_row[0];	//��ȣȭ�� �����Ͱ� ������

		char* st2 = LPSTR(LPCTSTR(enc_data));	//!�� @�и��ϱ����� char�� �迭�� ��ȯ

		for (int i = 0; i < enc_data.GetLength(); i++)
		{
			if (st2[i] == '@')
			{
				count2++;	//@���� ī��Ʈ
				count3++;	//@�� !���� ī��Ʈ
			}

			if (st2[i] == '!')
				count3++;	//@�� !���� ī��Ʈ
		}

		for (int i = 0; i < 4096; i++)
		{
			temp_dec2[i] = new CString();
			*temp_dec2[i] = "";	//CString �迭 �ʱ�ȭ
		}

		k = 0;

		for (int i = 0; i < enc_data.GetLength(); i++)
		{
			if (st2[i] != '!' && st2[i] != '@')	//!�� @�� �ƴҽ�
			{
				*temp_dec2[k] += st2[i];	//CString �迭�� �� �߰�
			}
			else
			{
				if (i == 0)
					k = 0;
				else
					k++;	//k ����
			}
		}

		int temp_int[4096] = { 0, };

		for (int i = 0; i < count3; i++)
			temp_int[i] = _ttoi(*temp_dec2[i]);	//CString -> int ��ȯ

		k = 0;
		for (int m = 0; m < count2; m++)
			for (int i = 0; i < 4; i++)
				for (int j = 0; j < 4; j++)
					State[m][i][j] = temp_int[k++];	//���°��� ����

		for (int i = 0; i < count2; i++)
			m_aes->Decrypt(Decrypt_AES[i], State[i], CipherKey);	//State ���� CipherKey�� �̿��� AES ��ȣȭ��Ŵ

		CString dec_data = "";

		for (int i = 0; i < count2; i++)
		{
			for (int j = 0; j < 16; j++)
			{
				//Decrypt_AES[i][j] = Decrypt_AES[i][j] + 48;
				dec_data += Decrypt_AES[i][j];	//��ȣȭ�� ������ �߰�
			}
		}

		DB_LIST.InsertItem(list_count, dec_data);	//��ȣȭ ������ List Control�� �߰�

		DB_LIST.SetItem(list_count, 1, LVIF_TEXT, date, 0, 0, 0, NULL);		//date �� List Control�� �߰�

		list_count++;	//����Ʈ ī��Ʈ ����

		for (int i = 0; i < 4096; i++)
			delete temp_dec2[i];	//CString �޸� ����

		for (int i = 0; i < 16; i++)
			delete temp_dec[i];		//CString �޸� ����

		mysql_free_result(sql_result2);	//Mysql result ����

		memset(st, 0, sizeof(st));
		memset(st2, 0, sizeof(st2));

	}	

	mysql_free_result(sql_result);	//Mysql result ����

	temp_aes.Empty();
	temp_pq.Empty();
	temp_n.Empty();
	temp_e.Empty();

}

void CDB_Check::ThreadFunc2()
{
	DB_LIST.DeleteAllItems();

	int k = 0;

	dec[16] = { 0, };	//AES Ű ��ȣȭ��

	dec2[4096] = { 0, }; //AES ��ȣ ������ ��ȣȭ��

	int Decrypt_RSA[16] = { 0, };	//RSA ��ȣȭ�� ���� �迭
	char Decrypt_AES[1024][16] = { 0, };	//AES ��ȣȭ�� ���� 2���� �迭

	CString str = "";
	int n;
	n = DB_DATE.GetLBTextLen(DB_DATE.GetCurSel());	//��Ʈ�� ���̸� ����
	DB_DATE.GetLBText(DB_DATE.GetCurSel(), str.GetBuffer(n));	//�޺� �ڽ��� ���� ��Ʈ�� ������
	str.ReleaseBuffer();	//��������

	query_state = mysql_query(connection, "SELECT * FROM data WHERE date LIKE '" + str + "%'");	//������ �޺��ڽ� ������ �Է� �� ����

	if (query_state != 0)	//������ ���� �����޽��� ���
		AfxMessageBox("Query Select3 Error!");

	sql_result = mysql_store_result(connection);	//���� ������ ����

	int list_count = 0;	//����Ʈ ���� ī��Ʈ

	while ((sql_row = mysql_fetch_row(sql_result)) != NULL)	//���� ������ �� ������
	{
		for (int i = 0; i < 16; i++)
		{
			temp_dec[i] = new CString();
			*temp_dec[i] = "";	//CString �ʱ�ȭ
		}

		CString date = "";
		date = sql_row[1];	//������ date �� ����

		query_state2 = mysql_query(connection2, "SELECT * FROM encryptkey WHERE date LIKE '" + date + "%'");	//date �� �̿��� RSA ��ȣȭ�� Ű�� ���� ���� ����

		sql_result2 = mysql_store_result(connection2);	//���� ������ ����

		while ((sql_row2 = mysql_fetch_row(sql_result2)) != NULL)	//���� ������ �� ������
		{
			temp_aes = sql_row2[0];	//!���� ��ȣȭ�� AESŰ

			temp_pq = sql_row2[1];	//RSA pq��
			temp_n = sql_row2[2];	//RSA n��
			temp_e = sql_row2[3];	//RSA e��
		}


		//int������ ����ȯ//////////
		m_rsa->pq = _ttoi(temp_pq);
		m_rsa->n = _ttoi(temp_n);
		m_rsa->e = _ttoi(temp_e);
		//////////////////////////

		char* st = LPSTR(LPCTSTR(temp_aes));	//!�и��ϱ� ���� char�� �迭�� ����ȯ
		k = 0;
		for (int i = 0; i < temp_aes.GetLength(); i++)
		{
			if (st[i] != '!')	//�迭���� !�� ������
			{
				*temp_dec[k] += st[i];	//CString�迭�� �� �߰�
			}
			else	//!�� ������
			{
				if (i == 0)
					k = 0;
				else
					k++;	//k ����
			}
		}

		for (int i = 0; i < 16; i++)
			dec[i] = _ttoi(*temp_dec[i]);	//int�� �迭�� CString���� �и��� �����Ͱ� ����ȯ �� ����

		m_rsa->Decrypt(Decrypt_RSA, dec);	//RSA ��ȣȭ

		k = 0;

		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				CipherKey[i][j] = Decrypt_RSA[k++];	//��ȣȭ�� Ű���� CipherKey�� ����

		int count2 = 0;	//@���� (3���� �迭 ���鰳��)
		int count3 = 0; //@�� !���� (CString �迭 ���鰳��)

		CString enc_data = "";
		enc_data = sql_row[0];	//��ȣȭ�� �����Ͱ� ������

		char* st2 = LPSTR(LPCTSTR(enc_data));	//!�� @�и��ϱ����� char�� �迭�� ��ȯ

		for (int i = 0; i < enc_data.GetLength(); i++)
		{
			if (st2[i] == '@')
			{
				count2++;	//@���� ī��Ʈ
				count3++;	//@�� !���� ī��Ʈ
			}

			if (st2[i] == '!')
				count3++;	//@�� !���� ī��Ʈ
		}

		for (int i = 0; i < 4096; i++)
		{
			temp_dec2[i] = new CString();
			*temp_dec2[i] = "";	//CString �迭 �ʱ�ȭ
		}

		k = 0;

		for (int i = 0; i < enc_data.GetLength(); i++)
		{
			if (st2[i] != '!' && st2[i] != '@')	//!�� @�� �ƴҽ�
			{
				*temp_dec2[k] += st2[i];	//CString �迭�� �� �߰�
			}
			else
			{
				if (i == 0)
					k = 0;
				else
					k++;	//k ����
			}
		}

		int temp_int[4096] = { 0, };

		for (int i = 0; i < count3; i++)
			temp_int[i] = _ttoi(*temp_dec2[i]);	//CString -> int ��ȯ

		k = 0;
		for (int m = 0; m < count2; m++)
			for (int i = 0; i < 4; i++)
				for (int j = 0; j < 4; j++)
					State[m][i][j] = temp_int[k++];	//���°��� ����

		for (int i = 0; i < count2; i++)
			m_aes->Decrypt(Decrypt_AES[i], State[i], CipherKey);	//State ���� CipherKey�� �̿��� AES ��ȣȭ��Ŵ

		CString dec_data = "";

		for (int i = 0; i < count2; i++)
		{
			for (int j = 0; j < 16; j++)
			{
				//Decrypt_AES[i][j] = Decrypt_AES[i][j] + 48;
				dec_data += Decrypt_AES[i][j];	//��ȣȭ�� ������ �߰�
			}
		}

		DB_LIST.InsertItem(list_count, dec_data);	//��ȣȭ ������ List Control�� �߰�

		DB_LIST.SetItem(list_count, 1, LVIF_TEXT, date, 0, 0, 0, NULL);		//date �� List Control�� �߰�

		list_count++;	//����Ʈ ī��Ʈ ����

		for (int i = 0; i < 4096; i++)
			delete temp_dec2[i];	//CString �޸� ����

		for (int i = 0; i < 16; i++)
			delete temp_dec[i];		//CString �޸� ����

		mysql_free_result(sql_result2);		//Mysql result ���� 

		memset(st, 0, sizeof(st));
		memset(st2, 0, sizeof(st2));

	}

	mysql_free_result(sql_result);	//Mysql result ���� 

	temp_aes.Empty();
	temp_pq.Empty();
	temp_n.Empty();
	temp_e.Empty();
}

void CDB_Check::OnBnClickedDb()
{
	if (p2 != NULL)	//p2 ������ �����Ͱ� NULL�� �ƴҶ� (�����尡 �������϶�)
	{
		DestroyTherad2();	//p2 ������ �����Լ� ����
	}
	else if (p1 != NULL)	//p1 ������ �����Ͱ� NULL�� �ƴҶ� (�����尡 �������϶�)
	{
		DestroyTherad();	//p1 ������ �����Լ� ����
	}

	p1 = AfxBeginThread(ThreadFirst, this, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);	//AfxBeginThread �� ������ ����

	if (p1 == NULL)	//p1 ������ �����Ͱ� NULL �ΰ�� (������ ������ �ȵ��� ��)
	{
		AfxMessageBox("Error");	
	}

	p1->m_bAutoDelete = FALSE;	//������ �ڵ����� ����
	p1->ResumeThread();

}


void CDB_Check::OnCbnSelchangeDbdate()
{
	
	if (p2 != NULL)	//p2 ������ �����Ͱ� NULL�� �ƴҶ� (�����尡 �������϶�)
	{
		DestroyTherad2();	//p2 ������ �����Լ� ����
	}
	else if (p1 != NULL)	//p1 ������ �����Ͱ� NULL�� �ƴҶ� (�����尡 �������϶�)
	{
		DestroyTherad();	//p1 ������ �����Լ� ����
	}

	p2 = AfxBeginThread(ThreadSecond, this, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);	//AfxBeginThread �� ������ ����

	if (p2 == NULL)	//p2 ������ �����Ͱ� NULL �ΰ�� (������ ������ �ȵ��� ��)
	{
		AfxMessageBox("Error");
	}

	p2->m_bAutoDelete = FALSE;	//������ �ڵ����� ����
	p2->ResumeThread();
	

}

bool CDB_Check::DestroyTherad()
{
	if (p1 != NULL)
	{
		DWORD dwResult = ::WaitForSingleObject(p1->m_hThread, INFINITE);	//Ư�� ������Ʈ ���°� ������ �� ���� ���� �������� ������ ����

		if (dwResult == WAIT_TIMEOUT)	//������Ʈ ���°� �������� �������
			AfxMessageBox("Time OUT!");
		
		delete p1;

		p1 = NULL;
	}
	return true;

}

bool CDB_Check::DestroyTherad2()
{
	if (p2 != NULL)
	{
		DWORD dwResult = ::WaitForSingleObject(p2->m_hThread, INFINITE);	//Ư�� ������Ʈ ���°� ������ �� ���� ���� �������� ������ ����

		if (dwResult == WAIT_TIMEOUT)	//������Ʈ ���°� �������� �������
			AfxMessageBox("Time OUT!");
		
		delete p2;

		p2 = NULL;
	}
	return true;

}




