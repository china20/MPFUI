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

    // �ؼ����ü���
    long _refNum;
    // �ؼ���������
    suic::Rect _rect;
    // �ؼ�������UI�ؼ�
    suic::Element* _owner;

    // �Ƿ��Ѿ�����
    bool  _bInPlaced;
    // �Ƿ�֧���޴��ھ��ģʽ
    bool _bWindowless;

    // ʵ�ʵĿؼ��ӿ�ָ��
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
