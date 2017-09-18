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
		temp_dec[i] = NULL;	//CString 배열 초기화

	for (int i = 0; i < 4096; i++)
		temp_dec2[i] = NULL;	//CString 배열 초기화
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

	click_count = 0;	//클릭 카운트 초기화


	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			CipherKey[i][j] = 0;	//CipherKey값 초기화


	for (int m = 0; m < 1024; m++)
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				State[m][i][j] = 0;		//State값 초기화


	m_aes = new AES();//(AES*)AfxGetApp();
	m_rsa = new RSA_CODE();

	connection = NULL;
	mysql_init(&conn);	//Mysql 초기화

	connection = mysql_real_connect(&conn, DB_HOST, USER_NAME, USER_PASS, DB_NAME, 3306, (char *)NULL, 0);	//Mysql DB서버에 연결시도

	if (connection == NULL)	//연결실패 시 에러메시지 출력
	{
		AfxMessageBox(_T("DB CONNECTION ERROR"));
	}

	connection2 = NULL;
	mysql_init(&conn2);	//Mysql 초기화

	connection2 = mysql_real_connect(&conn2, DB_HOST2, USER_NAME2, USER_PASS2, DB_NAME2, 3306, (char *)NULL, 0);	//Mysql DB서버에 연결시도

	if (connection2 == NULL)	//연결실패 시 에러메시지 출력
	{
		AfxMessageBox(_T("KEY MANAGEMENT DB CONNECTION ERROR"));
	}


	DB_LIST.InsertColumn(1, _T("DATA"), LVCFMT_LEFT, 650);		//List Control 초기화
	DB_LIST.InsertColumn(2, _T("DATE"), LVCFMT_LEFT, 120);	//List Control 초기화

	p1 = NULL;
	p2 = NULL;	//스레드 포인터 초기화

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


// CDB_Check message handlers

UINT CDB_Check::ThreadFirst(LPVOID _mothod)
{
	CDB_Check *fir = (CDB_Check*)_mothod;

	fir->ThreadFunc();	//스레드에서 실행할 함수
	
	return 0;

}

UINT CDB_Check::ThreadSecond(LPVOID _mothod)
{
	CDB_Check *fir = (CDB_Check*)_mothod;

	fir->ThreadFunc2();	//스레드에서 실행할 함수

	return 0;

}

