
// TestServerDlg.cpp : implementation file
//
#include "stdafx.h"
#include "TestServer.h"
#include "TestServerDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



#define SQL_SELECT "select * from encryptkey order by date desc limit 1"

#define SQL_INSERT "insert into data values('%s', '%s')"

#pragma warning(disable:4996)

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CTestServerDlg dialog

CTestServerDlg::~CTestServerDlg()
{
	while (user)
	{
		user--;
		delete cs[user].p;
	}

	delete m_pServerSock;	//다이얼로그 종료시 생성소켓 지움
	

	mysql_close(connection);	//Mysql 연결 끊음
}


CTestServerDlg::CTestServerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_TESTSERVER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	for (int i = 0; i < 16; i++)
		temp_dec[i] = NULL;	//CString 배열 초기화
}

void CTestServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SERVER_LINE, List_ServerLine);



	DDX_Control(pDX, IDC_CLIENT, CLIENT_LIST);
}

BEGIN_MESSAGE_MAP(CTestServerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_SOCKET_CREATE, &CTestServerDlg::OnBnClickedSocketCreate)
	ON_BN_CLICKED(IDC_SOCKET_CLOSE, &CTestServerDlg::OnBnClickedSocketClose)
	ON_BN_CLICKED(IDC_DB, &CTestServerDlg::OnBnClickedDb)

	ON_BN_CLICKED(IDC_IDS, &CTestServerDlg::OnBnClickedIds)
END_MESSAGE_MAP()


// CTestServerDlg message handlers




BOOL CTestServerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	user = 0; //접속 사용자 수 초기화
	line = 0; //서버 상태표시줄 수 초기화


	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			CipherKey[i][j] = 0;	//CipherKey값 초기화
		

	for (int m = 0; m < 1024; m++)
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				State[m][i][j] = 0;		//State값 초기화
		

	m_aes = (AES*)AfxGetApp();
	//m_rsa = (RSA_CODE*)AfxGetApp();
	m_rsa = new RSA_CODE();

	connection = NULL;
	mysql_init(&conn);	//Mysql 초기화

	connection = mysql_real_connect(&conn, DB_HOST, USER_NAME, USER_PASS, DB_NAME, 3306, (char *)NULL, 0);	//Mysql DB서버에 연결시도

	if (connection == NULL)	//연결실패 시 에러메시지 출력
	{
		AfxMessageBox(_T("WINDOW DB CONNECTION ERROR"));
	}
	 
	connection2 = NULL;
	mysql_init(&conn2);	//Mysql 초기화

	connection2 = mysql_real_connect(&conn2, DB_HOST2, USER_NAME2, USER_PASS2, DB_NAME2, 3306, (char *)NULL, 0);	//Mysql DB서버에 연결시도

	if (connection2 == NULL)	//연결실패 시 에러메시지 출력
	{
		AfxMessageBox(_T("KEY MANAGEMENT DB CONNECTION ERROR"));
	}

	CLIENT_LIST.InsertColumn(1, _T("NUM"), LVCFMT_LEFT, 50);		//List Control 초기화
	CLIENT_LIST.InsertColumn(2, _T("ADDRESS"), LVCFMT_LEFT, 150);	//List Control 초기화
	CLIENT_LIST.InsertColumn(3, _T("DATE"), LVCFMT_LEFT, 160);	//List Control 초기화

//	list_count = 0;	//List Control 표시줄 카운트 초기화

	//p1 = NULL;

	if (m_pServerSock == NULL && cs->p == NULL)
		GetDlgItem(IDC_SOCKET_CLOSE)->EnableWindow(FALSE);	//소켓해제 버튼 비활성화


	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CTestServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTestServerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CTestServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
