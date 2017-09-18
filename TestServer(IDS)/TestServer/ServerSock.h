#pragma once

// ServerSock command target

class ServerSock : public CAsyncSocket
{
public:
	ServerSock();
	virtual ~ServerSock();

public:
	virtual void OnAccept(int nErrorCode);
};


