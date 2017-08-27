// 华勤科技版权所有 2010-2020
// 
// 作  者：MPF开发组
// 时  间：2010-08-31
//
// MPF界面引擎采用业界成熟的体系架构，其属性系统、事件系统以
// 及模版在目前c++界面库是一大特点，真正做到样式和代码分离，底层
// 更采用了SSE2指令进行加速，不论在普通窗口还是半透明窗口下性能都
// 非常优秀，完善、灵活的动画接口，完全可以满足各种商业使用。
// 
// 如果需购买技术支持服务或咨询请联系
// china0851@foxmail.com；QQ:386666951。
// ====================================================================

#ifndef _UIACTIVEXSITE_H_
#define _UIACTIVEXSITE_H_

#include <System/Windows/Element.h>

class IActiveXLisnter
{
public:

    virtual ~IActiveXLisnter() {}

    virtual suic::Rect GetRect() = 0;
    virtual void OnRender() = 0;
    virtual void OnSetFocus(bool bFocus) = 0;
    virtual bool OnGetFocus() = 0;

    virtual void OnSetCapture(bool val) = 0;
    virtual bool OnGetCapture() = 0;
};

class ActiveXSite :
    public IOleClientSite,
    public IOleInPlaceSiteWindowless,
    public IOleControlSite,
    public IObjectWithSite
{
public:

    ActiveXSite();
    ~ActiveXSite(void);

public:

    // IUnknown methods
    STDMETHOD(QueryInterface)(REFIID iid,void**ppvObject);
    STDMETHOD_(ULONG,AddRef)();
    STDMETHOD_(ULONG,Release)();

    // IObjectWithSite
    STDMETHOD(SetSite)(IUnknown *pUnkSite);
    STDMETHOD(GetSite)(REFIID riid, LPVOID* ppvSite);

    // IOleClientSite methods
    STDMETHOD(SaveObject)();
    STDMETHOD(GetMoniker)(DWORD dwA, DWORD dwW, IMoniker** pm);
    STDMETHOD(GetContainer)(IOleContainer**pc);
    STDMETHOD(ShowObject)();
    STDMETHOD(OnShowWindow)(BOOL f);
    STDMETHOD(RequestNewObjectLayout)();

    STDMETHOD(GetWindow)(HWND *p);
    STDMETHOD(ContextSensitiveHelp)(BOOL);

    // IOleInPlaceSite methods
    STDMETHOD(CanInPlaceActivate)();
    STDMETHOD(OnInPlaceActivate)();
    STDMETHOD(OnUIActivate)();
    STDMETHOD(GetWindowContext)(IOleInPlaceFrame** ppFrame,IOleInPlaceUIWindow **ppDoc,LPRECT r1,LPRECT r2,LPOLEINPLACEFRAMEINFO o);
    STDMETHOD(Scroll)(SIZE s);
    STDMETHOD(OnUIDeactivate)(int);
    STDMETHOD(OnInPlaceDeactivate)();
    STDMETHOD(DiscardUndoState)();
    STDMETHOD(DeactivateAndUndo)();
    STDMETHOD(OnPosRectChange)(LPCRECT);

    // IOleInPlaceSiteWindowless
    STDMETHOD(CanWindowlessActivate)(void);
    STDMETHOD(GetCapture)(void);
    STDMETHOD(SetCapture)(BOOL fCapture);
    STDMETHOD(GetFocus)(void);
    STDMETHOD(SetFocus)(BOOL fFocus);
    STDMETHOD(GetDC)(LPCRECT pRect, DWORD grfFlags, HDC* phDC);
    STDMETHOD(ReleaseDC)(HDC hDC);
    STDMETHOD(InvalidateRect)(LPCRECT pRect, BOOL fErase);
    STDMETHOD(InvalidateRgn)(HRGN hRGN, BOOL fErase);
    STDMETHOD(ScrollRect)(INT dx, INT dy, LPCRECT pRectScroll, LPCRECT pRectClip);
    STDMETHOD(AdjustRect)(LPRECT prc);
    STDMETHOD(OnDefWindowMessage)(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT* plResult);

    // IOleControlSite
    STDMETHOD(OnControlInfoChanged)(void);      
    STDMETHOD(LockInPlaceActive)(BOOL fLock);       
    STDMETHOD(GetExtendedControl)(IDispatch** ppDisp);        
    STDMETHOD(TransformCoords)(POINTL* pPtlHimetric, POINTF* pPtfContainer, DWORD dwFlags);       
    STDMETHOD(TranslateAccelerator)(MSG* pMsg, DWORD grfModifiers);
    STDMETHOD(OnFocus)(BOOL fGotFocus);
    STDMETHOD(ShowPropertyFrame)(void);

    // IOleInPlaceSiteEx
    STDMETHOD(OnInPlaceActivateEx)(BOOL *pfNoRedraw, DWORD dwFlags);        
    STDMETHOD(OnInPlaceDeactivateEx)(BOOL fNoRedraw);       
    STDMETHOD(RequestUIActivate)(void);

public:

    virtual void OnRender(HDC hdc);

public:

    void Clear();

    void SetOwner(suic::Element* val);
    suic::Handle GetHandle() const;

    void SetContainer(IOleContainer* pc);
    bool DoVerb(int verb);

    BOOL SetControlRect(LPRECT lprc);
    void InitWrapper(IOleObject* pOleObj);

    bool IsWindowless() const;

    IViewObject* GetViewObject();
    IOleObject* GetOleObject();
    IOleInPlaceObjectWindowless* GetInPlaceObject();

protected:

    // 控件引用计数
    long _refNum;
    // 控件所在区域
    suic::Rect _rect;
    // 控件所属的UI控件
    suic::Element* _owner;

    // 是否已经激活
    bool  _bInPlaced;
    // 是否支持无窗口句柄模式
    bool _bWindowless;

    // 实际的控件接口指针
    IOleObject* _oleObj;
    IUnknown* _oleSite;
    IViewObject* _viewObject;
    IOleContainer* _container;
    IOleInPlaceObjectWindowless* _inPlaceObj;
};

inline void ActiveXSite::SetContainer(IOleContainer* pc)
{
    _container = pc;
}

inline bool ActiveXSite::IsWindowless() const
{
    return _bWindowless;
}

inline IViewObject* ActiveXSite::GetViewObject()
{ 
    return _viewObject; 
}

inline IOleObject* ActiveXSite::GetOleObject()
{
    return _oleObj;
}

inline IOleInPlaceObjectWindowless* ActiveXSite::GetInPlaceObject()
{ 
    return _inPlaceObj; 
}

#endif