void CDB_Check::ThreadFunc()
{
	DB_LIST.DeleteAllItems();

	int k = 0;

	dec[16] = { 0, };	//AES 키 복호화용

	dec2[4096] = { 0, }; //AES 암호 데이터 복호화용

	int Decrypt_RSA[16] = { 0, };	//RSA 복호화값 받을 배열
	char Decrypt_AES[1024][16] = { 0, };	//AES 복호화값 받을 2차원 배열

	query_state = mysql_query(connection, SQL_SELECT2);	//쿼리 실행

	if (query_state != 0)	//쿼리문 실행 에러메시지 출력
		AfxMessageBox("Query Select2 Error!");

	sql_result = mysql_store_result(connection);	//쿼리 실행결과 저장

	if (click_count == 0)	//첫 버튼 클릭시
	{
		while ((sql_row = mysql_fetch_row(sql_result)) != NULL)	//쿼리 실행결과 값 가져옴
		{
			DB_DATE.AddString(sql_row[0]);	//콤보박스에 내용추가
		}
		DB_DATE.SetCurSel(0);
		click_count++;	//카운트 증가
	}

	CString str = "";
	int n;
	n = DB_DATE.GetLBTextLen(DB_DATE.GetCurSel());	//스트링 길이를 지정
	DB_DATE.GetLBText(DB_DATE.GetCurSel(), str.GetBuffer(n));	//콤보 박스로 부터 스트링 가져옴
	str.ReleaseBuffer();	//버퍼제거

	query_state = mysql_query(connection, "SELECT * FROM data WHERE date LIKE '" + str + "%'");	//쿼리에 콤보박스 데이터 입력 후 실행

	if (query_state != 0)	//쿼리문 실행 에러메시지 출력
		AfxMessageBox("Query Select3 Error!");

	sql_result = mysql_store_result(connection);	//쿼리 실행결과 저장

	int list_count = 0;	//리스트 숫자 카운트

	while ((sql_row = mysql_fetch_row(sql_result)) != NULL)	//쿼리 실행결과 값 가져옴
	{
		for (int i = 0; i < 16; i++)
		{
			temp_dec[i] = new CString();
			*temp_dec[i] = "";	//CString 초기화
		}

		CString date = "";
		date = sql_row[1];	//쿼리에 date 값 저장

		query_state2 = mysql_query(connection2, "SELECT * FROM encryptkey WHERE date LIKE '" + date + "%'");	//date 값 이용해 RSA 암호화된 키값 선택 쿼리 실행

		sql_result2 = mysql_store_result(connection2);	//쿼리 실행결과 저장

		while ((sql_row2 = mysql_fetch_row(sql_result2)) != NULL)	//쿼리 실행결과 값 가져옴
		{
			temp_aes = sql_row2[0];	//!포함 암호화된 AES키

			temp_pq = sql_row2[1];	//RSA pq값
			temp_n = sql_row2[2];	//RSA n값
			temp_e = sql_row2[3];	//RSA e값
		}


		//int형으로 형변환//////////
		m_rsa->pq = _ttoi(temp_pq);
		m_rsa->n = _ttoi(temp_n);
		m_rsa->e = _ttoi(temp_e);
		//////////////////////////

		char* st = LPSTR(LPCTSTR(temp_aes));	//!분리하기 위해 char형 배열로 형변환
		k = 0;
		for (int i = 0; i < temp_aes.GetLength(); i++)
		{
			if (st[i] != '!')	//배열값중 !가 없으면
			{
				*temp_dec[k] += st[i];	//CString배열에 값 추가
			}
			else	//!가 있으면
			{
				if (i == 0)
					k = 0;
				else
					k++;	//k 증가
			}
		}

		for (int i = 0; i < 16; i++)
			dec[i] = _ttoi(*temp_dec[i]);	//int형 배열에 CString으로 분리한 데이터값 형변환 후 대입

		m_rsa->Decrypt(Decrypt_RSA, dec);	//RSA 복호화

		k = 0;

		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				CipherKey[i][j] = Decrypt_RSA[k++];	//복호화된 키값을 CipherKey에 대입

		int count2 = 0;	//@개수 (3차원 배열 만들개수)
		int count3 = 0; //@및 !개수 (CString 배열 만들개수)

		CString enc_data = "";
		enc_data = sql_row[0];	//암호화된 데이터값 가져옴

		char* st2 = LPSTR(LPCTSTR(enc_data));	//!및 @분리하기위해 char형 배열로 변환

		for (int i = 0; i < enc_data.GetLength(); i++)
		{
			if (st2[i] == '@')
			{
				count2++;	//@개수 카운트
				count3++;	//@및 !개수 카운트
			}

			if (st2[i] == '!')
				count3++;	//@및 !개수 카운트
		}

		for (int i = 0; i < 4096; i++)
		{
			temp_dec2[i] = new CString();
			*temp_dec2[i] = "";	//CString 배열 초기화
		}

		k = 0;

		for (int i = 0; i < enc_data.GetLength(); i++)
		{
			if (st2[i] != '!' && st2[i] != '@')	//!나 @가 아닐시
			{
				*temp_dec2[k] += st2[i];	//CString 배열에 값 추가
			}
			else
			{
				if (i == 0)
					k = 0;
				else
					k++;	//k 증가
			}
		}

		int temp_int[4096] = { 0, };

		for (int i = 0; i < count3; i++)
			temp_int[i] = _ttoi(*temp_dec2[i]);	//CString -> int 변환

		k = 0;
		for (int m = 0; m < count2; m++)
			for (int i = 0; i < 4; i++)
				for (int j = 0; j < 4; j++)
					State[m][i][j] = temp_int[k++];	//상태값에 대입

		for (int i = 0; i < count2; i++)
			m_aes->Decrypt(Decrypt_AES[i], State[i], CipherKey);	//State 값을 CipherKey를 이용해 AES 복호화시킴

		CString dec_data = "";

		for (int i = 0; i < count2; i++)
		{
			for (int j = 0; j < 16; j++)
			{
				//Decrypt_AES[i][j] = Decrypt_AES[i][j] + 48;
				dec_data += Decrypt_AES[i][j];	//복호화된 데이터 추가
			}
		}

		DB_LIST.InsertItem(list_count, dec_data);	//복호화 데이터 List Control에 추가

		DB_LIST.SetItem(list_count, 1, LVIF_TEXT, date, 0, 0, 0, NULL);		//date 값 List Control에 추가

		list_count++;	//리스트 카운트 증가

		for (int i = 0; i < 4096; i++)
			delete temp_dec2[i];	//CString 메모리 해제

		for (int i = 0; i < 16; i++)
			delete temp_dec[i];		//CString 메모리 해제

		mysql_free_result(sql_result2);	//Mysql result 해제

		memset(st, 0, sizeof(st));
		memset(st2, 0, sizeof(st2));

	}	

	mysql_free_result(sql_result);	//Mysql result 해제

	temp_aes.Empty();
	temp_pq.Empty();
	temp_n.Empty();
	temp_e.Empty();

}

