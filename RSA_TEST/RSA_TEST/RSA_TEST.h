
// RSA_TEST.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CRSA_TESTApp:
// See RSA_TEST.cpp for the implementation of this class
//

class CRSA_TESTApp : public CWinApp
{
public:
	CRSA_TESTApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CRSA_TESTApp theApp;