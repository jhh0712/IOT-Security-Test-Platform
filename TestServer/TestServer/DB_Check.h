#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "AES.h"
#include "RSA_CODE.h"

#define DB_HOST "localhost"
#define USER_NAME "root"
#define USER_PASS "apmsetup"
#define DB_NAME "issi"

// CDB_Check dialog

class CDB_Check : public CDialogEx
{
	DECLARE_DYNAMIC(CDB_Check)

public:
	CDB_Check(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDB_Check();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DBCHECK };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();	//�ʱ�ȭ�Լ�

	CListCtrl DB_LIST;	//List Control ���� ����
	afx_msg void OnBnClickedDb();	//DB Ȯ�� ��ư

	int dec[16];	//DB ����� AESŰ ��ȣ�� �и��� DB
	CString *temp_dec[16];	//DB �����  AESŰ ��ȣ�� �и��� DB

	int dec2[4096];	//DB ����� AES ��ȣ�� �и��� 
	CString *temp_dec2[4096];	//DB �����  AESŰ ��ȣ�� �и��� 

	//Mysql ����ϱ� ���� ����
	MYSQL *connection, conn;	
	MYSQL_RES *sql_result;
	MYSQL_RES *sql_result2;

	MYSQL_ROW sql_row;
	MYSQL_ROW sql_row2;
	
	int query_state;
	int query_state2;

	char query[2048];
	//////////////////////////


	afx_msg void OnCbnSelchangeDbdate();	//��¥���� �޺��ڽ� Ŭ����
	CComboBox DB_DATE;	//�޺��ڽ�
	
	int click_count;	//Ŭ�� ī��Ʈ

	//DBȮ�ν� ��� (DB ���� �ӽ� ����)
	CString temp_aes;
	CString temp_pq;
	CString temp_n;
	CString temp_e;
	/////////////////////////////////

	unsigned char CipherKey[0x04][0x04];	//��ȣŰ ���� DB
	unsigned char State[1024][0x04][0x04];		//���°� ���� DB

	AES *m_aes;		//AES ��� ����ϱ� ���� ����
	RSA_CODE *m_rsa;	//RSA ��� ����ϱ� ���� ����

	static UINT ThreadFirst(LPVOID _mothod);	//DB �ҷ��ö� ������� �����ϱ� ���� ����	(��ư)
	static UINT ThreadSecond(LPVOID _mothod);	//DB �ҷ��ö� ������� �����ϱ� ���� ����	(�޺��ڽ�)

	void ThreadFunc();	//�����忡�� ������ �Լ�
	void ThreadFunc2();	//�����忡�� ������ �Լ�2

	CWinThread *p1;	//������ ������ 
	CWinThread *p2;	//������ ������2

	bool DestroyTherad();	//������ �����Լ�
	bool DestroyTherad2();	//������ �����Լ�2

};
