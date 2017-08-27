//////////////////////////////////////////////
// WLTls

#include <util/ETTls.h>
#include <assert.h>

#define WL_DEBUG_ONLY(f)  (f)

/////////////////////////////////////////////////////////////////////////////
// ETThreadSlotData

inline void __declspec(noreturn) _ETRaiseException( DWORD dwExCode, DWORD dwExFlags = EXCEPTION_NONCONTINUABLE )
{
    ::RaiseException( dwExCode, dwExFlags, 0, NULL );
}

/////////////////////////////////////////////////////////////////////////////
// CSimpleList

void ETSimpleList::AddHead(void* p)
{
    assert(p != NULL);
    assert(*GetNextPtr(p) == NULL);

    *GetNextPtr(p) = m_pHead;
    m_pHead = p;
}

BOOL ETSimpleList::Remove(void* p)
{
    assert(p != NULL);

    if (m_pHead == NULL)
        return FALSE;

    BOOL bResult = FALSE;
    if (m_pHead == p)
    {
        m_pHead = *GetNextPtr(p);
        WL_DEBUG_ONLY(*GetNextPtr(p) = NULL);
        bResult = TRUE;
    }
    else
    {
        void* pTest = m_pHead;
        while (pTest != NULL && *GetNextPtr(pTest) != p)
            pTest = *GetNextPtr(pTest);
        if (pTest != NULL)
        {
            *GetNextPtr(pTest) = *GetNextPtr(p);
            WL_DEBUG_ONLY(*GetNextPtr(p) = NULL);
            bResult = TRUE;
        }
    }
    return bResult;
}

// global _etThreadData used to allocate thread local indexes
BYTE __wlThreadData[sizeof(ETThreadSlotData)];
ETThreadSlotData* _etThreadData;

struct WLThreadData : public ETNoTrackBase
{
    WLThreadData* pNext; // required to be member of CSimpleList
    int nCount;         // current size of pData
    LPVOID* pData;      // actual thread local data (indexed by nSlot)
};

struct WLSlotData
{
    DWORD dwFlags;      // slot flags (allocated/not allocated)
    HINSTANCE hInst;    // module which owns this slot
};

// flags used for WLSlotData::dwFlags above
#define WL_SLOT_USED   0x01    // slot is allocated

ETThreadSlotData::ETThreadSlotData()
{
    m_list.Construct(offsetof(WLThreadData, pNext));

    // initialize state and allocate TLS index
    m_nAlloc = 0;
    m_nRover = 1;   // first slot (0) is always reserved
    m_nMax = 0;
    m_pSlotData = NULL;

    // init m_tlsIndex to -1 if !bThreadLocal, otherwise TlsAlloc
    m_tlsIndex = TlsAlloc();
    if (m_tlsIndex == (DWORD)-1)
    {
        throw;
    }

    InitializeCriticalSection(&m_sect);
}

ETThreadSlotData::~ETThreadSlotData()
{
    WLThreadData* pData = m_list;
    while (pData != NULL)
    {
        WLThreadData* pDataNext = pData->pNext;
        DeleteValues(pData, NULL);
        pData = pDataNext;
    }

    if (m_tlsIndex != (DWORD)-1)
    {
        ::TlsFree(m_tlsIndex);
        WL_DEBUG_ONLY(m_tlsIndex = (DWORD)-1);
    }

    if (m_pSlotData != NULL)
    {
        HGLOBAL hSlotData = ::GlobalHandle(m_pSlotData);
        ::GlobalUnlock(hSlotData);
        ::GlobalFree(hSlotData);
        WL_DEBUG_ONLY(m_pSlotData = NULL);
    }

    DeleteCriticalSection(&m_sect);
}