/*
UINT CTestServerDlg::ThreadFirst(LPVOID _mothod)
{
	CTestServerDlg *fir = (CTestServerDlg*)_mothod;

	fir->ThreadFunc();	//스레드에서 실행할 함수

	return 0;

}

void CTestServerDlg::ThreadFunc()
{
	int send_state = 0;
	int flag = 0;
	char buff[1] = { 0, };

	while (1)
	{
		for (int i = 0; i < user; i++)
		{
			send_state = cs[i].p->Send(buff, 1);

			if (send_state == -1)
			{
				send_state = cs[i].p->GetLastError();

				delete cs[i].p;
				
				CString str("클라이언트 소켓해제\r\n");

				List_ServerLine.InsertString(line++, str);	//소켓해제 메시지 출력
				List_ServerLine.SetCurSel(line - 1);	//리스트박스 선택값 마지막 라인으로 이동

				//CLIENT_LIST.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
				CLIENT_LIST.DeleteItem(i);
				flag = 1;
				break;
			}
		}
		if (flag == 1)
			break;

		Sleep(1);
	}

}

bool CTestServerDlg::DestroyTherad()
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
*/
//비동기 Accept 함수
void CTestServerDlg::OnAccept()
{

	cs[user].p = new ClientSock();	//ClientSock 생성

	CString str("클라이언트소켓 생성\r\n");

	List_ServerLine.InsertString(line++, str);	//클라이언트 소켓 생성 메시지 출력

	m_pServerSock->Accept(*cs[user].p);	//생성된 서버소켓으로 클라이언트 accept

	CString clientName("");
	UINT clientPort = 0;

	cs[user].p->GetPeerName(clientName, clientPort);	//클라이언트에서 접속한 IP와 포트를 가져옴
	//cs[user].state = "open";
/*
	CString num = "";
	num.Format("%d", user);

	CTime cTime = CTime::GetCurrentTime(); // 현재 시스템으로부터 날짜 및 시간을 얻어 온다.

	CString strDate; // 반환되는 날짜와 시간을 저장할 CString 변수 선언
	strDate.Format("%02d-%02d-%02d %02d:%02d:%02d", cTime.GetYear(), // 현재 년도 반환
		cTime.GetMonth(),	// 현재 월 반환
		cTime.GetDay(),		// 현재 일 반환
		cTime.GetHour(),	// 현재 시간 반환
		cTime.GetMinute(),  // 현재 분 반환
		cTime.GetSecond()); // 현재 초 반환


	CLIENT_LIST.InsertItem(list_count, num);	//접속 user 순서 List Control에 추가
	CLIENT_LIST.SetItem(list_count, 1, LVIF_TEXT, clientName, 0, 0, 0, NULL);		//clientName 값 List Control에 추가
	CLIENT_LIST.SetItem(list_count, 2, LVIF_TEXT, strDate, 0, 0, 0, NULL);
*/
	user++;
	//list_count++;

	str = clientName;
	str += "에서 접속\r\n";

	List_ServerLine.InsertString(line++, str);	//어디에서 접속했는지 메시지 출력
	List_ServerLine.SetCurSel(line-1);	//리스트박스 선택값 마지막 라인으로 이동
	
	if (m_pServerSock != NULL && cs->p != NULL)
		GetDlgItem(IDC_SOCKET_CLOSE)->EnableWindow(TRUE);	//소켓 해제버튼 활성화

	/*
	if (p1 != NULL)	//p1 스레드 포인터가 NULL이 아닐때 (스레드가 실행중일때)
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

	*/
}


