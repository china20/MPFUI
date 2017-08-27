// ======================================================================
//
// Copyright (c) 2008-2022 汪荣(视点UI), Inc. All rights reserved.
//
// MPF界面库遵循指定开源协议，团体或公司商用请根据协议购买授权，
// 任何个人、团体或公司不能居于此库衍生任何商业性质的库或代码。
//
// ======================================================================

/////////////////////////////////////////////////////////////////////////
// WindowHelper.cpp

#include <Framework/Controls/WindowHelper.h>
#include <System/Windows/RenderTarget.h>
#include <System/Windows/CoreTool.h>

namespace suic
{

/////////////////////////////////////////////////////////////////////////
// WndHelper
// 
WndHelper::WndHelper(Element* p) 
    : _owner(p)
{
}

WndHelper::~WndHelper()
{
}

bool WndHelper::OpenClipboard()
{
    VisualHostPtr pHost = VisualHost::GetVisualHost(_owner);

    if (!pHost)
    {
        return false;
    }

    HWND realwnd = HANDLETOHWND(pHost->GetHandle());

    if (::IsWindow(realwnd))
    {
        if (::OpenClipboard(realwnd))
        {
            return true;
        }
    }

#ifdef _DEBUG
    LPVOID lpMsgBuf;
    ::FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        ::GetLastError(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
        (LPTSTR) &lpMsgBuf,
        0,
        NULL
        );
    MessageBox(NULL, (LPCTSTR)lpMsgBuf, _T("Info"), MB_OK | MB_ICONINFORMATION );
    ::LocalFree( lpMsgBuf );
#endif

    return false;
}

void WndHelper::CloseClipboard()
{
    ::CloseClipboard();
}

int WndHelper::PasteText(String & text)
{
    // 首先打开我们自己定义的数据格式
    String UI_EDIT_CFMSDEVCOLSEL(_T("UI_EDIT_BINARY"));
    CLIPFORMAT uCF_MSDEVColumnSelect = (CLIPFORMAT)::RegisterClipboardFormat(UI_EDIT_CFMSDEVCOLSEL.c_str());
    HANDLE hMemText = ::GetClipboardData(uCF_MSDEVColumnSelect);
    
    if (hMemText)
    {
        int nSize = (int)::GlobalSize(hMemText);
        BYTE * arColBlk = (BYTE*)::GlobalLock(hMemText);
    }

    HGLOBAL hmem = NULL;
    if (::IsClipboardFormatAvailable(CF_UNICODETEXT))
    {
        hmem = ::GetClipboardData(CF_UNICODETEXT);
    }

    if (hmem)
    {
        Char * pClipData = (Char *)::GlobalLock(hmem);
        DWORD dwLen = (DWORD)::GlobalSize(hmem);

        text = String(pClipData, (dwLen / 2) - 1);

#ifdef _DEBUG
        _CrtCheckMemory();
#endif
        ::GlobalUnlock(hmem);
    }

    return 0;
}

int WndHelper::CutText(String & text)
{
    int nRet = CopyText(text);
    if (0 == nRet)
    {
        ::EmptyClipboard();
    }
    return nRet;
}

int WndHelper::CopyText(const String & text)
{
    ::EmptyClipboard();

    // 采用unicode编码来存储字符
    Uint32 dataSize = (Uint32)text.Length() * sizeof(Char);

    HGLOBAL hmem = ::GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, dataSize + sizeof(TCHAR));
    LPVOID lpClipData = (LPVOID)::GlobalLock(hmem);

    if (NULL != lpClipData)
    {
        memcpy(lpClipData, text.c_str(), dataSize);
        ::ZeroMemory(((BYTE *)lpClipData + dataSize), sizeof(TCHAR));
    }

    int uCodePage = 0;

    // 计算需要的字节数
    int nLen  = ::WideCharToMultiByte(uCodePage, 0, (LPWSTR)lpClipData, -1, NULL, 0, NULL, NULL);

    HGLOBAL hMBCSMem = ::GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, (dataSize + 2));
    LPSTR lpMBCSSource = (LPSTR)::GlobalLock(hMBCSMem);

    int nBytes = ::WideCharToMultiByte(uCodePage, 0, (LPWSTR)lpClipData, -1, lpMBCSSource, nLen, NULL, NULL);

    assert(nBytes <= (int)nLen);

    lpMBCSSource[nBytes] = _T('\0');

    ::GlobalUnlock(hmem);
    ::GlobalUnlock(hMBCSMem);

    // 我们存储是unicode文本
    ::SetClipboardData(CF_UNICODETEXT, hmem);
    ::SetClipboardData(CF_TEXT, hMBCSMem);
    ::SetClipboardData(CF_OEMTEXT, hMBCSMem);

    return 0;
}

};