void CDB_Check::ThreadFunc2()
{
	DB_LIST.DeleteAllItems();

	int k = 0;

	dec[16] = { 0, };	//AES 키 복호화용

	dec2[4096] = { 0, }; //AES 암호 데이터 복호화용

	int Decrypt_RSA[16] = { 0, };	//RSA 복호화값 받을 배열
	char Decrypt_AES[1024][16] = { 0, };	//AES 복호화값 받을 2차원 배열

	CString str = "";
	int n;
	n = DB_DATE.GetLBTextLen(DB_DATE.GetCurSel());	//스트링 길이를 지정
	DB_DATE.GetLBText(DB_DATE.GetCurSel(), str.GetBuffer(n));	//콤보 박스로 부터 스트링 가져옴
	str.ReleaseBuffer();	//버퍼제거

	query_state = mysql_query(connection, "SELECT * FROM data WHERE date LIKE '" + str + "%'");	//쿼리에 콤보박스 데이터 입력 후 실행

	if (query_state != 0)	//쿼리문 실행 에러메시지 출력
		AfxMessageBox("Query Select3 Error!");

	sql_result = mysql_store_result(connection);	//쿼리 실행결과 저장

	int list_count = 0;	//리스트 숫자 카운트

	while ((sql_row = mysql_fetch_row(sql_result)) != NULL)	//쿼리 실행결과 값 가져옴
	{
		for (int i = 0; i < 16; i++)
		{
			temp_dec[i] = new CString();
			*temp_dec[i] = "";	//CString 초기화
		}

		CString date = "";
		date = sql_row[1];	//쿼리에 date 값 저장

		query_state2 = mysql_query(connection2, "SELECT * FROM encryptkey WHERE date LIKE '" + date + "%'");	//date 값 이용해 RSA 암호화된 키값 선택 쿼리 실행

		sql_result2 = mysql_store_result(connection2);	//쿼리 실행결과 저장

		while ((sql_row2 = mysql_fetch_row(sql_result2)) != NULL)	//쿼리 실행결과 값 가져옴
		{
			temp_aes = sql_row2[0];	//!포함 암호화된 AES키

			temp_pq = sql_row2[1];	//RSA pq값
			temp_n = sql_row2[2];	//RSA n값
			temp_e = sql_row2[3];	//RSA e값
		}


		//int형으로 형변환//////////
		m_rsa->pq = _ttoi(temp_pq);
		m_rsa->n = _ttoi(temp_n);
		m_rsa->e = _ttoi(temp_e);
		//////////////////////////

		char* st = LPSTR(LPCTSTR(temp_aes));	//!분리하기 위해 char형 배열로 형변환
		k = 0;
		for (int i = 0; i < temp_aes.GetLength(); i++)
		{
			if (st[i] != '!')	//배열값중 !가 없으면
			{
				*temp_dec[k] += st[i];	//CString배열에 값 추가
			}
			else	//!가 있으면
			{
				if (i == 0)
					k = 0;
				else
					k++;	//k 증가
			}
		}

		for (int i = 0; i < 16; i++)
			dec[i] = _ttoi(*temp_dec[i]);	//int형 배열에 CString으로 분리한 데이터값 형변환 후 대입

		m_rsa->Decrypt(Decrypt_RSA, dec);	//RSA 복호화

		k = 0;

		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				CipherKey[i][j] = Decrypt_RSA[k++];	//복호화된 키값을 CipherKey에 대입

		int count2 = 0;	//@개수 (3차원 배열 만들개수)
		int count3 = 0; //@및 !개수 (CString 배열 만들개수)

		CString enc_data = "";
		enc_data = sql_row[0];	//암호화된 데이터값 가져옴

		char* st2 = LPSTR(LPCTSTR(enc_data));	//!및 @분리하기위해 char형 배열로 변환

		for (int i = 0; i < enc_data.GetLength(); i++)
		{
			if (st2[i] == '@')
			{
				count2++;	//@개수 카운트
				count3++;	//@및 !개수 카운트
			}

			if (st2[i] == '!')
				count3++;	//@및 !개수 카운트
		}

		for (int i = 0; i < 4096; i++)
		{
			temp_dec2[i] = new CString();
			*temp_dec2[i] = "";	//CString 배열 초기화
		}

		k = 0;

		for (int i = 0; i < enc_data.GetLength(); i++)
		{
			if (st2[i] != '!' && st2[i] != '@')	//!나 @가 아닐시
			{
				*temp_dec2[k] += st2[i];	//CString 배열에 값 추가
			}
			else
			{
				if (i == 0)
					k = 0;
				else
					k++;	//k 증가
			}
		}

		int temp_int[4096] = { 0, };

		for (int i = 0; i < count3; i++)
			temp_int[i] = _ttoi(*temp_dec2[i]);	//CString -> int 변환

		k = 0;
		for (int m = 0; m < count2; m++)
			for (int i = 0; i < 4; i++)
				for (int j = 0; j < 4; j++)
					State[m][i][j] = temp_int[k++];	//상태값에 대입

		for (int i = 0; i < count2; i++)
			m_aes->Decrypt(Decrypt_AES[i], State[i], CipherKey);	//State 값을 CipherKey를 이용해 AES 복호화시킴

		CString dec_data = "";

		for (int i = 0; i < count2; i++)
		{
			for (int j = 0; j < 16; j++)
			{
				//Decrypt_AES[i][j] = Decrypt_AES[i][j] + 48;
				dec_data += Decrypt_AES[i][j];	//복호화된 데이터 추가
			}
		}

		DB_LIST.InsertItem(list_count, dec_data);	//복호화 데이터 List Control에 추가

		DB_LIST.SetItem(list_count, 1, LVIF_TEXT, date, 0, 0, 0, NULL);		//date 값 List Control에 추가

		list_count++;	//리스트 카운트 증가

		for (int i = 0; i < 4096; i++)
			delete temp_dec2[i];	//CString 메모리 해제

		for (int i = 0; i < 16; i++)
			delete temp_dec[i];		//CString 메모리 해제

		mysql_free_result(sql_result2);		//Mysql result 해제 

		memset(st, 0, sizeof(st));
		memset(st2, 0, sizeof(st2));

	}

	mysql_free_result(sql_result);	//Mysql result 해제 

	temp_aes.Empty();
	temp_pq.Empty();
	temp_n.Empty();
	temp_e.Empty();
}

