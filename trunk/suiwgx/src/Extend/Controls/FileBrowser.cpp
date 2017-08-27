
/////////////////////////////////////////////////////////////////////////
// FileBrowser.cpp

#include <System/Windows/CoreTool.h>
#include <System/Tools/HwndHelper.h>
#include <Extend/Controls/FileBrowser.h>

namespace suic
{

/////////////////////////////////////////////////////////////////////////
// FileBrowser

FileBrowser::FileBrowser(bool val)
    : _selMulti(val)
{
}

FileBrowser::~FileBrowser()
{
}

bool FileBrowser::InternalOpen(const Element* parent, bool bOpen, Char* filter)
{
    HWND hwnd = HANDLETOHWND(HwndHelper::GetHostHwnd(const_cast<Element*>(parent)));
    if (!hwnd)
    {
        hwnd = HANDLETOHWND(HwndHelper::GetActiveHandle());
    }

    OPENFILENAME ofn;
    TCHAR szFile[260] = {0};

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFile = szFile;

    ofn.lpstrFile[0] = _T('\0');
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = filter;
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST/* | OFN_FILEMUSTEXIST*/;

    if (_selMulti)
    {
        ofn.Flags |= OFN_ALLOWMULTISELECT;
    }

    BOOL bRet = FALSE;

    if (bOpen)
    {
        bRet = GetOpenFileName(&ofn);
    }
    else
    {
        bRet = GetSaveFileName(&ofn);
    }

    if (bRet) 
    {
        _path = szFile;

        return true;
    }
    else
    {
        return false;
    }
}

bool FileBrowser::Open(const Element* parent, Char* filter)
{
    return InternalOpen(parent, true, filter);
}

bool FileBrowser::Save(const Element* parent, Char* filter)
{
    return InternalOpen(parent, false, filter);
}

String FileBrowser::GetFilePath() const
{
    return _path;
}

}
