
// TestServerDlg.h : header file
//



#pragma once

#include "ServerSock.h"		//ServerSock 헤더추가
#include "ClientSock.h"		//ClientSock 헤더추가
#include "AES.h"	//AES 헤더추가
#include "RSA_CODE.h"	//RSA 헤더추가
#include "DB_Check.h"	//DB 다이얼로그 헤더추가
#include "IDS.h"
#include "afxwin.h"
#include "afxcmn.h"


#define DB_HOST "localhost"	//윈도우 서버 DB
#define USER_NAME "root"
#define USER_PASS "apmsetup"
#define DB_NAME "issi"

#define DB_HOST2 "192.168.0.76"	//키관리 서버 DB
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


//소켓통신을 위해 선언///////////////
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

	afx_msg void OnBnClickedSocketCreate();	//소켓생성 버튼
	afx_msg void OnBnClickedSocketClose();	//소켓종료 버튼

	CListBox List_ServerLine;	//서버 상태표시줄

	int line;
	
	AES *m_aes;		//AES 헤더 사용하기 위해 선언
	RSA_CODE *m_rsa;	//RSA 헤더 사용하기 위해 선언
	
	CDB_Check *m_dbcheck;	//DB 다이얼로그 사용하기 위해 선언
	CIDS *m_ids;

	unsigned char CipherKey[0x04][0x04];	//암호키 저장
	unsigned char State[1024][0x04][0x04];		//상태값 저장
	unsigned char init_CipherKey[0x04][0x04];	//암호키 초기값 저장

	

	//소켓통신시 사용 (DB 임시 저장용)
	CString temp_aes;
	CString temp_pq;
	CString temp_n;
	CString temp_e;
	CString temp_date;
	///////////////

	

//MYSQL(윈도우서버) 사용하기 위해 선언/////
	MYSQL *connection, conn;
	MYSQL_RES *sql_result;
	
	MYSQL_ROW sql_row;
	
	int query_state;
	
	char query[2048];
//////////////////////////////

//MYSQL(키관리서버) 사용하기 위해 선언/////
	MYSQL *connection2, conn2;
	MYSQL_RES *sql_result2;

	MYSQL_ROW sql_row2;

	int query_state2;

	char query2[2048];
//////////////////////////////


	int dec[16];	//DB 저장된 AES키 암호문 분리용 소켓
	CString *temp_dec[16];	//DB 저장된  AES키 암호문 분리용 소켓


	afx_msg void OnBnClickedDb();	//DB 확인버튼

	afx_msg void OnBnClickedIds();
	
	//CListCtrl CLIENT_LIST;
	/*int list_count;
	
	static UINT ThreadFirst(LPVOID _mothod);
	
	void ThreadFunc();

	CWinThread *p1;

	bool DestroyTherad();
	*/
};