int ETThreadSlotData::AllocSlot()
{
    EnterCriticalSection(&m_sect);
    int nAlloc = m_nAlloc;
    int nSlot = m_nRover;
    if (nSlot >= nAlloc || (m_pSlotData[nSlot].dwFlags & WL_SLOT_USED))
    {
        // search for first free slot, starting at beginning
        for (nSlot = 1;
            nSlot < nAlloc && (m_pSlotData[nSlot].dwFlags & WL_SLOT_USED); nSlot++)
            ;

        // if none found, need to allocate more space
        if (nSlot >= nAlloc)
        {
            // realloc memory for the bit array and the slot memory
            int nNewAlloc = m_nAlloc + 32;
            HGLOBAL hSlotData = 0;
            if (m_pSlotData == NULL)
            {
                hSlotData = GlobalAlloc(GMEM_MOVEABLE, nNewAlloc * sizeof(WLSlotData)
                    /*static_cast<UINT>(::ATL::AtlMultiplyThrow(static_cast<UINT>(nNewAlloc),
                    static_cast<UINT>(sizeof(WLSlotData))))*/);
            }
            else
            {
                hSlotData = GlobalHandle(m_pSlotData);
                GlobalUnlock(hSlotData);
                hSlotData = GlobalReAlloc(hSlotData, nNewAlloc * sizeof(WLSlotData)
                    /*static_cast<UINT>(::ATL::AtlMultiplyThrow(static_cast<UINT>(nNewAlloc),
                    static_cast<UINT>(sizeof(WLSlotData))))*/, GMEM_MOVEABLE|GMEM_SHARE);
            }

            if (hSlotData == NULL)
            {
                if (m_pSlotData != NULL)
                {
                    GlobalLock(GlobalHandle(m_pSlotData));
                }
                LeaveCriticalSection(&m_sect);
                //AfxThrowMemoryException();
            }
            WLSlotData* pSlotData = (WLSlotData*)GlobalLock(hSlotData);

            // always zero initialize after success
            memset(pSlotData+m_nAlloc, 0, (nNewAlloc-m_nAlloc)*sizeof(WLSlotData));
            m_nAlloc = nNewAlloc;
            m_pSlotData = pSlotData;
        }
    }
    assert(nSlot != 0); // first slot (0) is reserved

    // adjust m_nMax to largest slot ever allocated
    if (nSlot >= m_nMax)
    {
        m_nMax = nSlot+1;
    }

    assert(!(m_pSlotData[nSlot].dwFlags & WL_SLOT_USED));
    m_pSlotData[nSlot].dwFlags |= WL_SLOT_USED;
    // update m_nRover (likely place to find a free slot is next one)
    m_nRover = nSlot+1;

    LeaveCriticalSection(&m_sect);
    return nSlot;   // slot can be used for FreeSlot, GetValue, SetValue
}

void ETThreadSlotData::FreeSlot(int nSlot)
{
    EnterCriticalSection(&m_sect);
    assert(nSlot != 0 && nSlot < m_nMax);
    assert(m_pSlotData != NULL);
    assert(m_pSlotData[nSlot].dwFlags & WL_SLOT_USED);
    if( nSlot <= 0 || nSlot >= m_nMax ) // check for retail builds.
    {
        return;
    }

    // delete the data from all threads/processes
    WLThreadData* pData = m_list;
    while (pData != NULL)
    {
        if (nSlot < pData->nCount)
        {
            delete (ETNoTrackBase*)pData->pData[nSlot];
            pData->pData[nSlot] = NULL;
        }
        pData = pData->pNext;
    }
    // free the slot itself
    m_pSlotData[nSlot].dwFlags &= ~WL_SLOT_USED;
    LeaveCriticalSection(&m_sect);
}

// special version of ETThreadSlotData::GetData that only works with
// thread local storage (and not process local storage)
// this version is inlined and simplified for speed
inline void* ETThreadSlotData::GetThreadValue(int nSlot)
{
    EnterCriticalSection(&m_sect);
    assert(nSlot != 0 && nSlot < m_nMax);
    assert(m_pSlotData != NULL);
    assert(m_pSlotData[nSlot].dwFlags & WL_SLOT_USED);
    assert(m_tlsIndex != (DWORD)-1);
    if( nSlot <= 0 || nSlot >= m_nMax ) // check for retail builds.
    {
        LeaveCriticalSection(&m_sect);
        return NULL;
    }

    WLThreadData* pData = (WLThreadData*)TlsGetValue(m_tlsIndex);
    if (pData == NULL || nSlot >= pData->nCount)
    {
        LeaveCriticalSection(&m_sect);
        return NULL;
    }
    void * pRetVal = pData->pData[nSlot];
    LeaveCriticalSection(&m_sect);
    return pRetVal;
}

