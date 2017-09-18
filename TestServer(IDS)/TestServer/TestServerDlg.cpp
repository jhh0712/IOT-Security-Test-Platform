
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
		AfxMessageBox(_T("WINDOW DB CONNECTION ERROR"));
	}
	 
	connection2 = NULL;
	mysql_init(&conn2);	//Mysql �ʱ�ȭ

	connection2 = mysql_real_connect(&conn2, DB_HOST2, USER_NAME2, USER_PASS2, DB_NAME2, 3306, (char *)NULL, 0);	//Mysql DB������ ����õ�

	if (connection2 == NULL)	//������� �� �����޽��� ���
	{
		AfxMessageBox(_T("KEY MANAGEMENT DB CONNECTION ERROR"));
	}

	CLIENT_LIST.InsertColumn(1, _T("NUM"), LVCFMT_LEFT, 50);		//List Control �ʱ�ȭ
	CLIENT_LIST.InsertColumn(2, _T("ADDRESS"), LVCFMT_LEFT, 150);	//List Control �ʱ�ȭ
	CLIENT_LIST.InsertColumn(3, _T("DATE"), LVCFMT_LEFT, 160);	//List Control �ʱ�ȭ

//	list_count = 0;	//List Control ǥ���� ī��Ʈ �ʱ�ȭ

	//p1 = NULL;

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
/*
UINT CTestServerDlg::ThreadFirst(LPVOID _mothod)
{
	CTestServerDlg *fir = (CTestServerDlg*)_mothod;

	fir->ThreadFunc();	//�����忡�� ������ �Լ�

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
				
				CString str("Ŭ���̾�Ʈ ��������\r\n");

				List_ServerLine.InsertString(line++, str);	//�������� �޽��� ���
				List_ServerLine.SetCurSel(line - 1);	//����Ʈ�ڽ� ���ð� ������ �������� �̵�

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
		DWORD dwResult = ::WaitForSingleObject(p1->m_hThread, INFINITE);	//Ư�� ������Ʈ ���°� ������ �� ���� ���� �������� ������ ����

		if (dwResult == WAIT_TIMEOUT)	//������Ʈ ���°� �������� �������
			AfxMessageBox("Time OUT!");

		delete p1;

		p1 = NULL;
	}
	return true;

}
*/
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
	//cs[user].state = "open";
/*
	CString num = "";
	num.Format("%d", user);

	CTime cTime = CTime::GetCurrentTime(); // ���� �ý������κ��� ��¥ �� �ð��� ��� �´�.

	CString strDate; // ��ȯ�Ǵ� ��¥�� �ð��� ������ CString ���� ����
	strDate.Format("%02d-%02d-%02d %02d:%02d:%02d", cTime.GetYear(), // ���� �⵵ ��ȯ
		cTime.GetMonth(),	// ���� �� ��ȯ
		cTime.GetDay(),		// ���� �� ��ȯ
		cTime.GetHour(),	// ���� �ð� ��ȯ
		cTime.GetMinute(),  // ���� �� ��ȯ
		cTime.GetSecond()); // ���� �� ��ȯ


	CLIENT_LIST.InsertItem(list_count, num);	//���� user ���� List Control�� �߰�
	CLIENT_LIST.SetItem(list_count, 1, LVIF_TEXT, clientName, 0, 0, 0, NULL);		//clientName �� List Control�� �߰�
	CLIENT_LIST.SetItem(list_count, 2, LVIF_TEXT, strDate, 0, 0, 0, NULL);
*/
	user++;
	//list_count++;

	str = clientName;
	str += "���� ����\r\n";

	List_ServerLine.InsertString(line++, str);	//��𿡼� �����ߴ��� �޽��� ���
	List_ServerLine.SetCurSel(line-1);	//����Ʈ�ڽ� ���ð� ������ �������� �̵�
	
	if (m_pServerSock != NULL && cs->p != NULL)
		GetDlgItem(IDC_SOCKET_CLOSE)->EnableWindow(TRUE);	//���� ������ư Ȱ��ȭ

	/*
	if (p1 != NULL)	//p1 ������ �����Ͱ� NULL�� �ƴҶ� (�����尡 �������϶�)
	{
		DestroyTherad();	//p1 ������ �����Լ� ����
	}

	p1 = AfxBeginThread(ThreadFirst, this, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);	//AfxBeginThread �� ������ ����

	if (p1 == NULL)	//p1 ������ �����Ͱ� NULL �ΰ�� (������ ������ �ȵ��� ��)
	{
		AfxMessageBox("Error");
	}

	p1->m_bAutoDelete = FALSE;	//������ �ڵ����� ����
	p1->ResumeThread();

	*/
}


