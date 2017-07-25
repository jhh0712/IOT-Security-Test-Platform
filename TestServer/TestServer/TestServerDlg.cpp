
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
	delete cs;
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
	m_pServerSock->Accept(*cs[user].p);	//생성된 서버소켓

	CString clientName("");
	UINT clientPort = 0;

	cs[user].p->GetPeerName(clientName, clientPort);	//클라이언트에서 접속한 IP와 포트를 가져옴

	user++;
	
	CString str = clientName + "에서 접속\r\n";

	List_ServerLine.InsertString(line++, str);	//어디에서 접속했는지 메시지 출력
	List_ServerLine.SetCurSel(line-1);

}

//비동기 Receive 함수
void CTestServerDlg::OnReceive(ClientSock* pSock)
{
	char buff[16] = { 0, };

	pSock->Receive(buff, 16);

}

//소켓생성 및 리스닝 시작버튼
void CTestServerDlg::OnBnClickedSocketCreate()
{
	UpdateData(TRUE);

	AfxSocketInit(NULL);	//소켓초기화
	m_pServerSock = new ServerSock();	//ServerSock 포인터	생성
	m_pServerSock->Create(7777, SOCK_STREAM);	//포트번호 설정 후 소켓생성 
	// 포트번호는 변경가능합니다. (단, 클라이언트에서도 수정이 필요함)
	
	CString str("소켓생성\r\n");
	
	List_ServerLine.InsertString(line++, str);	//소켓생성 메시지 출력

	m_pServerSock->Listen();	//소켓 리스닝 시작
	
	str = "리스닝 시작\r\n";

	List_ServerLine.InsertString(line++, str);	//리스닝 시작 메시지 출력
	List_ServerLine.SetCurSel(line-1);

}
