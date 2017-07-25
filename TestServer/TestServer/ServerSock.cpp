// ServerSock.cpp : implementation file
//

#include "stdafx.h"
#include "TestServer.h"
#include "ServerSock.h"
#include "TestServerDlg.h"


// ServerSock

ServerSock::ServerSock()
{
}

ServerSock::~ServerSock()
{

}

void ServerSock::OnAccept(int nErrorCode)
{
	CTestServerDlg *dlg = (CTestServerDlg*)AfxGetApp()->m_pMainWnd;
	dlg->OnAccept();

	CAsyncSocket::OnAccept(nErrorCode);
}


// ServerSock member functions
