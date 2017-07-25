#pragma once

// ClientSock command target

class ClientSock : public CAsyncSocket
{
public:
	ClientSock();
	virtual ~ClientSock();

public:
	virtual void OnReceive(int nErrorCode);
};


