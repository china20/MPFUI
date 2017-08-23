// MFCDemonDlg.h : header file
//

#pragma once

#include "src/SettingWindow.h"

class CMPFWindow : public suic::Object
{
public:

    CMPFWindow(HWND hwnd)
    {
        m_hWnd = hwnd;
    }

    void OnOkClick(suic::Element* sender, suic::RoutedEventArg* e);

private:

    suic::WindowPtr _wnd;
    HWND m_hWnd;
};

// CMFCDemonDlg dialog
class CMFCDemonDlg : public CDialog
{
// Construction
public:
	CMFCDemonDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_MFCDEMON_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
    BOOL PreTranslateMessage(MSG* pMsg);

    void InitMainUI();

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedOk();

    CMPFWindow* m_mpfWnd;

    suic::WindowPtr m_userName;
    suic::WindowPtr m_userPwd;
    suic::WindowPtr m_loginBtn;
    suic::WindowPtr m_cancelBtn;
};
