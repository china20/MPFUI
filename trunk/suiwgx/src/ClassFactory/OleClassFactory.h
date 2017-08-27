// 华勤科技版权所有 2008-2022
// 
// 文件名：OleClassFactory.h
// 功  能：实现OleClassFactory接口
// 
// 作  者：MPF开发组
// 时  间：2010-08-31
// 
// ============================================================================

#ifndef _OLECLASSFACTORY_H_
#define _OLECLASSFACTORY_H_

class OleClassFactory : public IClassFactory
{
public:

    static OleClassFactory* Ins();

    HRESULT WINAPI QueryInterface(REFIID iid, void ** ppvObject);
    ULONG WINAPI AddRef(void);
    ULONG WINAPI Release(void);

    HRESULT STDMETHODCALLTYPE CreateInstance(IUnknown *pUnkOuter, REFIID riid, void **ppvObject);
    HRESULT STDMETHODCALLTYPE LockServer(BOOL fLock);

private:

    OleClassFactory();

    LONG _refCount;
};

#endif
