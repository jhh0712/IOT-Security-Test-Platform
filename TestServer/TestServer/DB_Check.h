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
	virtual BOOL OnInitDialog();	//초기화함수

	CListCtrl DB_LIST;	//List Control 변수 선언
	afx_msg void OnBnClickedDb();	//DB 확인 버튼

	int dec[16];	//DB 저장된 AES키 암호문 분리용 DB
	CString *temp_dec[16];	//DB 저장된  AES키 암호문 분리용 DB

	int dec2[4096];	//DB 저장된 AES 암호문 분리용 
	CString *temp_dec2[4096];	//DB 저장된  AES키 암호문 분리용 

	//Mysql 사용하기 위해 선언
	MYSQL *connection, conn;	
	MYSQL_RES *sql_result;
	MYSQL_RES *sql_result2;

	MYSQL_ROW sql_row;
	MYSQL_ROW sql_row2;
	
	int query_state;
	int query_state2;

	char query[2048];
	//////////////////////////


	afx_msg void OnCbnSelchangeDbdate();	//날짜선택 콤보박스 클릭시
	CComboBox DB_DATE;	//콤보박스
	
	int click_count;	//클릭 카운트

	//DB확인시 사용 (DB 내용 임시 저장)
	CString temp_aes;
	CString temp_pq;
	CString temp_n;
	CString temp_e;
	/////////////////////////////////

	unsigned char CipherKey[0x04][0x04];	//암호키 저장 DB
	unsigned char State[1024][0x04][0x04];		//상태값 저장 DB

	AES *m_aes;		//AES 헤더 사용하기 위해 선언
	RSA_CODE *m_rsa;	//RSA 헤더 사용하기 위해 선언

	static UINT ThreadFirst(LPVOID _mothod);	//DB 불러올때 스레드로 실행하기 위해 선언	(버튼)
	static UINT ThreadSecond(LPVOID _mothod);	//DB 불러올때 스레드로 실행하기 위해 선언	(콤보박스)

	void ThreadFunc();	//스레드에서 실행할 함수
	void ThreadFunc2();	//스레드에서 실행할 함수2

	CWinThread *p1;	//스레드 포인터 
	CWinThread *p2;	//스레드 포인터2

	bool DestroyTherad();	//스레드 종료함수
	bool DestroyTherad2();	//스레드 종료함수2

};
