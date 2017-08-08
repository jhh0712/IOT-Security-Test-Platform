
// TestServerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TestServer.h"
#include "TestServerDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


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
}


CTestServerDlg::CTestServerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_TESTSERVER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
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
CString textHex("");
//비동기 Receive 함수
void CTestServerDlg::OnReceive(ClientSock* pSock)
{
	int count = 1;	//줄바꿈 위한 카운트
	
	DWORD dwReadLen;	//수신 데이터 길이
	pSock->IOCtl(FIONREAD, &dwReadLen); //수신 데이터 길이를 읽어옴

	char *buff = new char[dwReadLen];	//수신 데이터 저장할 버퍼 동적할당
	
	pSock->Receive(buff, dwReadLen);	//수신 데이터만큼 리시브

	//CString textHex("");
	CString textAscii("");
	CString temp("");

	for (int i = 0; i < dwReadLen; i++)
	{
		int value = buff[i];
		temp.Format(_T("%02X\t"), value);	//각 바이트를 HEX 코드로 바꿔줌
		textHex += buff[i];
		
		if (count % 9 == 0)
			textHex += "\r\n";	//8바이트마다 줄바꿈
		
		count++;
	}

	temp = "";
	count = 1;

	for (int i = 0; i < dwReadLen; i++)
	{
		int value = buff[i];
		temp.Format(_T("%d\t"), value);	//각 바이트를 ASCII 코드로 바꿔줌
		textAscii += temp;

		if (count % 8 == 0)
			textAscii += "\r\n";	//8바이트마다 줄바꿈

		count++;
	}

	SetDlgItemText(IDC_DATA_HEX, textHex);	//Edit Control에 출력
	SetDlgItemText(IDC_DATA_ASCII, textAscii);	//Edit Control에 출력


	OnSend(buff, dwReadLen);

	delete buff;

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