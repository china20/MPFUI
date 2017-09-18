// 华勤科技版权所有 2010-2022
// 
// 文件名：ShellHelper.h
// 功  能：目录操作辅助函数。
// 
// 作  者：MPF开发组
// 时  间：2017-09-02
// 
// ============================================================================

#ifndef _SHELLHELPER_H_
#define _SHELLHELPER_H_

#include "shlobj.h"

class ShellMalloc
{
public:

    ShellMalloc()
    {
        _lpMalloc = NULL;

        if (FAILED(::SHGetMalloc(&_lpMalloc)))
        {
            _lpMalloc = NULL;
        }
    }

    virtual ~ShellMalloc()
    {
        if (_lpMalloc)
        {
            _lpMalloc->Release();
        }
    }

    void* Alloc (DWORD nBytes)
    {
        return _lpMalloc ? _lpMalloc->Alloc(nBytes) : NULL;
    }

    void Free(void* lpMem)
    {
        if (_lpMalloc)
        {
            _lpMalloc->Free(lpMem);
        }
    }

    operator LPMALLOC()
    {
        return _lpMalloc;
    }

protected:

    LPMALLOC _lpMalloc;
};

class ShellHelper
{
public:

    static void MEMCPY_S(void *dest, const void* src, size_t count);

    static ULONG GetAttributes(LPSHELLFOLDER lpsf, LPITEMIDLIST lpi, ULONG uFlags);
    static bool GetName(LPSHELLFOLDER lpsf, LPITEMIDLIST lpi, DWORD dwFlags, suic::String& strFriendlyName);

    static void FreePidl(LPITEMIDLIST pidl);
    static LPITEMIDLIST CreatePidl(LPMALLOC lpMalloc, UINT cbSize);

    static LPITEMIDLIST ConcatPidls(LPCITEMIDLIST pidl1, LPCITEMIDLIST pidl2);
    static LPITEMIDLIST ConcatPidls(LPMALLOC lpMalloc, LPCITEMIDLIST pidl1, LPCITEMIDLIST pidl2);

private:

    static UINT GetPidlCount(LPCITEMIDLIST pidl);
    static UINT GetPidlItemCount(LPCITEMIDLIST pidl);

    static LPITEMIDLIST GetNextPidlItem(LPCITEMIDLIST pidl);

    static BOOL ComparePidls(LPCITEMIDLIST pidl1, LPCITEMIDLIST pidl2, LPSHELLFOLDER pShellFolder);
};

#endif
