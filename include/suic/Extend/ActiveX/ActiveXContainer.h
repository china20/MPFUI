// ���ڿƼ���Ȩ���� 2010-2020
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2010-08-31
//
// SHARPUI�����������ҵ��������ϵ�ܹ���������ϵͳ���¼�ϵͳ��
// ��ģ����Ŀǰc++�������һ���ص㣬����������ʽ�ʹ�����룬�ײ�
// ��������SSE2ָ����м��٣���������ͨ���ڻ��ǰ�͸�����������ܶ�
// �ǳ����㣬���ơ����Ķ����ӿڣ���ȫ�������������ҵʹ�á�
// 
// ����蹺����֧�ַ������ѯ����ϵ
// china0851@foxmail.com��QQ:386666951��
// ====================================================================

#ifndef _UIACTIVEXCONTAINER_H_
#define _UIACTIVEXCONTAINER_H_

#include <System/Windows/Element.h>

#include <olectl.h>
#include <atlbase.h>
#include <atlwin.h>

class EnumUnknown : public IEnumUnknown
{
public:

    EnumUnknown(IUnknown* val, int lc=0) 
    {
        arr = val; 
        refNum = 1;
        loc = lc;
    }

    STDMETHOD_(ULONG,AddRef)()
    {
        return ++refNum;
    }

    STDMETHOD_(ULONG,Release)()
    {
        LONG lSt = --refNum;

        if (lSt == 0) 
        {
            delete this;
        }

        return lSt;
    }

    STDMETHOD(QueryInterface)(REFIID riid, LPVOID *ppvObject)
    {
        *ppvObject = NULL;
        if (riid == IID_IUnknown) 
        {
            *ppvObject = static_cast<IEnumUnknown*>(this);
        }
        else if (riid == IID_IEnumUnknown) 
        {
            *ppvObject = static_cast<IEnumUnknown*>(this);
        }
        if (*ppvObject != NULL) 
        {
            AddRef();
        }

        return *ppvObject == NULL ? E_NOINTERFACE : S_OK;
    }

    STDMETHOD(Next)(ULONG celt, IUnknown** rgelt, ULONG* pceltFetched) 
    { 
        if (pceltFetched != NULL)
        {
            *pceltFetched = 0;
        }

        if (celt < 0)
        {
            return E_INVALIDARG; 
        }

        if (++loc > 1) 
        {
            return S_FALSE;
        }

        *rgelt = arr;
        (*rgelt)->AddRef();

        if (pceltFetched != NULL) 
        {
            *pceltFetched = 1;
        }

        return S_OK; 
    } 

    STDMETHOD(Skip)(ULONG celt)
    { 
        loc += celt;

        if (loc >= 1)
        {
            return (S_FALSE); 
        }
        else
        {
            return S_OK; 
        }
    } 

    STDMETHOD(Reset)() 
    {
        loc = 0;
        return S_OK; 
    }

    STDMETHOD(Clone)(IEnumUnknown** ppenum)
    { 
        *ppenum = new EnumUnknown(arr, loc);
        return S_OK; 
    }

private:

    IUnknown* arr;
    int refNum; 
    int loc;
};

class ActiveXContainer :
    public IOleInPlaceFrame,
    public IOleContainer
{
public:

    ActiveXContainer(IOleObject* oleObj);
    ~ActiveXContainer(void);

public:

    // IUnknown methods
    STDMETHOD(QueryInterface)(REFIID iid,void**ppvObject);
    STDMETHOD_(ULONG,AddRef)();
    STDMETHOD_(ULONG,Release)();

    // IOleInPlaceFrame methods
    STDMETHOD(GetWindow)(HWND *p);
    STDMETHOD(ContextSensitiveHelp)(BOOL);
    STDMETHOD(GetBorder)(LPRECT l);
    STDMETHOD(RequestBorderSpace)(LPCBORDERWIDTHS);
    STDMETHOD(SetBorderSpace)(LPCBORDERWIDTHS w);
    STDMETHOD(SetActiveObject)(IOleInPlaceActiveObject*pV,LPCOLESTR s);
    STDMETHOD(InsertMenus)(HMENU h,LPOLEMENUGROUPWIDTHS x);
    STDMETHOD(SetMenu)(HMENU h,HOLEMENU hO,HWND hw);
    STDMETHOD(RemoveMenus)(HMENU h);
    STDMETHOD(SetStatusText)(LPCOLESTR t);
    STDMETHOD(EnableModeless)(BOOL f);
    STDMETHOD(TranslateAccelerator)(LPMSG,WORD);

    // IOleContainer
    STDMETHOD(EnumObjects)(DWORD grfFlags, IEnumUnknown** ppenum);
    STDMETHOD(LockContainer)(BOOL fLock);

    // IParseDisplayName
    STDMETHOD(ParseDisplayName)(IBindCtx* pbc, LPOLESTR pszDisplayName, ULONG* pchEaten, IMoniker** ppmkOut);

public:

    void SetHandle(suic::Element* val)
    {
        _owner = val;
    }

    suic::Handle GetHandle() const;

    void SetOleObject(IOleObject* pOleObj)
    {
        _oleObj = pOleObj;
    }

protected:

    long _refNum;
    suic::Element* _owner;

    IOleObject* _oleObj;
    IOleInPlaceActiveObject* _inPlaceActiveObj;
};

#endif
