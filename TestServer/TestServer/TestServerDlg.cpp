
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
	delete m_pServerSock;	//다이얼로그 종료시 생성소켓 지움
	while (user)
	{
		user--;
		delete cs[user].p;
	}

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

}

BEGIN_MESSAGE_MAP(CTestServerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_SOCKET_CREATE, &CTestServerDlg::OnBnClickedSocketCreate)
	ON_BN_CLICKED(IDC_SOCKET_CLOSE, &CTestServerDlg::OnBnClickedSocketClose)
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
	{
		for (int j = 0; j < 4; j++)
		{
			CipherKey[i][j] = 0;
			State[i][j] = 0;	//CipherKey, State 값 초기화
		}
	}

	m_aes = (AES*)AfxGetApp();
	//m_rsa = (RSA_CODE*)AfxGetApp();
	m_rsa = new RSA_CODE();

	connection = NULL;
	mysql_init(&conn);	//Mysql 초기화

	connection = mysql_real_connect(&conn, DB_HOST, USER_NAME, USER_PASS, DB_NAME, 3306, (char *)NULL, 0);	//서버에 연결시도

	if (connection == NULL)	//연결실패 시 에러메시지 출력
	{
		AfxMessageBox(_T("DB CONNECTION ERROR"));
	}
	 
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

//비동기 Accept 함수
void CTestServerDlg::OnAccept()
{

	cs[user].p = new ClientSock();	//ClientSock 구조체 생성

	CString str("클라이언트소켓 생성\r\n");

	List_ServerLine.InsertString(line++, str);	//클라이언트 소켓 생성 메시지 출력

	m_pServerSock->Accept(*cs[user].p);	//생성된 서버소켓으로 클라이언트 accept

	CString clientName("");
	UINT clientPort = 0;

	cs[user].p->GetPeerName(clientName, clientPort);	//클라이언트에서 접속한 IP와 포트를 가져옴

	user++;
	
	str = clientName;
	str += "에서 접속\r\n";

	List_ServerLine.InsertString(line++, str);	//어디에서 접속했는지 메시지 출력
	List_ServerLine.SetCurSel(line-1);	//리스트박스 선택값 마지막 라인으로 이동
	
	if (m_pServerSock != NULL && cs->p != NULL)
		GetDlgItem(IDC_SOCKET_CLOSE)->EnableWindow(TRUE);	//소켓 해제버튼 활성화

}

//비동기 Receive 함수
void CTestServerDlg::OnReceive(ClientSock* pSock)
{
	int count = 1;	//줄바꿈 위한 카운트
	int k = 0;

	CString textHex = "";	//Hex 데이터 출력용
	CString textAscii = "";	//Ascii 데이터 출력용

	CString temp1 = "";	//Code 변환을 위한 임시 저장용1
	CString temp2 = ""; //Code 변환을 위한 임시 저장용2
	
	//쿼리를 이용해 데이터베이스값 저장할 변수 선언
	CString temp_aes = "";	
	CString temp_pq = "";
	CString temp_n = "";
	CString temp_e = "";
	////////////////////////////////////////////

	dec[16] = { 0, };

	int Decrypt_RSA[16];	//RSA 복호화값 받을 포인터
	char Decrypt_AES[16];	//AES 복호화값 받을 포인터

	for (int i = 0; i < 16; i++)
	{
		temp_dec[i] = new CString();
		*temp_dec[i] = "";
	}
	
	DWORD dwReadLen;	//수신 데이터 길이
	pSock->IOCtl(FIONREAD, &dwReadLen); //수신 데이터 길이를 읽어옴
	
	//dwReadLen = 16;
	
	char *buff = new char[dwReadLen];	//수신 데이터 저장할 버퍼 동적할당
	
	pSock->Receive(buff, dwReadLen);	//수신 데이터만큼 리시브

	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			State[i][j] = buff[k++];	//수신 데이터를 상태 2차원 배열에 대입

	query_state = mysql_query(connection, SQL_SELECT);	//쿼리문 실행

	if(query_state != 0)	//쿼리문 실행 에러메시지 출력
		AfxMessageBox("Query Error");	

	sql_result = mysql_store_result(connection);	//쿼리 실행결과 저장

	k = 0;

	while ((sql_row = mysql_fetch_row(sql_result)) != NULL)	//쿼리 실행결과를 한줄씩 읽어옴
	{
		temp_aes = sql_row[0];	//!포함 암호화된 AES키
		
		temp_pq = sql_row[1];	//RSA pq값
		temp_n = sql_row[2];	//RSA n값
		temp_e = sql_row[3];	//RSA e값
	}

	//int형으로 형변환//////////
	m_rsa->pq = _ttoi(temp_pq);
	m_rsa->n = _ttoi(temp_n);
	m_rsa->e = _ttoi(temp_e);
	//////////////////////////

	char* st = LPSTR(LPCTSTR(temp_aes));	//!분리하기 위해 char형 배열로 형변환
	
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

	m_aes->Decrypt(Decrypt_AES, State, CipherKey);	//State 값을 CipherKey를 이용해 AES 복호화시킴

	for (int i = 0; i < 16; i++)
	{
		int value = Decrypt_AES[i];
		temp1.Format("%02X\t", value);	//각 바이트를 HEX 코드로 바꿔줌
		textHex += temp1;
		
		if (count % 8 == 0)
			textHex += "\r\n";	//8바이트마다 줄바꿈
		
		count++;
	}

	count = 1;

	for (int i = 0; i < 16; i++)
	{
		int value = Decrypt_AES[i];
		temp2.Format("%d\t", value);	//각 바이트를 ASCII 코드로 바꿔줌
		textAscii += temp2;

		if (count % 8 == 0)
			textAscii += "\r\n";	//8바이트마다 줄바꿈

		count++;
	}

	SetDlgItemText(IDC_DATA_HEX, textHex);	//Edit Control에 출력
	SetDlgItemText(IDC_DATA_ASCII, textAscii);	//Edit Control에 출력

	OnSend(buff, dwReadLen);

	//CString 값 비우기(혹시 모를 남아있는 메모리 제거)
	textHex.Empty();
	textAscii.Empty();
	temp1.Empty();
	temp2.Empty();
	temp_aes.Empty();
	temp_pq.Empty();
	temp_n.Empty();
	temp_e.Empty();
	////////////////////////////////////////////////

	mysql_free_result(sql_result);
	
	delete buff;
	
	//m_aes->FreeFunc(buff2);
	//m_rsa->FreeFunc(temp_key);

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
	
	delete m_pServerSock;	//소켓 지움

	while (user)
	{
		user--;
		delete cs[user].p;
	}

	GetDlgItem(IDC_SOCKET_CLOSE)->EnableWindow(FALSE);	//소켓 해제 버튼 비활성화
	GetDlgItem(IDC_SOCKET_CREATE)->EnableWindow(TRUE);	//소켓 생성 버튼 활성화
	
}

void CTestServerDlg::OnSend(char *buff, int len)
{
	
	for (int i = 0; i < user; i++)
		cs[i].p->Send(buff, len);

}