//�񵿱� Receive �Լ�
void CTestServerDlg::OnReceive(ClientSock* pSock)
{
	int e_count = 1;	//Edit Control ������ �ٹٲ� ���� ī��Ʈ
	int k = 0;
	int send_state = 0;	//send ���°�


	DWORD dwReadLen;	//���� ������ ����
	pSock->IOCtl(FIONREAD, &dwReadLen); //���� ������ ���̸� �о��
	
	char *buff = new char[dwReadLen];	//���� ������ ������ ���� �����Ҵ�
	
	pSock->Receive(buff, dwReadLen);	//���� �����͸�ŭ ���ú�
	
	
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
				temp1.Format("%02X\t", value);	//�� ����Ʈ�� HEX �ڵ�� �ٲ���

			textHex += temp1;

			if (e_count % 8 == 0)
				textHex += "\r\n";	//8����Ʈ���� �ٹٲ�

			e_count++;
		}

		e_count = 1;

		for (int i = 0; i < dwReadLen; i++)
		{
			int value = buff[i];

			if (value < 0)
				temp2.Format("%d\t", value & (0xff));
			else
				temp2.Format("%d\t", value);	//�� ����Ʈ�� ASCII �ڵ�� �ٲ���
			textAscii += temp2;

			if (e_count % 8 == 0)
				textAscii += "\r\n";	//8����Ʈ���� �ٹٲ�

			e_count++;
		}

		SetDlgItemText(IDC_DATA_HEX, textHex);	//Edit Control�� ���
		SetDlgItemText(IDC_DATA_ASCII, textAscii);	//Edit Control�� ���

		srand((unsigned int)(time(NULL))); //���� ����

		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				CipherKey[i][j] = rand() % 255;	//CipherKey ����

		int temp_key[16] = { 0, };
		int enc_key[16] = { 0, };
		char temp[10] = { 0, };		//RSA ��ȣȭ �� ������ ����ȯ �ϱ� ���� �ӽ� �迭
		char aes_key[100] = { 0, };	//�������� �ֱ� ���� �迭

		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				temp_key[k++] = CipherKey[i][j];

		m_rsa->Encrypt(enc_key, temp_key);

		for (int i = 0; i < 16; i++)
		{
			sprintf(temp, "%d", enc_key[i]);	//RSA ��ȣȭ �� int�� ������ Char������ ����ȯ
			strcat(aes_key, temp);	//aes_key�� ���� �߰�
			strcat(aes_key, "!");	//����Ʈ ������ ���� ! �߰�
		}

		sprintf(query2, "insert into encryptkey values('%s', '%d', '%d', '%d', now());", aes_key, m_rsa->pq, m_rsa->n, m_rsa->e);	//������ ����

		query_state2 = mysql_query(connection2, query2);	//������ ����
		if (query_state2 != 0) {	//������ ������
			AfxMessageBox("MYSQL QUERY ERROR!");
		}

		int count = 0;
		int count2 = 0;

		int flag = 0;

		count = dwReadLen / 16; 	//3���� �迭 ũ�� ���� ���� ���
		count2 = dwReadLen % 16;	//3���� �迭 ũ�� ���� ���� ���

		if (count2 != 0)	//�������� ������ �迭ũ�� �������Ѿ���
			count++;

		for (int m = 0; m < count; m++)
		{
			for (int i = 0; i < 4; i++)
			{
				for (int j = 0; j < 4; j++)
				{
					State[m][i][j] = buff[k++];	//3���� state �� �����Ͱ� ����

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
				init_CipherKey[i][j] = CipherKey[i][j];	//CipherKey�� ��ȭ�ϱ� ������ init_CipherKey�� ����


		for (int m = 0; m < count; m++)
		{
			m_aes->Encrypt(buff2[m], State[m], CipherKey);	//AES ��ȣȭ

			for (int i = 0; i < 4; i++)
				for (int j = 0; j < 4; j++)
					CipherKey[i][j] = init_CipherKey[i][j];	//CipherKey�� ���� Ű�� �ٽ� �ǵ�����
		}


		char *buff3 = (char *)malloc(sizeof(char)*dwReadLen);


		k = 0;
		flag = 0;

		for (int i = 0; i < count; i++)
		{
			for (int j = 0; j < 16; j++)
			{
				buff3[k++] = buff2[i][j];	//��������� ���� 1���� ���ۿ� 2���� ��ȣȭ ������ ����

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

		query_state2 = mysql_query(connection2, SQL_SELECT);	//������ ����

		if (query_state2 != 0)	//������ ���� �����޽��� ���
			AfxMessageBox("Query Select Error!");

		sql_result2 = mysql_store_result(connection2);	//���� ������ ����


		while ((sql_row2 = mysql_fetch_row(sql_result2)) != NULL)	//���� �������� ���پ� �о��
		{
			temp_date = sql_row2[4];	//date ��
		}


		CString enc_query = "";	//��ȣȭ�� ������ ������ ����


		int *db_enc = (int *)malloc(sizeof(int)*dwReadLen);	//���ŵ����� DB�� ASCII�ڵ� ���·� �����ϱ� ���� int�� �迭 �����Ҵ�

		for (int i = 0; i < dwReadLen; i++)
		{
			db_enc[i] = buff3[i];	//char�� int���� ����

			CString temp_query;

			temp_query.Format("%d", db_enc[i]);	//int -> CString ��ȯ

			if (i % 15 == 0 && i != 0)	//0��°�� �����ϰ� 15��°���� @�� �߰�
				temp_query += "@";
			else	//�� �ܿ��� !�� �߰�
				temp_query += "!";

			enc_query += temp_query;	//�ش� �����͸� ��ȣȭ ���� ���庯���� ��� �߰�����
		}

		free(db_enc);	//�����޸� ����

		sprintf(query, SQL_INSERT, enc_query, temp_date);	//������ ��ȣȭ ������ �߰�
		query_state = mysql_query(connection, query);	//���� ����

		if (query_state != 0)	//������ ���� �����޽��� ���
			AfxMessageBox("Query Insert Error!");

		for (int i = 0; i < 16; i++)
			delete temp_dec[i];

		//CString �� ����(Ȥ�� �� �����ִ� �޸� ����)
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
	{
		cs[i].p->Send(buff, len);
		
	}

}


//DB Ȯ�ι�ư Ŭ���� 
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
