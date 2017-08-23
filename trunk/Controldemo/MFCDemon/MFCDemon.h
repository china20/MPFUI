// MFCDemon.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CMFCDemonApp:
// See MFCDemon.cpp for the implementation of this class
//

class CMFCDemonApp : public CWinApp
{
public:
	CMFCDemonApp();

    // 初始化MPF环境、资源
    void InitMPF();
    void ExitMPF();

// Overrides
	public:
	virtual BOOL InitInstance();

    virtual BOOL PreTranslateMessage(MSG* pMsg);

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CMFCDemonApp theApp;