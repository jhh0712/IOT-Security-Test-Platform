
// TestServerDlg.h : header file
//

#pragma once

#include "ServerSock.h"		//ServerSock ����߰�
#include "ClientSock.h"		//ClientSock ����߰�
#include "afxwin.h"

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

	CListBox List_ServerLine;	//���� ����ǥ����

	int line;
	afx_msg void OnBnClickedSocketClose();
	
};
