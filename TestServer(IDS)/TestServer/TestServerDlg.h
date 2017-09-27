
// TestServerDlg.h : header file
//



#pragma once

#include "ServerSock.h"		//ServerSock ����߰�
#include "ClientSock.h"		//ClientSock ����߰�
#include "AES.h"	//AES ����߰�
#include "RSA_CODE.h"	//RSA ����߰�
#include "DB_Check.h"	//DB ���̾�α� ����߰�
#include "IDS.h"
#include "afxwin.h"
#include "afxcmn.h"


#define DB_HOST "localhost"	//������ ���� DB
#define USER_NAME "root"
#define USER_PASS "apmsetup"
#define DB_NAME "issi"

#define DB_HOST2 "192.168.0.76"	//Ű���� ���� DB
#define USER_NAME2 "root"
#define USER_PASS2 "issi"
#define DB_NAME2 "issi"

// CTestServerDlg dialog
class CTestServerDlg : public CDialogEx
{
// Construction
public:
	CTestServerDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TESTSERVER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()


//��������� ���� ����///////////////
public:
	void OnReceive(ClientSock* pSock);
	void OnAccept();
	void OnSend(char *buff, int len);

	ServerSock *m_pServerSock;
	~CTestServerDlg();

	struct client_struct {
		ClientSock *p;
		char *state;
	};
	struct client_struct cs[4096];

	int user;
//////////////////////////////////

	afx_msg void OnBnClickedSocketCreate();	//���ϻ��� ��ư
	afx_msg void OnBnClickedSocketClose();	//�������� ��ư

	CListBox List_ServerLine;	//���� ����ǥ����

	int line;
	
	AES *m_aes;		//AES ��� ����ϱ� ���� ����
	RSA_CODE *m_rsa;	//RSA ��� ����ϱ� ���� ����
	
	CDB_Check *m_dbcheck;	//DB ���̾�α� ����ϱ� ���� ����
	CIDS *m_ids;

	unsigned char CipherKey[0x04][0x04];	//��ȣŰ ����
	unsigned char State[1024][0x04][0x04];		//���°� ����
	unsigned char init_CipherKey[0x04][0x04];	//��ȣŰ �ʱⰪ ����

	

	//������Ž� ��� (DB �ӽ� �����)
	CString temp_aes;
	CString temp_pq;
	CString temp_n;
	CString temp_e;
	CString temp_date;
	///////////////

	

//MYSQL(�����켭��) ����ϱ� ���� ����/////
	MYSQL *connection, conn;
	MYSQL_RES *sql_result;
	
	MYSQL_ROW sql_row;
	
	int query_state;
	
	char query[2048];
//////////////////////////////

//MYSQL(Ű��������) ����ϱ� ���� ����/////
	MYSQL *connection2, conn2;
	MYSQL_RES *sql_result2;

	MYSQL_ROW sql_row2;

	int query_state2;

	char query2[2048];
//////////////////////////////


	int dec[16];	//DB ����� AESŰ ��ȣ�� �и��� ����
	CString *temp_dec[16];	//DB �����  AESŰ ��ȣ�� �и��� ����


	afx_msg void OnBnClickedDb();	//DB Ȯ�ι�ư

	afx_msg void OnBnClickedIds();
	
	//CListCtrl CLIENT_LIST;
	/*int list_count;
	
	static UINT ThreadFirst(LPVOID _mothod);
	
	void ThreadFunc();

	CWinThread *p1;

	bool DestroyTherad();
	*/
};
