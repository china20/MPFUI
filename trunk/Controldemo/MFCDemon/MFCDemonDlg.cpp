// MFCDemonDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MFCDemon.h"
#include "MFCDemonDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CMFCDemonDlg dialog




CMFCDemonDlg::CMFCDemonDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMFCDemonDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCDemonDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BOOL CMFCDemonDlg::PreTranslateMessage(MSG* pMsg)
{
    if (WM_ERASEBKGND == pMsg->message)
    {
        return 1;
    }
    if (pMsg->message >= WM_KEYFIRST && pMsg->message <= WM_KEYLAST)
    {
        return 0;
    }
    return CDialog::PreTranslateMessage(pMsg);
}

BEGIN_MESSAGE_MAP(CMFCDemonDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
    ON_BN_CLICKED(IDOK, &CMFCDemonDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CMFCDemonDlg message handlers

BOOL CMFCDemonDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

    m_mpfWnd = new CMPFWindow(GetSafeHwnd());
    m_mpfWnd->ref();

    InitMainUI();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMFCDemonDlg::InitMainUI()
{
    m_userName = new suic::Window();
    m_userName->ShowChild("ControlDemo/MFCDemon/UserWindow.xml", HWNDTOHANDLE(GetSafeHwnd()));

    m_userPwd = new suic::Window();
    m_userPwd->ShowChild("ControlDemo/MFCDemon/PwdWindow.xml", HWNDTOHANDLE(GetSafeHwnd()));

    m_loginBtn = new suic::Window();
    m_loginBtn->ShowChild("ControlDemo/MFCDemon/OkWindow.xml", HWNDTOHANDLE(GetSafeHwnd()));
    m_loginBtn->AddHandler(suic::Button::ClickEvent, new suic::ClickEventHandler(m_mpfWnd, &CMPFWindow::OnOkClick));

}

void CMFCDemonDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMFCDemonDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMFCDemonDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CMFCDemonDlg::OnBnClickedOk()
{
}

void CMPFWindow::OnOkClick(suic::Element* sender, suic::RoutedEventArg* e)
{
    _wnd = new suic::Window();

    // 直接以对话框方式
    _wnd->ShowDialog("ControlDemo/MFCDemon/SettingWindow.xml");
    e->SetHandled(true);
}
