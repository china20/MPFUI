/////////////////////////////////////////////////////
// ShellHelper.cpp

#include "stdafx.h"
#include "ShellHelper.h"

ULONG ShellHelper::GetAttributes(LPSHELLFOLDER lpsf, LPITEMIDLIST lpi, ULONG uFlags)
{
	if (!lpsf || !lpi)
    {
        return 0;
    }

	if ((uFlags & SFGAO_DISPLAYATTRMASK) == 0)
	{
		lpsf->GetAttributesOf(1, (LPCITEMIDLIST*)&lpi, &uFlags);
		return uFlags;
	}

	ULONG ulAttrs = SFGAO_REMOVABLE;
	lpsf->GetAttributesOf(1, (LPCITEMIDLIST*)&lpi, &ulAttrs);

	if ((ulAttrs & SFGAO_REMOVABLE) != 0)
    {
        return SFGAO_REMOVABLE | SFGAO_HASSUBFOLDER | SFGAO_FOLDER;
    }

	lpsf->GetAttributesOf(1, (LPCITEMIDLIST*)&lpi, &uFlags);

	return uFlags;
}

void ShellHelper::MEMCPY_S(void *dest, const void* src, size_t count)
{
#if (_MSC_VER > 1310) // VS2005
    memcpy_s(dest, count, src, count);
#else
    memcpy(dest, src, count);
#endif
}

LPITEMIDLIST ShellHelper::GetNextPidlItem(LPCITEMIDLIST pidl)
{
	LPSTR lpMem = (LPSTR)pidl;
	lpMem += pidl->mkid.cb;
	return (LPITEMIDLIST)lpMem;
}

UINT ShellHelper::GetPidlCount(LPCITEMIDLIST pidl)
{
	UINT nCount = 0;

	if (NULL != pidl)
	{
		while (NULL != pidl->mkid.cb)
		{
			++nCount;
			pidl = GetNextPidlItem(pidl);
		}
	}

	return nCount;
}

UINT ShellHelper::GetPidlItemCount(LPCITEMIDLIST pidl)
{
	UINT cbTotal = 0;
	if (NULL != pidl)
	{
        // 
        // ÒÔNULL½áÎ²
        // 
		cbTotal += sizeof(pidl->mkid.cb);
		while (pidl->mkid.cb)
		{
			cbTotal += pidl->mkid.cb;
			pidl = GetNextPidlItem(pidl);
		}
	}

	return cbTotal;
}

LPITEMIDLIST ShellHelper::CreatePidl(LPMALLOC lpMalloc, UINT cbSize)
{
	if (!lpMalloc)
    {
        return NULL;
    }

	// 
    // zero-init for external task allocate
    // 
	LPITEMIDLIST pidl = (LPITEMIDLIST)lpMalloc->Alloc(cbSize);
	if (NULL != pidl)
	{
		memset(pidl, 0, cbSize);
	}

	return pidl;
}

void ShellHelper::FreePidl(LPITEMIDLIST pidl)
{
	if (NULL != pidl)
    {
        ShellMalloc lpMalloc;

        if (lpMalloc)
        {
            lpMalloc.Free(pidl);
        }
    }
}

BOOL ShellHelper::ComparePidls(LPCITEMIDLIST pidl1, LPCITEMIDLIST pidl2, LPSHELLFOLDER pShellFolder /*NULL*/)
{
	if (pidl1 == NULL || pidl2 == NULL)
    {
        return FALSE;
    }

	BOOL bLPCreated = FALSE;
	if (pShellFolder == NULL)
	{
		if (FAILED(::SHGetDesktopFolder(&pShellFolder)))
        {
            return FALSE;
        }
		bLPCreated = TRUE;
	}

	HRESULT hr = pShellFolder->CompareIDs(0, pidl1, pidl2);

	if (bLPCreated)
	{
		pShellFolder->Release();
	}

	return ((short)hr) == 0;
}

LPITEMIDLIST ShellHelper::ConcatPidls(LPCITEMIDLIST pidl1, LPCITEMIDLIST pidl2)
{
	return ConcatPidls(ShellMalloc(), pidl1, pidl2);
}

LPITEMIDLIST ShellHelper::ConcatPidls(LPMALLOC lpMalloc, LPCITEMIDLIST pidl1, LPCITEMIDLIST pidl2)
{
	LPITEMIDLIST pidlNew;
	UINT cb1;
	UINT cb2;

	// 
    // May be NULL
    // 
	if (NULL != pidl1)
	{
		cb1 = GetPidlItemCount(pidl1) - sizeof(pidl1->mkid.cb);
	}
	else
	{
		cb1 = 0;
	}

	cb2 = GetPidlItemCount(pidl2);
	pidlNew = CreatePidl(lpMalloc, cb1 + cb2);

	if (pidlNew)
	{
		if (pidl1)
		{
			MEMCPY_S(pidlNew, pidl1, cb1);
		}

		MEMCPY_S(((LPSTR)pidlNew) + cb1, pidl2, cb2);
	}

	return pidlNew;
}

bool ShellHelper::GetName(LPSHELLFOLDER lpsf, LPITEMIDLIST lpi, DWORD dwFlags, suic::String& strFriendlyName)
{
	STRRET str;

	if (lpsf->GetDisplayNameOf(lpi, dwFlags, &str) == NOERROR)
	{
		switch (str.uType)
		{
		case STRRET_WSTR:
			{
				strFriendlyName = str.pOleStr;

				ShellMalloc pMalloc;
				if (pMalloc) 
                {
                    pMalloc.Free(str.pOleStr);
                }
			}
			break;

		case STRRET_OFFSET:
			{
				LPSTR lpszOffset = (LPSTR)lpi + str.uOffset;
				strFriendlyName = lpszOffset;
			}
			break;

		case STRRET_CSTR:
			strFriendlyName = (LPSTR)str.cStr;
			break;

		default:
			return FALSE;
		}
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}

