
// RSA_TESTDlg.h : header file
//

#pragma once
#include "RSA_CODE.h"

// CRSA_TESTDlg dialog
class CRSA_TESTDlg : public CDialogEx
{
// Construction
public:
	CRSA_TESTDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_RSA_TEST_DIALOG };
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

	RSA_CODE *m_rsa;

public:
	afx_msg void OnBnClickedOk();
};
