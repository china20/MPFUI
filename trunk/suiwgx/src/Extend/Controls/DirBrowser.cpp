
/////////////////////////////////////////////////////////////////////////
// DirBrowser.cpp

#include <Windows.h>
#include "commdlg.h"
#include <ShlObj.h>
#include <System/Tools/HwndHelper.h>
#include <System/Windows/CoreTool.h>
#include <Extend/Controls/DirBrowser.h>

namespace suic
{

DirBrowser::DirBrowser()
{
}

DirBrowser::~DirBrowser()
{
}

int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)     
{     
    /*if  (uMsg == BFFM_INITIALIZED)  
    {     
        ::SendMessage(hwnd, BFFM_SETSELECTION, TRUE, lpData);     
    }*/

    return 0;    
}

bool DirBrowser::Open(const Element* owner, String dir, bool bMulti/*=false*/)
{
    return Open(owner, _U("Ñ¡ÔñÄ¿Â¼"), dir, bMulti);
}

bool DirBrowser::Open(const Element* owner, String title, String dir, bool bMulti/*=false*/)
{
    HWND hwnd = HANDLETOHWND(HwndHelper::GetHostHwnd(const_cast<Element*>(owner)));

    if (!hwnd)
    {
        hwnd = HANDLETOHWND(HwndHelper::GetActiveHandle());
    }

    LPITEMIDLIST pidlRoot = NULL;  
    String curPath(dir);  
    BROWSEINFO bi;  
    Char buffer[MAX_PATH] = {0};

    bi.hwndOwner = hwnd;  
    bi.pidlRoot = NULL;
    bi.pszDisplayName = NULL;  
    bi.lpszTitle = title.c_str();
    bi.ulFlags = BIF_EDITBOX | BIF_NEWDIALOGSTYLE | BIF_STATUSTEXT;  
    bi.lpfn = BrowseCallbackProc;  
    bi.lParam = (LPARAM)dir.c_str();  
    bi.iImage = 0;  
    LPITEMIDLIST pList = NULL; 

    if ((pList = SHBrowseForFolder(&bi)) != NULL)  
    {  
        suic::Char path[MAX_PATH] = {0};  

        SHGetPathFromIDList(pList, path);
        _dir = path;

        return true;
    }
    else
    {
        return false;
    }
}

String DirBrowser::GetDir() const
{
    return _dir;
}

};
