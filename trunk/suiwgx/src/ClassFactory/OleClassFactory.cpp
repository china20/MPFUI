
#include "stdafx.h"
#include "OleClassFactory.h"

#include <Extend/Tools/RegKeyOp.h>
#include <Extend/Editor/RichTextOleCallback.h>

extern HMODULE suiwgx_handle;

EXTERN_C const GUID CLSID_UIOleObject = 
{ 0x63be71f1, 0x1a5a, 0xedcf, { 0xab, 0x2b, 0x0, 0xda, 0x1, 0xe6, 0x58, 0x92 } };

OleClassFactory::OleClassFactory()
{
    _refCount = 1;
}

OleClassFactory* OleClassFactory::Ins()
{
    static OleClassFactory ins;
    return &ins;
}

// IUnknown接口
HRESULT WINAPI OleClassFactory::QueryInterface(REFIID iid, void ** ppvObject)
{
    if (iid == IID_IUnknown || iid == IID_IClassFactory)
    {
        *ppvObject = (IClassFactory*)this;
        ((IClassFactory*)(*ppvObject))->AddRef();
        return S_OK;
    }
    else
    {
        return E_NOINTERFACE;
    }
}

ULONG WINAPI OleClassFactory::AddRef(void)
{
    ++_refCount;
    return _refCount;
}

ULONG WINAPI OleClassFactory::Release(void)
{
    --_refCount;
    return _refCount;
}

HRESULT STDMETHODCALLTYPE OleClassFactory::CreateInstance(IUnknown *pUnkOuter, REFIID riid, void **ppvObject)
{
    if (::IsEqualIID(riid, IID_IOleObject))
    {
        UIOleObject* lpCls = new UIOleObject(0);
        *ppvObject = lpCls;
        return S_OK;
    }
    return E_NOINTERFACE;
}

HRESULT STDMETHODCALLTYPE OleClassFactory::LockServer(BOOL fLock)
{
    return S_OK;
}

//----------------------------------------------------------------

STDAPI DllCanUnloadNow(void)
{
    return S_OK;
}

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
    if (::IsEqualIID(riid, IID_IClassFactory))
    {
        OleClassFactory::Ins()->QueryInterface(riid, ppv);
        return S_OK;
    }
    else
    {
        return S_FALSE;
    }
}

LONG OpenSubKey(suic::RegKeyOp& keyCur, HKEY hkParent, suic::Char* name)
{
    LONG lRes = 0;

    // 注册Prog.ID
    lRes = keyCur.Open(hkParent, name, KEY_READ | KEY_WRITE);

    if (ERROR_SUCCESS != lRes)
    {
        lRes = keyCur.Open(hkParent, name, KEY_READ);

        if (ERROR_SUCCESS != lRes)
        {
            lRes = keyCur.Create(hkParent, name, REG_NONE, REG_OPTION_NON_VOLATILE, KEY_READ | KEY_WRITE);
            if (lRes != ERROR_SUCCESS)
            {
                return -1;
            }
        }
    }

    return lRes;
}

void SetRegValue(HKEY hkParent, suic::Char* node, suic::Char* name, suic::Char* val)
{
    suic::RegKeyOp curKey;
    LONG lRes = OpenSubKey(curKey, hkParent, node);

    if (ERROR_SUCCESS != lRes)
    {
        return ;
    }
    
    lRes = curKey.SetStringValue(name, val);
}

void RegUIClassLibary(REFCLSID clsid, bool bRegister)
{
    LPOLESTR szClsid = {0};

    ::StringFromCLSID(clsid, &szClsid);

    if (szClsid != NULL)
    {
        LONG lRes = 0;
        // 
        // 打开根注册表
        //
        suic::RegKeyOp root;
        suic::RegKeyOp clsReg;
        suic::Char dllPath[256];

        lRes = root.Open(HKEY_CLASSES_ROOT, _U("CLSID"), KEY_READ | KEY_WRITE);

        if (ERROR_SUCCESS != lRes)
        {
            ::CoTaskMemFree(szClsid);
            return ;
        }

        if (!bRegister)
        {
            clsReg.RecurseDeleteKey(szClsid);
            ::CoTaskMemFree(szClsid);
            return ;
        }

        lRes = OpenSubKey(clsReg, root.hkey, szClsid);
        ::CoTaskMemFree(szClsid);

        if (ERROR_SUCCESS != lRes)
        {
            return ;
        }

        ::GetModuleFileName(suiwgx_handle, dllPath, 256);

        SetRegValue(clsReg.hkey, _U("InprocServer32"), _U("ThreadingModel"), _U("Apartment"));
        SetRegValue(clsReg.hkey, _U("InprocServer32"), NULL, dllPath);

        SetRegValue(clsReg.hkey, _U("ProgID"), NULL, _U("MPF.OleImage.1"));
    }
}

STDAPI DllRegisterServer(void)
{
    HRESULT hr = S_FALSE;
       
    RegUIClassLibary(CLSID_UIOleObject, true);

    return hr;
}

STDAPI DllUnregisterServer(void)
{
    HRESULT hr = S_FALSE;
    
    RegUIClassLibary(CLSID_UIOleObject, false);

    return hr;
}
	
STDAPI DllInstall(BOOL bInstall, LPCWSTR pszCmdLine)
{
    HRESULT hr = E_FAIL;

    if (bInstall)
    {	
        hr = DllRegisterServer();
        if (FAILED(hr))
        {	
            DllUnregisterServer();
        }
    }
    else
    {
        hr = DllUnregisterServer();
    }

    return hr;
}