void ETThreadSlotData::SetValue(int nSlot, void* pValue)
{
    EnterCriticalSection(&m_sect);
    assert(nSlot != 0 && nSlot < m_nMax);
    assert(m_pSlotData != NULL);
    assert(m_pSlotData[nSlot].dwFlags & WL_SLOT_USED);
    if( nSlot <= 0 || nSlot >= m_nMax ) // check for retail builds.
    {
        LeaveCriticalSection(&m_sect);
        return;
    }

    WLThreadData* pData = (WLThreadData*)TlsGetValue(m_tlsIndex);
    if (pData == NULL || nSlot >= pData->nCount && pValue != NULL)
    {
        // if pData is NULL then this thread has not been visited yet
        if (pData == NULL)
        {
            //TRY
            try
            {
                pData = new WLThreadData;
            }
            //CATCH_ALL(e)
            catch (...)
            {
                LeaveCriticalSection(&m_sect);
                //THROW_LAST();
            }
            //END_CATCH_ALL
            pData->nCount = 0;
            pData->pData = NULL;
            WL_DEBUG_ONLY(pData->pNext = NULL);

            m_list.AddHead(pData);
        }

        // grow to now current size
        void** ppvTemp;
        if (pData->pData == NULL)
        {
            ppvTemp = (void**)LocalAlloc(LMEM_FIXED, m_nMax * sizeof(LPVOID)
                /*static_cast<UINT>(::ATL::AtlMultiplyThrow(static_cast<UINT>(m_nMax),
                static_cast<UINT>(sizeof(LPVOID))))*/);
        }
        else
        {
            ppvTemp = (void**)LocalReAlloc(pData->pData, m_nMax * sizeof(LPVOID)
                /*static_cast<UINT>(::ATL::AtlMultiplyThrow(static_cast<UINT>(m_nMax),
                static_cast<UINT>(sizeof(LPVOID))))*/, LMEM_MOVEABLE);
        }
        if (ppvTemp == NULL)
        {
            LeaveCriticalSection(&m_sect);
            //AfxThrowMemoryException();
        }
        pData->pData = ppvTemp;

        // initialize the newly allocated part
        memset(pData->pData + pData->nCount, 0,
            (m_nMax - pData->nCount) * sizeof(LPVOID));
        pData->nCount = m_nMax;
        TlsSetValue(m_tlsIndex, pData);
    }
    assert(pData->pData != NULL && nSlot < pData->nCount);
    if( pData->pData != NULL && nSlot < pData->nCount )
    {
        pData->pData[nSlot] = pValue;
    }
    LeaveCriticalSection(&m_sect);
}

void ETThreadSlotData::AssignInstance(HINSTANCE hInst)
{
    EnterCriticalSection(&m_sect);
    assert(m_pSlotData != NULL);
    assert(hInst != NULL);

    for (int i = 1; i < m_nMax; i++)
    {
        if (m_pSlotData[i].hInst == NULL && 
            (m_pSlotData[i].dwFlags & WL_SLOT_USED))
        {
            m_pSlotData[i].hInst = hInst;
        }
    }
    LeaveCriticalSection(&m_sect);
}