void CDB_Check::OnBnClickedDb()
{
	if (p2 != NULL)	//p2 스레드 포인터가 NULL이 아닐때 (스레드가 실행중일때)
	{
		DestroyTherad2();	//p2 스레드 종료함수 실행
	}
	else if (p1 != NULL)	//p1 스레드 포인터가 NULL이 아닐때 (스레드가 실행중일때)
	{
		DestroyTherad();	//p1 스레드 종료함수 실행
	}

	p1 = AfxBeginThread(ThreadFirst, this, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);	//AfxBeginThread 로 스레드 실행

	if (p1 == NULL)	//p1 스레드 포인터가 NULL 인경우 (스레드 실행이 안됐을 떄)
	{
		AfxMessageBox("Error");	
	}

	p1->m_bAutoDelete = FALSE;	//스레드 자동종료 해제
	p1->ResumeThread();

}


void CDB_Check::OnCbnSelchangeDbdate()
{
	
	if (p2 != NULL)	//p2 스레드 포인터가 NULL이 아닐때 (스레드가 실행중일때)
	{
		DestroyTherad2();	//p2 스레드 종료함수 실행
	}
	else if (p1 != NULL)	//p1 스레드 포인터가 NULL이 아닐때 (스레드가 실행중일때)
	{
		DestroyTherad();	//p1 스레드 종료함수 실행
	}

	p2 = AfxBeginThread(ThreadSecond, this, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);	//AfxBeginThread 로 스레드 실행

	if (p2 == NULL)	//p2 스레드 포인터가 NULL 인경우 (스레드 실행이 안됐을 떄)
	{
		AfxMessageBox("Error");
	}

	p2->m_bAutoDelete = FALSE;	//스레드 자동종료 해제
	p2->ResumeThread();
	

}

bool CDB_Check::DestroyTherad()
{
	if (p1 != NULL)
	{
		DWORD dwResult = ::WaitForSingleObject(p1->m_hThread, INFINITE);	//특정 오브젝트 상태가 설정될 때 까지 현재 스레드의 실행을 멈춤

		if (dwResult == WAIT_TIMEOUT)	//오브젝트 상태가 설정되지 않은경우
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
		DWORD dwResult = ::WaitForSingleObject(p2->m_hThread, INFINITE);	//특정 오브젝트 상태가 설정될 때 까지 현재 스레드의 실행을 멈춤

		if (dwResult == WAIT_TIMEOUT)	//오브젝트 상태가 설정되지 않은경우
			AfxMessageBox("Time OUT!");
		
		delete p2;

		p2 = NULL;
	}
	return true;

}




