// MFCDemon.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "MFCDemon.h"
#include "MFCDemonDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMFCDemonApp

BEGIN_MESSAGE_MAP(CMFCDemonApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CMFCDemonApp construction

CMFCDemonApp::CMFCDemonApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CMFCDemonApp object

CMFCDemonApp theApp;

// CMFCDemonApp initialization

BOOL CMFCDemonApp::PreTranslateMessage(MSG* pMsg)
{
    return CWinApp::PreTranslateMessage(pMsg);
}

BOOL CMFCDemonApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

    // 初始化MPF
    InitMPF();

	CMFCDemonDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

    ExitMPF();

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

void CMFCDemonApp::InitMPF()
{
    // 初始化MPF环境
    suic::InitUIWgx();

    suic::ThemeOp::SetSystemTheme("default.sres", "wr");
    suic::String strApp = _T("ControlDemo/MFCDemon/Application.xml");

    // 载入全局资源
    suic::Application::LoadApp(NULL, strApp);
}

void CMFCDemonApp::ExitMPF()
{
    // 清除MPF资源
    suic::ExitUIWgx();
}
