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
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	CTestServerDlg* dlg = (CTestServerDlg*)AfxGetApp()->m_pMainWnd;
	dlg->OnReceive(this);

	CAsyncSocket::OnReceive(nErrorCode);
}
