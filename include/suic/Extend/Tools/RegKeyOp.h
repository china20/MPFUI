// 华勤科技版权所有 2010-2011
// 
// 文件名：RegKeyOp.h
// 功  能：注册表操作。
// 
// 作  者：MPF开发组
// 时  间：2010-07-02
// 
// ============================================================================

#ifndef _UIREGKEYOP_H_
#define _UIREGKEYOP_H_

#include <Extend/suiwgxconfig.h>
#include <System/Windows/Object.h>

namespace suic
{
    
class SUIWGX_API RegKeyOp
{
public:

    explicit RegKeyOp(HKEY hKey) throw();

    RegKeyOp() throw();
    RegKeyOp(RegKeyOp& key) throw();
    
    ~RegKeyOp() throw();

    RegKeyOp& operator=(RegKeyOp& key) throw();

public:

    operator HKEY() const throw();

    HKEY hkey;
    REGSAM samWOW64;

public:

    LONG SetValue(DWORD dwValue, LPCTSTR lpszValueName);
    LONG SetValue(LPCTSTR lpszValue, LPCTSTR lpszValueName = NULL, bool bMulti = false, int nValueLen = -1);
    LONG SetValue(LPCTSTR pszValueName, DWORD dwType, const void* pValue, ULONG nBytes) throw();
    LONG SetBinaryValue(LPCTSTR pszValueName, const void* pValue, ULONG nBytes) throw();
    LONG SetDWORDValue(LPCTSTR pszValueName, DWORD dwValue) throw();
    LONG SetQWORDValue(LPCTSTR pszValueName, ULONGLONG qwValue) throw();
    LONG SetStringValue(LPCTSTR pszValueName, LPCTSTR pszValue, DWORD dwType = REG_SZ) throw();
    LONG SetMultiStringValue(LPCTSTR pszValueName, LPCTSTR pszValue) throw();

    LONG QueryValue(DWORD& dwValue, LPCTSTR lpszValueName);
    LONG QueryValue(LPTSTR szValue, LPCTSTR lpszValueName, DWORD* pdwCount);
    LONG QueryValue(LPCTSTR pszValueName, DWORD* pdwType, void* pData, ULONG* pnBytes) throw();
    LONG QueryBinaryValue(LPCTSTR pszValueName, void* pValue, ULONG* pnBytes) throw();
    LONG QueryDWORDValue(LPCTSTR pszValueName, DWORD& dwValue) throw();
    LONG QueryQWORDValue(LPCTSTR pszValueName, ULONGLONG& qwValue) throw();
    LONG QueryStringValue(LPCTSTR pszValueName, LPTSTR pszValue, ULONG* pnChars) throw();
    LONG QueryMultiStringValue(LPCTSTR pszValueName, LPTSTR pszValue, ULONG* pnChars) throw();

    LONG GetKeySecurity(SECURITY_INFORMATION si, PSECURITY_DESCRIPTOR psd, LPDWORD pnBytes) throw();
    LONG SetKeySecurity(SECURITY_INFORMATION si, PSECURITY_DESCRIPTOR psd) throw();

    LONG SetKeyValue(LPCTSTR lpszKeyName, LPCTSTR lpszValue, LPCTSTR lpszValueName = NULL) throw();
    
    static LONG WINAPI SetValue(HKEY hKeyParent, LPCTSTR lpszKeyName,LPCTSTR lpszValue, LPCTSTR lpszValueName = NULL);

    LONG Create(HKEY hKeyParent, LPCTSTR lpszKeyName,
        LPTSTR lpszClass = REG_NONE, DWORD dwOptions = REG_OPTION_NON_VOLATILE,
        REGSAM samDesired = KEY_READ | KEY_WRITE,
        LPSECURITY_ATTRIBUTES lpSecAttr = NULL,
        LPDWORD lpdwDisposition = NULL) throw();

    LONG Open(HKEY hKeyParent, LPCTSTR lpszKeyName,REGSAM samDesired = KEY_READ | KEY_WRITE) throw();
    LONG Close() throw();
    LONG Flush() throw();

    HKEY Detach() throw();
    void Attach(HKEY hKey) throw();

    LONG EnumKey(DWORD iIndex, LPTSTR pszName, LPDWORD pnNameLength, FILETIME* pftLastWriteTime = NULL) throw();
    LONG NotifyChangeKeyValue(BOOL bWatchSubtree, DWORD dwNotifyFilter, HANDLE hEvent, BOOL bAsync = TRUE) throw();

    LONG DeleteSubKey(LPCTSTR lpszSubKey) throw();
    LONG RecurseDeleteKey(LPCTSTR lpszKey) throw();
    LONG DeleteValue(LPCTSTR lpszValue) throw();
};

}

#endif
