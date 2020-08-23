
// Life-MFC.h : main header file for the Life-MFC application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CLifeMFCApp:
// See Life-MFC.cpp for the implementation of this class
//

class CLifeMFCApp : public CWinApp
{
public:
	CLifeMFCApp() noexcept;


// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation

public:
	afx_msg void OnAppAbout();
	afx_msg void OnViewOptions();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnOptionsOpen();
	afx_msg void OnOptionsSave();
	afx_msg void OnHelpHelp();
};

extern CLifeMFCApp theApp;
