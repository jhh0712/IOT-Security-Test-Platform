
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

	delete m_pServerSock;	//���̾�α� ����� �������� ����
	

	mysql_close(connection);	//Mysql ���� ����
}


CTestServerDlg::CTestServerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_TESTSERVER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	for (int i = 0; i < 16; i++)
		temp_dec[i] = NULL;	//CString �迭 �ʱ�ȭ
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
	ON_BN_CLICKED(IDC_DB, &CTestServerDlg::OnBnClickedDb)

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

	user = 0; //���� ����� �� �ʱ�ȭ
	line = 0; //���� ����ǥ���� �� �ʱ�ȭ


	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			CipherKey[i][j] = 0;	//CipherKey�� �ʱ�ȭ
		

	for (int m = 0; m < 1024; m++)
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				State[m][i][j] = 0;		//State�� �ʱ�ȭ
		

	m_aes = (AES*)AfxGetApp();
	//m_rsa = (RSA_CODE*)AfxGetApp();
	m_rsa = new RSA_CODE();

	connection = NULL;
	mysql_init(&conn);	//Mysql �ʱ�ȭ

	connection = mysql_real_connect(&conn, DB_HOST, USER_NAME, USER_PASS, DB_NAME, 3306, (char *)NULL, 0);	//Mysql DB������ ����õ�

	if (connection == NULL)	//������� �� �����޽��� ���
	{
		AfxMessageBox(_T("DB CONNECTION ERROR"));
	}

	connection2 = NULL;
	mysql_init(&conn2);	//Mysql �ʱ�ȭ

	connection2 = mysql_real_connect(&conn2, DB_HOST2, USER_NAME2, USER_PASS2, DB_NAME2, 3306, (char *)NULL, 0);	//Mysql DB������ ����õ�

	if (connection2 == NULL)	//������� �� �����޽��� ���
	{
		AfxMessageBox(_T("KEY MANAGEMENT DB CONNECTION ERROR"));
	}

	 
	if (m_pServerSock == NULL && cs->p == NULL)
		GetDlgItem(IDC_SOCKET_CLOSE)->EnableWindow(FALSE);	//�������� ��ư ��Ȱ��ȭ


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

//�񵿱� Accept �Լ�
void CTestServerDlg::OnAccept()
{

	cs[user].p = new ClientSock();	//ClientSock ����

	CString str("Ŭ���̾�Ʈ���� ����\r\n");

	List_ServerLine.InsertString(line++, str);	//Ŭ���̾�Ʈ ���� ���� �޽��� ���

	m_pServerSock->Accept(*cs[user].p);	//������ ������������ Ŭ���̾�Ʈ accept

	CString clientName("");
	UINT clientPort = 0;

	cs[user].p->GetPeerName(clientName, clientPort);	//Ŭ���̾�Ʈ���� ������ IP�� ��Ʈ�� ������

	user++;
	
	str = clientName;
	str += "���� ����\r\n";

	List_ServerLine.InsertString(line++, str);	//��𿡼� �����ߴ��� �޽��� ���
	List_ServerLine.SetCurSel(line-1);	//����Ʈ�ڽ� ���ð� ������ �������� �̵�
	
	if (m_pServerSock != NULL && cs->p != NULL)
		GetDlgItem(IDC_SOCKET_CLOSE)->EnableWindow(TRUE);	//���� ������ư Ȱ��ȭ

}


