
// TestServerDlg.h : header file
//



#pragma once

#include "ServerSock.h"		//ServerSock ����߰�
#include "ClientSock.h"		//ClientSock ����߰�
#include "AES.h"	//AES ����߰�
#include "RSA_CODE.h"	//RSA ����߰�
#include "afxwin.h"


#define DB_HOST "localhost"
#define USER_NAME "root"
#define USER_PASS "apmsetup"
#define DB_NAME "issi"

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
		char *name;
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

	unsigned char CipherKey[0x04][0x04];	//��ȣŰ ����
	unsigned char State[0x04][0x04];		//���°� ����


//MYSQL ����ϱ� ���� ����/////
	MYSQL *connection, conn;
	MYSQL_RES *sql_result;
	MYSQL_ROW sql_row;

	int query_state;
	char query[2048];
//////////////////////////////

	int dec[16];	//DB ����� AESŰ ��ȣ�� �и���
	CString *temp_dec[16];	//DB �����  AESŰ ��ȣ�� �и���

	
};
