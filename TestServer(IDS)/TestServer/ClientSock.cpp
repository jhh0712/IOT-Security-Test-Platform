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
	
	CTestServerDlg* dlg = (CTestServerDlg*)AfxGetApp()->m_pMainWnd;
	dlg->OnReceive(this);

	CAsyncSocket::OnReceive(nErrorCode);
}
