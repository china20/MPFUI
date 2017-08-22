
#include "stdafx.h"
#include <Extend/Tools/RegKeyOp.h>

namespace suic
{

RegKeyOp::RegKeyOp() throw() 
    : hkey( NULL ), samWOW64(0)
{
}

RegKeyOp::RegKeyOp( RegKeyOp& key ) throw() 
    : hkey( NULL )
    , samWOW64(key.samWOW64)
{
    Attach( key.Detach() );
}

RegKeyOp::RegKeyOp(HKEY hKey) throw() 
    : hkey(hKey)
    , samWOW64(0)
{
}

RegKeyOp::~RegKeyOp() throw()
{
    Close();
}

RegKeyOp& RegKeyOp::operator=( RegKeyOp& key ) throw()
{
    if (hkey!=key.hkey)
    {
        Close();
        Attach( key.Detach() );
        samWOW64 = key.samWOW64;
    }
    return (*this);
}

RegKeyOp::operator HKEY() const throw()
{
    return hkey;
}

HKEY RegKeyOp::Detach() throw()
{
    HKEY hKey = hkey;
    hkey = NULL;
    samWOW64 = 0;
    return hKey;
}

void RegKeyOp::Attach(HKEY hKey) throw()
{
    //ATLASSUME(hkey == NULL);
    hkey = hKey;
    samWOW64 = 0;
}

LONG RegKeyOp::DeleteSubKey(LPCTSTR lpszSubKey) throw()
{
    //ATLASSUME(hkey != NULL);

#if WINVER >= 0x0501
    typedef LSTATUS (WINAPI * PFNRegDeleteKeyEx)(HKEY, LPCTSTR, REGSAM, DWORD);
    static bool bInitialized = false;
    static PFNRegDeleteKeyEx pfnRegDeleteKeyEx = NULL;

    if (!bInitialized)
    {
        HMODULE hAdvapi32 = GetModuleHandle(_T("Advapi32.dll"));
        if (hAdvapi32 != NULL)
        {
#ifdef _UNICODE
            pfnRegDeleteKeyEx = (PFNRegDeleteKeyEx)GetProcAddress(hAdvapi32, "RegDeleteKeyExW");
#else
            pfnRegDeleteKeyEx = (PFNRegDeleteKeyEx)GetProcAddress(hAdvapi32, "RegDeleteKeyExA");
#endif	// _UNICODE
        }
        bInitialized = true;
    }

    if (pfnRegDeleteKeyEx != NULL)
    {
        return pfnRegDeleteKeyEx(hkey, lpszSubKey, samWOW64, 0);
    }

#endif	// WINVER

    return RegDeleteKey(hkey, lpszSubKey);
}

LONG RegKeyOp::DeleteValue(LPCTSTR lpszValue) throw()
{
    //ATLASSUME(hkey != NULL);
    return RegDeleteValue(hkey, (LPTSTR)lpszValue);
}

LONG RegKeyOp::Close() throw()
{
    LONG lRes = ERROR_SUCCESS;

    if (hkey != NULL)
    {
        lRes = RegCloseKey(hkey);
        hkey = NULL;
    }

    samWOW64 = 0;
    return lRes;
}

LONG RegKeyOp::Flush() throw()
{
    //ATLASSUME(hkey != NULL);

    return ::RegFlushKey(hkey);
}

LONG RegKeyOp::EnumKey(DWORD iIndex, LPTSTR pszName, LPDWORD pnNameLength, FILETIME* pftLastWriteTime) throw()
{
    FILETIME ftLastWriteTime;

    //ATLASSUME(hkey != NULL);

    if (pftLastWriteTime == NULL)
    {
        pftLastWriteTime = &ftLastWriteTime;
    }

    return ::RegEnumKeyEx(hkey, iIndex, pszName, pnNameLength, NULL, NULL, NULL, pftLastWriteTime);
}

LONG RegKeyOp::NotifyChangeKeyValue(BOOL bWatchSubtree, DWORD dwNotifyFilter, HANDLE hEvent, BOOL bAsync) throw()
{
    //ATLASSUME(hkey != NULL);
    //ATLASSERT((hEvent != NULL) || !bAsync);

    return ::RegNotifyChangeKeyValue(hkey, bWatchSubtree, dwNotifyFilter, hEvent, bAsync);
}

LONG RegKeyOp::Create(HKEY hKeyParent, LPCTSTR lpszKeyName,
                            LPTSTR lpszClass, DWORD dwOptions, REGSAM samDesired,
                            LPSECURITY_ATTRIBUTES lpSecAttr, LPDWORD lpdwDisposition) throw()
{
    //ATLASSERT(hKeyParent != NULL);
    DWORD dw;
    HKEY hKey = NULL;
    LONG lRes = RegCreateKeyEx(hKeyParent, lpszKeyName, 0,
        lpszClass, dwOptions, samDesired, lpSecAttr, &hKey, &dw);

    if (lpdwDisposition != NULL)
    {
        *lpdwDisposition = dw;
    }

    if (lRes == ERROR_SUCCESS)
    {
        lRes = Close();
        hkey = hKey;
#if WINVER >= 0x0501
        samWOW64 = samDesired & (KEY_WOW64_32KEY | KEY_WOW64_64KEY);
#endif
    }
    return lRes;
}

LONG RegKeyOp::Open(HKEY hKeyParent, LPCTSTR lpszKeyName, REGSAM samDesired) throw()
{
    //ATLASSUME(hKeyParent != NULL);
    HKEY hKey = NULL;
    LONG lRes = RegOpenKeyEx(hKeyParent, lpszKeyName, 0, samDesired, &hKey);
    if (lRes == ERROR_SUCCESS)
    {
        lRes = Close();
        //ATLASSERT(lRes == ERROR_SUCCESS);
        hkey = hKey;
#if WINVER >= 0x0501
        samWOW64 = samDesired & (KEY_WOW64_32KEY | KEY_WOW64_64KEY);
#endif
    }
    return lRes;
}

#pragma warning(push)
#pragma warning(disable: 4996)
LONG RegKeyOp::QueryValue(DWORD& dwValue, LPCTSTR lpszValueName)
{
    DWORD dwType = NULL;
    DWORD dwCount = sizeof(DWORD);
    LONG lRes = RegQueryValueEx(hkey, lpszValueName, NULL, &dwType,
        (LPBYTE)&dwValue, &dwCount);
    //ATLASSERT((lRes!=ERROR_SUCCESS) || (dwType == REG_DWORD));
    //ATLASSERT((lRes!=ERROR_SUCCESS) || (dwCount == sizeof(DWORD)));
    if (dwType != REG_DWORD)
        return ERROR_INVALID_DATA;
    return lRes;
}

#pragma warning(disable: 6053 6385)
LONG RegKeyOp::QueryValue(LPTSTR pszValue, LPCTSTR lpszValueName, DWORD* pdwCount)
{
    //ATLENSURE(pdwCount != NULL);
    DWORD dwType = NULL;
    LONG lRes = RegQueryValueEx(hkey, lpszValueName, NULL, &dwType, (LPBYTE)pszValue, pdwCount);
    //ATLASSERT((lRes!=ERROR_SUCCESS) || (dwType == REG_SZ) ||
    //    (dwType == REG_MULTI_SZ) || (dwType == REG_EXPAND_SZ));
    if (pszValue != NULL)
    {
        if(*pdwCount>0)
        {
            switch(dwType)
            {
            case REG_SZ:
            case REG_EXPAND_SZ:
                if ((*pdwCount) % sizeof(TCHAR) != 0 || pszValue[(*pdwCount) / sizeof(TCHAR) - 1] != 0)
                {
                    pszValue[0]=_T('\0');
                    return ERROR_INVALID_DATA;
                }
                break;
            case REG_MULTI_SZ:
                if ((*pdwCount) % sizeof(TCHAR) != 0 || (*pdwCount) / sizeof(TCHAR) < 1 || pszValue[(*pdwCount) / sizeof(TCHAR) -1] != 0 || (((*pdwCount) / sizeof(TCHAR))>1 && pszValue[(*pdwCount) / sizeof(TCHAR) - 2] != 0) )
                {
                    pszValue[0]=_T('\0');
                    return ERROR_INVALID_DATA;
                }
                break;
            default:
                // Ensure termination
                pszValue[0]=_T('\0');
                return ERROR_INVALID_DATA;
            }
        }
        else
        {
            // this is a blank one with no data yet
            // Ensure termination
            pszValue[0]=_T('\0');
        }
    }
    return lRes;
}
#pragma warning(pop)	

LONG RegKeyOp::QueryValue(LPCTSTR pszValueName, DWORD* pdwType, void* pData, ULONG* pnBytes) throw()
{
    //ATLASSUME(hkey != NULL);

    return( ::RegQueryValueEx(hkey, pszValueName, NULL, pdwType, static_cast< LPBYTE >( pData ), pnBytes) );
}

LONG RegKeyOp::QueryDWORDValue(LPCTSTR pszValueName, DWORD& dwValue) throw()
{
    LONG lRes;
    ULONG nBytes;
    DWORD dwType;

    //ATLASSUME(hkey != NULL);

    nBytes = sizeof(DWORD);
    lRes = ::RegQueryValueEx(hkey, pszValueName, NULL, &dwType, reinterpret_cast<LPBYTE>(&dwValue),
        &nBytes);
    if (lRes != ERROR_SUCCESS)
        return lRes;
    if (dwType != REG_DWORD)
        return ERROR_INVALID_DATA;

    return ERROR_SUCCESS;
}
LONG RegKeyOp::QueryQWORDValue(LPCTSTR pszValueName, ULONGLONG& qwValue) throw()
{
    LONG lRes;
    ULONG nBytes;
    DWORD dwType;

    //ATLASSUME(hkey != NULL);

    nBytes = sizeof(ULONGLONG);
    lRes = ::RegQueryValueEx(hkey, pszValueName, NULL, &dwType, reinterpret_cast<LPBYTE>(&qwValue),
        &nBytes);
    if (lRes != ERROR_SUCCESS)
        return lRes;
    if (dwType != REG_QWORD)
        return ERROR_INVALID_DATA;

    return ERROR_SUCCESS;
}

LONG RegKeyOp::QueryBinaryValue(LPCTSTR pszValueName, void* pValue, ULONG* pnBytes) throw()
{
    LONG lRes;
    DWORD dwType;

    //ATLASSERT(pnBytes != NULL);
    //ATLASSUME(hkey != NULL);

    lRes = ::RegQueryValueEx(hkey, pszValueName, NULL, &dwType, reinterpret_cast<LPBYTE>(pValue),
        pnBytes);
    if (lRes != ERROR_SUCCESS)
        return lRes;
    if (dwType != REG_BINARY)
        return ERROR_INVALID_DATA;

    return ERROR_SUCCESS;
}

#pragma warning(push)
#pragma warning(disable: 6053)
/* prefast noise VSW 496818 */
LONG RegKeyOp::QueryStringValue(LPCTSTR pszValueName, LPTSTR pszValue, ULONG* pnChars) throw()
{
    LONG lRes;
    DWORD dwType;
    ULONG nBytes;

    //ATLASSUME(hkey != NULL);
    //ATLASSERT(pnChars != NULL);

    nBytes = (*pnChars)*sizeof(TCHAR);
    *pnChars = 0;
    lRes = ::RegQueryValueEx(hkey, pszValueName, NULL, &dwType, reinterpret_cast<LPBYTE>(pszValue),
        &nBytes);

    if (lRes != ERROR_SUCCESS)
    {
        return lRes;
    }

    if(dwType != REG_SZ && dwType != REG_EXPAND_SZ)
    {
        return ERROR_INVALID_DATA;
    }

    if (pszValue != NULL)
    {
        if(nBytes!=0)
        {
#pragma warning(suppress:6385) // suppress noisy code analysis warning due to annotation on RegQueryValueEx
            if ((nBytes % sizeof(TCHAR) != 0) || (pszValue[nBytes / sizeof(TCHAR) -1] != 0))
            {
                return ERROR_INVALID_DATA;
            }
        }
        else
        {
            pszValue[0]=_T('\0');
        }
    }

    *pnChars = nBytes/sizeof(TCHAR);

    return ERROR_SUCCESS;
}
#pragma warning(pop)

#pragma warning(push)
#pragma warning(disable: 6053)

LONG RegKeyOp::QueryMultiStringValue(LPCTSTR pszValueName, LPTSTR pszValue, ULONG* pnChars) throw()
{
    LONG lRes;
    DWORD dwType;
    ULONG nBytes;

    //ATLASSUME(hkey != NULL);
    //ATLASSERT(pnChars != NULL);

    if (pszValue != NULL && *pnChars < 2)
        return ERROR_INSUFFICIENT_BUFFER;

    nBytes = (*pnChars)*sizeof(TCHAR);
    *pnChars = 0;

    lRes = ::RegQueryValueEx(hkey, pszValueName, NULL, &dwType, reinterpret_cast<LPBYTE>(pszValue),
        &nBytes);
    if (lRes != ERROR_SUCCESS)
        return lRes;
    if (dwType != REG_MULTI_SZ)
        return ERROR_INVALID_DATA;
    if (pszValue != NULL && (nBytes % sizeof(TCHAR) != 0 || nBytes / sizeof(TCHAR) < 1 || pszValue[nBytes / sizeof(TCHAR) -1] != 0 || ((nBytes/sizeof(TCHAR))>1 && pszValue[nBytes / sizeof(TCHAR) - 2] != 0)))
        return ERROR_INVALID_DATA;

    *pnChars = nBytes/sizeof(TCHAR);

    return ERROR_SUCCESS;
}

#pragma warning(pop)

LONG WINAPI RegKeyOp::SetValue(HKEY hKeyParent, LPCTSTR lpszKeyName, LPCTSTR lpszValue, LPCTSTR lpszValueName)
{
    //ATLASSERT(lpszValue != NULL);
    RegKeyOp key;
    LONG lRes = key.Create(hKeyParent, lpszKeyName);
    if (lRes == ERROR_SUCCESS)
        lRes = key.SetStringValue(lpszValueName, lpszValue);
    return lRes;
}

LONG RegKeyOp::SetKeyValue(LPCTSTR lpszKeyName, LPCTSTR lpszValue, LPCTSTR lpszValueName) throw()
{
    //ATLASSERT(lpszValue != NULL);
    RegKeyOp key;
    LONG lRes = key.Create(hkey, lpszKeyName, REG_NONE, REG_OPTION_NON_VOLATILE, KEY_READ | KEY_WRITE | samWOW64);
    if (lRes == ERROR_SUCCESS)
        lRes = key.SetStringValue(lpszValueName, lpszValue);
    return lRes;
}

#pragma warning(push)
#pragma warning(disable: 4996)
LONG RegKeyOp::SetValue(DWORD dwValue, LPCTSTR pszValueName)
{
    //ATLASSUME(hkey != NULL);
    return SetDWORDValue(pszValueName, dwValue);
}

LONG RegKeyOp::SetValue(LPCTSTR lpszValue, LPCTSTR lpszValueName, bool bMulti, int nValueLen)
{
    //ATLENSURE(lpszValue != NULL);
    //ATLASSUME(hkey != NULL);

    if (bMulti && nValueLen == -1)
        return ERROR_INVALID_PARAMETER;

    if (nValueLen == -1)
        nValueLen = lstrlen(lpszValue) + 1;

    DWORD dwType = bMulti ? REG_MULTI_SZ : REG_SZ;

    return ::RegSetValueEx(hkey, lpszValueName, NULL, dwType,
        reinterpret_cast<const BYTE*>(lpszValue), nValueLen*sizeof(TCHAR));
}
#pragma warning(pop)

LONG RegKeyOp::SetValue(LPCTSTR pszValueName, DWORD dwType, const void* pValue, ULONG nBytes) throw()
{
    //ATLASSUME(hkey != NULL);
    return ::RegSetValueEx(hkey, pszValueName, NULL, dwType, static_cast<const BYTE*>(pValue), nBytes);
}

LONG RegKeyOp::SetBinaryValue(LPCTSTR pszValueName, const void* pData, ULONG nBytes) throw()
{
    //ATLASSUME(hkey != NULL);
    return ::RegSetValueEx(hkey, pszValueName, NULL, REG_BINARY, reinterpret_cast<const BYTE*>(pData), nBytes);
}

LONG RegKeyOp::SetDWORDValue(LPCTSTR pszValueName, DWORD dwValue) throw()
{
    //ATLASSUME(hkey != NULL);
    return ::RegSetValueEx(hkey, pszValueName, NULL, REG_DWORD, reinterpret_cast<const BYTE*>(&dwValue), sizeof(DWORD));
}

LONG RegKeyOp::SetQWORDValue(LPCTSTR pszValueName, ULONGLONG qwValue) throw()
{
    //ATLASSUME(hkey != NULL);
    return ::RegSetValueEx(hkey, pszValueName, NULL, REG_QWORD, reinterpret_cast<const BYTE*>(&qwValue), sizeof(ULONGLONG));
}

LONG RegKeyOp::SetStringValue(LPCTSTR pszValueName, LPCTSTR pszValue, DWORD dwType) throw()
{
    //ATLASSUME(hkey != NULL);
    //ATLENSURE_RETURN_VAL(pszValue != NULL, ERROR_INVALID_DATA);
    //ATLASSERT((dwType == REG_SZ) || (dwType == REG_EXPAND_SZ));

    return ::RegSetValueEx(hkey, pszValueName, NULL, dwType, reinterpret_cast<const BYTE*>(pszValue), (lstrlen(pszValue)+1)*sizeof(TCHAR));
}

LONG RegKeyOp::SetMultiStringValue(LPCTSTR pszValueName, LPCTSTR pszValue) throw()
{
    LPCTSTR pszTemp;
    ULONG nBytes;
    ULONG nLength;

    //ATLASSUME(hkey != NULL);
    //ATLENSURE_RETURN_VAL(pszValue != NULL, ERROR_INVALID_DATA);

    // Find the total length (in bytes) of all of the strings, including the
    // terminating '\0' of each string, and the second '\0' that terminates
    // the list.
    nBytes = 0;
    pszTemp = pszValue;
    do
    {
        nLength = lstrlen(pszTemp)+1;
        pszTemp += nLength;
        nBytes += nLength*sizeof(TCHAR);
    } while (nLength != 1);

    return ::RegSetValueEx(hkey, pszValueName, NULL, REG_MULTI_SZ, reinterpret_cast<const BYTE*>(pszValue),
        nBytes);
}

LONG RegKeyOp::GetKeySecurity(SECURITY_INFORMATION si, PSECURITY_DESCRIPTOR psd, LPDWORD pnBytes) throw()
{
    //ATLASSUME(hkey != NULL);
    //ATLASSUME(pnBytes != NULL);

    return ::RegGetKeySecurity(hkey, si, psd, pnBytes);
}

LONG RegKeyOp::SetKeySecurity(SECURITY_INFORMATION si, PSECURITY_DESCRIPTOR psd) throw()
{
    //ATLASSUME(hkey != NULL);
    //ATLASSUME(psd != NULL);

    return ::RegSetKeySecurity(hkey, si, psd);
}

LONG RegKeyOp::RecurseDeleteKey(LPCTSTR lpszKey) throw()
{
    RegKeyOp key;
    LONG lRes = key.Open(hkey, lpszKey, KEY_READ | KEY_WRITE | samWOW64);
    if (lRes != ERROR_SUCCESS)
    {
        if (lRes != ERROR_FILE_NOT_FOUND && lRes != ERROR_PATH_NOT_FOUND)
        {
            //ATLTRACE(atlTraceCOM, 0, _T("UIRegKey::RecurseDeleteKey : Failed to Open Key %s(Error = %d)\n"), lpszKey, lRes);
        }
        return lRes;
    }
    FILETIME time;
    DWORD dwSize = 256;
    TCHAR szBuffer[256];
    while (RegEnumKeyEx(key.hkey, 0, szBuffer, &dwSize, NULL, NULL, NULL,
        &time)==ERROR_SUCCESS)
    {
        lRes = key.RecurseDeleteKey(szBuffer);
        if (lRes != ERROR_SUCCESS)
            return lRes;
        dwSize = 256;
    }
    key.Close();
    return DeleteSubKey(lpszKey);
}

}