//�񵿱� Receive �Լ�
void CTestServerDlg::OnReceive(ClientSock* pSock)
{
	int count = 1;	//Edit Control ������ �ٹٲ� ���� ī��Ʈ
	int k = 0;

	CString textHex = "";	//Hex ������ ��¿�
	CString textAscii = "";	//Ascii ������ ��¿�

	CString temp1 = "";	//Code ��ȯ�� ���� �ӽ� �����1
	CString temp2 = ""; //Code ��ȯ�� ���� �ӽ� �����2
	
	//������ �̿��� �����ͺ��̽��� ������ ���� CString �ʱ�ȭ
	temp_aes = "";	
	temp_pq = "";
	temp_n = "";
	temp_e = "";
	////////////////////////////////////////////

	
	int Decrypt_RSA[16] = { 0, };	//RSA ��ȣȭ�� ���� �迭
	char Decrypt_AES[1024][16] = { 0, };	//AES ��ȣȭ�� ���� 2���� �迭

	dec[16] = { 0, };	//AES Ű ��ȣȭ�� ���� int �ʱ�ȭ

	for (int i = 0; i < 16; i++)
	{
		temp_dec[i] = new CString();
		*temp_dec[i] = "";	//CString �迭 �ʱ�ȭ
	}
	

	DWORD dwReadLen;	//���� ������ ����
	pSock->IOCtl(FIONREAD, &dwReadLen); //���� ������ ���̸� �о��
	
	char *buff = new char[dwReadLen];	//���� ������ ������ ���� �����Ҵ�
	pSock->Receive(buff, dwReadLen);	//���� �����͸�ŭ ���ú�


	CString enc_query = "";	//��ȣȭ�� ������ ������ ����
	
	
	int *db_enc = (int *)malloc(sizeof(int)*dwReadLen);	//���ŵ����� DB�� ASCII�ڵ� ���·� �����ϱ� ���� int�� �迭 �����Ҵ�

	for (int i = 0; i < dwReadLen; i++)
	{
		db_enc[i] = buff[i];	//char�� int���� ����

		CString temp_query;

		temp_query.Format("%d", db_enc[i]);	//int -> CString ��ȯ

		if (i % 15 == 0 && i != 0)	//0��°�� �����ϰ� 15��°���� @�� �߰�
			temp_query += "@";
		else	//�� �ܿ��� !�� �߰�
			temp_query += "!";

		enc_query += temp_query;	//�ش� �����͸� ��ȣȭ ���� ���庯���� ��� �߰�����
	}

	free(db_enc);	//�����޸� ����
	
	int count2 = 0;

	count2 = dwReadLen / 16;	//3���� �迭 ����

	k = 0;

	int flag = 0;

	for (int m = 0; m < count2; m++)
	{
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				State[m][i][j] = buff[k++];	//���� �����͸� ���� 3���� �迭�� ����

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

	query_state2 = mysql_query(connection2, SQL_SELECT);	//������ ����

	if(query_state2 != 0)	//������ ���� �����޽��� ���
		AfxMessageBox("Query Select Error!");	

	sql_result2 = mysql_store_result(connection2);	//���� ������ ����

	
	while ((sql_row2 = mysql_fetch_row(sql_result2)) != NULL)	//���� �������� ���پ� �о��
	{
		temp_aes = sql_row2[0];	//!���� ��ȣȭ�� AESŰ
		
		temp_pq = sql_row2[1];	//RSA pq��
		temp_n = sql_row2[2];	//RSA n��
		temp_e = sql_row2[3];	//RSA e��

		temp_date = sql_row2[4]; //date ��
	}

	//int������ ����ȯ//////////
	m_rsa->pq = _ttoi(temp_pq);
	m_rsa->n = _ttoi(temp_n);
	m_rsa->e = _ttoi(temp_e);
	//////////////////////////

	sprintf(query, SQL_INSERT, enc_query, temp_date);	//������ ��ȣȭ ������ �߰�
	query_state = mysql_query(connection, query);	//���� ����

	if (query_state != 0)	//������ ���� �����޽��� ���
		AfxMessageBox("Query Insert Error!");

	char* st = LPSTR(LPCTSTR(temp_aes));	//!�и��ϱ� ���� char�� �迭�� ����ȯ
	k = 0;

	for (int i = 0; i < temp_aes.GetLength(); i++)
	{
		if (st[i] != '!')	//�迭���� !�� ������
		{
			*temp_dec[k] += st[i];	//CString�迭�� �� �߰�
		}
		else	//!�� ������
		{
			if (i == 0)
				k = 0;
			else
				k++;	//k ����
		}
	}

	for (int i = 0; i < 16; i++)
		dec[i] = _ttoi(*temp_dec[i]);	//int�� �迭�� CString���� �и��� �����Ͱ� ����ȯ �� ����
		

	m_rsa->Decrypt(Decrypt_RSA, dec);	//RSA ��ȣȭ

	k = 0;

	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			CipherKey[i][j] = Decrypt_RSA[k++];	//��ȣȭ�� Ű���� CipherKey�� ����

	
	for (int i = 0; i < count2; i++)
		m_aes->Decrypt(Decrypt_AES[i], State[i], CipherKey);	//State ���� CipherKey�� �̿��� AES ��ȣȭ��Ŵ


	for (int m = 0; m < count2; m++)
	{
		for (int i = 0; i < 16; i++)
		{
			int value = Decrypt_AES[m][i];

			if (value < 0)
				temp1.Format("%02X\t", value & (0xff));
			else
				temp1.Format("%02X\t", value);	//�� ����Ʈ�� HEX �ڵ�� �ٲ���
			
			textHex += temp1;

			if (count % 8 == 0)
				textHex += "\r\n";	//8����Ʈ���� �ٹٲ�

			count++;
		}
	}

	count = 1;

	for (int m = 0; m < count2; m++)
	{
		for (int i = 0; i < 16; i++)
		{
			int value = Decrypt_AES[m][i];

			if (value < 0)
				temp2.Format("%d\t", value & (0xff));
			else
				temp2.Format("%d\t", value);	//�� ����Ʈ�� ASCII �ڵ�� �ٲ���

			textAscii += temp2;

			if (count % 8 == 0)
				textAscii += "\r\n";	//8����Ʈ���� �ٹٲ�

			count++;		
		}
	}

	SetDlgItemText(IDC_DATA_HEX, textHex);	//Edit Control�� ���
	SetDlgItemText(IDC_DATA_ASCII, textAscii);	//Edit Control�� ���

	OnSend(buff, dwReadLen);

	for (int i = 0; i < 16; i++)
		delete temp_dec[i];

	//CString �� ����(Ȥ�� �� �����ִ� �޸� ����)
	textHex.Empty();
	textAscii.Empty();
	temp1.Empty();
	temp2.Empty();
	temp_aes.Empty();
	temp_pq.Empty();
	temp_n.Empty();	
	temp_e.Empty();
	////////////////////////////////////////////////

	mysql_free_result(sql_result2);
	
	delete buff;

	memset(st, 0, sizeof(st));
	
}

//���ϻ��� �� ������ ���۹�ư
void CTestServerDlg::OnBnClickedSocketCreate()
{
	UpdateData(TRUE);

	AfxSocketInit(NULL);	//�����ʱ�ȭ
	m_pServerSock = new ServerSock();	//ServerSock ������	����
	m_pServerSock->Create(7777, SOCK_STREAM);	//��Ʈ��ȣ ���� �� ���ϻ��� 
	// ��Ʈ��ȣ�� ���氡���մϴ�. (��, Ŭ���̾�Ʈ������ ������ �ʿ���)
	
	CString str("���� ���ϻ���\r\n");
	
	List_ServerLine.InsertString(line++, str);	//���ϻ��� �޽��� ���

	m_pServerSock->Listen();	//���� ������ ����
	
	str = "������ ����\r\n";

	List_ServerLine.InsertString(line++, str);	//������ ���� �޽��� ���
	List_ServerLine.SetCurSel(line-1);	//����Ʈ�ڽ� ���ð� ������ �������� �̵�

	GetDlgItem(IDC_SOCKET_CREATE)->EnableWindow(FALSE);	//���� ���� ��ư ��Ȱ��ȭ

}

//���� ���� ��ư
void CTestServerDlg::OnBnClickedSocketClose()
{
	while (user)
	{
		user--;
		delete cs[user].p;
	}

	
	delete m_pServerSock;	//���� ����

	CString str("���� ����\r\n");

	List_ServerLine.InsertString(line++, str);	//�������� �޽��� ���
	List_ServerLine.SetCurSel(line - 1);	//����Ʈ�ڽ� ���ð� ������ �������� �̵�
	
	GetDlgItem(IDC_SOCKET_CLOSE)->EnableWindow(FALSE);	//���� ���� ��ư ��Ȱ��ȭ
	GetDlgItem(IDC_SOCKET_CREATE)->EnableWindow(TRUE);	//���� ���� ��ư Ȱ��ȭ
	
}

//������� Send �Լ�
void CTestServerDlg::OnSend(char *buff, int len)
{
	
	for (int i = 0; i < user; i++)
		cs[i].p->Send(buff, len);

}


//DB Ȯ�ι�ư Ŭ���� 
void CTestServerDlg::OnBnClickedDb()
{
	m_dbcheck = new CDB_Check();
	m_dbcheck->Create(IDD_DBCHECK, this);
	m_dbcheck->ShowWindow(SW_SHOW);
	
}