//비동기 Receive 함수
void CTestServerDlg::OnReceive(ClientSock* pSock)
{
	int e_count = 1;	//Edit Control 데이터 줄바꿈 위한 카운트
	int k = 0;
	int send_state = 0;	//send 상태값


	DWORD dwReadLen;	//수신 데이터 길이
	pSock->IOCtl(FIONREAD, &dwReadLen); //수신 데이터 길이를 읽어옴
	
	char *buff = new char[dwReadLen];	//수신 데이터 저장할 버퍼 동적할당
	
	pSock->Receive(buff, dwReadLen);	//수신 데이터만큼 리시브
	
	
		CString temp1 = "";
		CString temp2 = "";

		CString textHex = "";
		CString textAscii = "";


		for (int i = 0; i < dwReadLen; i++)
		{

			int value = buff[i];

			if (value < 0)
				temp1.Format("%02X\t", value & (0xff));
			else
				temp1.Format("%02X\t", value);	//각 바이트를 HEX 코드로 바꿔줌

			textHex += temp1;

			if (e_count % 8 == 0)
				textHex += "\r\n";	//8바이트마다 줄바꿈

			e_count++;
		}

		e_count = 1;

		for (int i = 0; i < dwReadLen; i++)
		{
			int value = buff[i];

			if (value < 0)
				temp2.Format("%d\t", value & (0xff));
			else
				temp2.Format("%d\t", value);	//각 바이트를 ASCII 코드로 바꿔줌
			textAscii += temp2;

			if (e_count % 8 == 0)
				textAscii += "\r\n";	//8바이트마다 줄바꿈

			e_count++;
		}

		SetDlgItemText(IDC_DATA_HEX, textHex);	//Edit Control에 출력
		SetDlgItemText(IDC_DATA_ASCII, textAscii);	//Edit Control에 출력

		srand((unsigned int)(time(NULL))); //난수 생성

		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				CipherKey[i][j] = rand() % 255;	//CipherKey 생성

		int temp_key[16] = { 0, };
		int enc_key[16] = { 0, };
		char temp[10] = { 0, };		//RSA 암호화 된 데이터 형변환 하기 위한 임시 배열
		char aes_key[100] = { 0, };	//쿼리문에 넣기 위한 배열

		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				temp_key[k++] = CipherKey[i][j];

		m_rsa->Encrypt(enc_key, temp_key);

		for (int i = 0; i < 16; i++)
		{
			sprintf(temp, "%d", enc_key[i]);	//RSA 암호화 된 int형 데이터 Char형으로 형변환
			strcat(aes_key, temp);	//aes_key에 내용 추가
			strcat(aes_key, "!");	//바이트 구분을 위해 ! 추가
		}

		sprintf(query2, "insert into encryptkey values('%s', '%d', '%d', '%d', now());", aes_key, m_rsa->pq, m_rsa->n, m_rsa->e);	//쿼리문 저장

		query_state2 = mysql_query(connection2, query2);	//쿼리문 실행
		if (query_state2 != 0) {	//쿼리문 에러시
			AfxMessageBox("MYSQL QUERY ERROR!");
		}

		int count = 0;
		int count2 = 0;

		int flag = 0;

		count = dwReadLen / 16; 	//3차원 배열 크기 지정 위해 사용
		count2 = dwReadLen % 16;	//3차원 배열 크기 지정 위해 사용

		if (count2 != 0)	//나머지가 있으면 배열크기 증가시켜야함
			count++;

		for (int m = 0; m < count; m++)
		{
			for (int i = 0; i < 4; i++)
			{
				for (int j = 0; j < 4; j++)
				{
					State[m][i][j] = buff[k++];	//3차원 state 에 데이터값 대입

					if (k == dwReadLen)
					{
						flag = 1;
						break;
					}
				}
				if (flag == 1)
					break;
			}
			if (flag == 1)
				break;
		}


		char buff2[1024][16] = { 0, };

		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				init_CipherKey[i][j] = CipherKey[i][j];	//CipherKey가 변화하기 때문에 init_CipherKey에 저장


		for (int m = 0; m < count; m++)
		{
			m_aes->Encrypt(buff2[m], State[m], CipherKey);	//AES 암호화

			for (int i = 0; i < 4; i++)
				for (int j = 0; j < 4; j++)
					CipherKey[i][j] = init_CipherKey[i][j];	//CipherKey를 원래 키로 다시 되돌려줌
		}


		char *buff3 = (char *)malloc(sizeof(char)*dwReadLen);


		k = 0;
		flag = 0;

		for (int i = 0; i < count; i++)
		{
			for (int j = 0; j < 16; j++)
			{
				buff3[k++] = buff2[i][j];	//소켓통신을 위해 1차원 버퍼에 2차원 암호화 데이터 대입

				if (k == dwReadLen)
				{
					flag = 1;
					break;
				}
			}
			if (flag == 1)
				break;
		}

		OnSend(buff3, dwReadLen);


		temp_date = "";

		query_state2 = mysql_query(connection2, SQL_SELECT);	//쿼리문 실행

		if (query_state2 != 0)	//쿼리문 실행 에러메시지 출력
			AfxMessageBox("Query Select Error!");

		sql_result2 = mysql_store_result(connection2);	//쿼리 실행결과 저장


		while ((sql_row2 = mysql_fetch_row(sql_result2)) != NULL)	//쿼리 실행결과를 한줄씩 읽어옴
		{
			temp_date = sql_row2[4];	//date 값
		}


		CString enc_query = "";	//암호화된 쿼리를 저장할 변수


		int *db_enc = (int *)malloc(sizeof(int)*dwReadLen);	//수신데이터 DB에 ASCII코드 형태로 저장하기 위해 int형 배열 동적할당

		for (int i = 0; i < dwReadLen; i++)
		{
			db_enc[i] = buff3[i];	//char형 int형에 대입

			CString temp_query;

			temp_query.Format("%d", db_enc[i]);	//int -> CString 변환

			if (i % 15 == 0 && i != 0)	//0번째를 제외하고 15번째에는 @를 추가
				temp_query += "@";
			else	//그 외에는 !를 추가
				temp_query += "!";

			enc_query += temp_query;	//해당 데이터를 암호화 쿼리 저장변수에 계속 추가해줌
		}

		free(db_enc);	//동적메모리 해제

		sprintf(query, SQL_INSERT, enc_query, temp_date);	//쿼리에 암호화 데이터 추가
		query_state = mysql_query(connection, query);	//쿼리 실행

		if (query_state != 0)	//쿼리문 실행 에러메시지 출력
			AfxMessageBox("Query Insert Error!");

		for (int i = 0; i < 16; i++)
			delete temp_dec[i];

		//CString 값 비우기(혹시 모를 남아있는 메모리 제거)
		temp1.Empty();
		temp2.Empty();
		textHex.Empty();
		textAscii.Empty();
		////////////////////////////////////////////////

		mysql_free_result(sql_result2);

		free(buff3);
		delete buff;
	
	/*else
	{                                  
		pSock->Close();

		int user_temp = 0;

		user_temp = user;

		while (user_temp)
		{
			user_temp--;
			
			if (cs[user_temp].p == NULL)
			{
				//cs[user_temp].state = "close";
				CLIENT_LIST.DeleteItem(user_temp);

			}
		}

	}*/

}

