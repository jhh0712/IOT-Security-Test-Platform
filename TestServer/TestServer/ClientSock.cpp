// ClientSock.cpp : implementation file
//

#include "stdafx.h"
#include "TestServer.h"
#include "ClientSock.h"

#include "TestServerDlg.h"

// ClientSock

ClientSock::ClientSock()
{
}

ClientSock::~ClientSock()
{
}



// ClientSock member functions

void ClientSock::OnReceive(int nErrorCode)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	CTestServerDlg* dlg = (CTestServerDlg*)AfxGetApp()->m_pMainWnd;
	dlg->OnReceive(this);

	CAsyncSocket::OnReceive(nErrorCode);
}