void ETThreadSlotData::DeleteValues(WLThreadData* pData, HINSTANCE hInst)
{
    assert(pData != NULL);

    // free each element in the table
    BOOL bDelete = TRUE;
    for (int i = 1; i < pData->nCount; i++)
    {
        if (hInst == NULL || m_pSlotData[i].hInst == hInst)
        {
            // delete the data since hInst matches (or is NULL)
            delete (ETNoTrackBase*)pData->pData[i];
            pData->pData[i] = NULL;
        }
        else if (pData->pData[i] != NULL)
        {
            // don't delete thread data if other modules still alive
            bDelete = FALSE;
        }
    }

    if (bDelete)
    {
        // remove from master list and free it
        EnterCriticalSection(&m_sect);
        m_list.Remove(pData);
        LeaveCriticalSection(&m_sect);
        LocalFree(pData->pData);
        delete pData;

        // clear TLS index to prevent from re-use
        TlsSetValue(m_tlsIndex, NULL);
    }
}

void ETThreadSlotData::DeleteValues(HINSTANCE hInst, BOOL bAll)
{
    EnterCriticalSection(&m_sect);
    if (!bAll)
    {
        // delete the values only for the current thread
        WLThreadData* pData = (WLThreadData*)TlsGetValue(m_tlsIndex);
        if (pData != NULL)
        {
            DeleteValues(pData, hInst);
        }
    }
    else
    {
        // delete the values for all threads
        WLThreadData* pData = m_list;
        while (pData != NULL)
        {
            WLThreadData* pDataNext = pData->pNext;
            DeleteValues(pData, hInst);
            pData = pDataNext;
        }
    }
    LeaveCriticalSection(&m_sect);
}

/////////////////////////////////////////////////////////////////////////////
// ETNoTrackBase

//#if defined(_DEBUG) && !defined(_AFX_NO_DEBUG_CRT)
//void* PASCAL ETNoTrackBase::operator new(size_t nSize, LPCSTR, int)
//{
//	return ETNoTrackBase::operator new(nSize);
//}
//
//#if _MSC_VER >= 1200
//void PASCAL ETNoTrackBase::operator delete(void* pObject, LPCSTR, int)
//{
//	if (pObject != NULL)
//	{
//		::LocalFree(pObject);
//	}
//}
//#endif
//#endif

void * PASCAL ETNoTrackBase::operator new(size_t nSize)
{
    void * p = ::LocalAlloc(LPTR, nSize);
    DWORD dwErr = ::GetLastError();
    if (p == NULL)
    {
        throw;
    }
    return p;
}

void PASCAL ETNoTrackBase::operator delete(void* p)
{
    if (p != NULL)
    {
        ::LocalFree(p);
    }
}

/////////////////////////////////////////////////////////////////////////////
// ETThreadLocalBase

ETNoTrackBase* ETThreadLocalBase::GetData(
    ETNoTrackBase* (__stdcall* pfnCreateObject)())
{
    //ENSURE(pfnCreateObject);
    if (m_nSlot == 0)
    {
        if (_etThreadData == NULL)
        {
            _etThreadData = new(__wlThreadData) ETThreadSlotData();
        }
        m_nSlot = _etThreadData->AllocSlot();
    }
    ETNoTrackBase* pValue = static_cast<ETNoTrackBase*>(_etThreadData->GetThreadValue(m_nSlot));
    if (pValue == NULL)
    {
        // allocate zero-init object
        pValue = (*pfnCreateObject)();

        // set tls data to newly created object
        _etThreadData->SetValue(m_nSlot, pValue);
        assert(_etThreadData->GetThreadValue(m_nSlot) == pValue);
    }
    return pValue;
}

ETNoTrackBase* ETThreadLocalBase::GetDataNA()
{
    if (m_nSlot == 0 || _etThreadData == NULL)
        return NULL;

    ETNoTrackBase* pValue =
        (ETNoTrackBase*)_etThreadData->GetThreadValue(m_nSlot);
    return pValue;
}

ETThreadLocalBase::~ETThreadLocalBase()
{
    if (m_nSlot != 0 && _etThreadData != NULL)
    {
        _etThreadData->FreeSlot(m_nSlot);
    }
    m_nSlot = 0;
}

void WINAPI ETTlsRelease()
{
    if (_etThreadData != NULL)
    {
        _etThreadData->~ETThreadSlotData();
        _etThreadData = NULL;
    }
}