//소켓생성 및 리스닝 시작버튼
void CTestServerDlg::OnBnClickedSocketCreate()
{
	UpdateData(TRUE);

	AfxSocketInit(NULL);	//소켓초기화
	m_pServerSock = new ServerSock();	//ServerSock 포인터	생성
	m_pServerSock->Create(7777, SOCK_STREAM);	//포트번호 설정 후 소켓생성 
	// 포트번호는 변경가능합니다. (단, 클라이언트에서도 수정이 필요함)
	
	CString str("서버 소켓생성\r\n");
	
	List_ServerLine.InsertString(line++, str);	//소켓생성 메시지 출력

	m_pServerSock->Listen();	//소켓 리스닝 시작
	
	str = "리스닝 시작\r\n";

	List_ServerLine.InsertString(line++, str);	//리스닝 시작 메시지 출력
	List_ServerLine.SetCurSel(line-1);	//리스트박스 선택값 마지막 라인으로 이동

	GetDlgItem(IDC_SOCKET_CREATE)->EnableWindow(FALSE);	//소켓 생성 버튼 비활성화

}

//소켓 해제 버튼
void CTestServerDlg::OnBnClickedSocketClose()
{
	while (user)
	{
		user--;
		delete cs[user].p;
	}

	
	delete m_pServerSock;	//소켓 지움

	CString str("소켓 해제\r\n");

	List_ServerLine.InsertString(line++, str);	//소켓해제 메시지 출력
	List_ServerLine.SetCurSel(line - 1);	//리스트박스 선택값 마지막 라인으로 이동
	
	GetDlgItem(IDC_SOCKET_CLOSE)->EnableWindow(FALSE);	//소켓 해제 버튼 비활성화
	GetDlgItem(IDC_SOCKET_CREATE)->EnableWindow(TRUE);	//소켓 생성 버튼 활성화
	
}

//소켓통신 Send 함수
void CTestServerDlg::OnSend(char *buff, int len)
{
	for (int i = 0; i < user; i++)
	{
		cs[i].p->Send(buff, len);
		
	}

}


//DB 확인버튼 클릭시 
void CTestServerDlg::OnBnClickedDb()
{
	m_dbcheck = new CDB_Check();
	m_dbcheck->Create(IDD_DBCHECK, this);
	m_dbcheck->ShowWindow(SW_SHOW);
	
}



void CTestServerDlg::OnBnClickedIds()
{
	m_ids = new CIDS();
	m_ids->Create(IDD_IDS, this);
	m_ids->ShowWindow(SW_SHOW);
}
