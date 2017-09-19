// 华勤科技版权所有 2008-2022
// 
// 文件名：uihwndobject.h
// 功  能：封装windows的句柄。
// 
// 作  者：MPF开发组
// 时  间：2010-07-02
// 
// ============================================================================

#ifndef _UIHWNDOBJECT_H_
#define _UIHWNDOBJECT_H_

#include <System/Windows/Object.h>
#include <System/Types/Interface.h>

#include <System/Windows/RenderTarget.h>
#include <System/Windows/FrameworkElement.h>

#include <System/Tools/SSE.h>
#include <System/Tools/Array.h>
#include <System/Graphics/Bitmap.h>

#include <System/Windows/InvManager.h>
#include <System/Windows/AssignerTimer.h>

#define WM_INVALIDATERENDER 888

namespace suic
{

class HwndObject;
class VisualHost;
class RenderInfo;

class HwndObject : public Object
{
public:

    HwndObject();
    HwndObject(HwndParam hp);
    ~HwndObject();

    static HwndObject* GetHwndObject(Element* elem);
    static RenderInfo* GetRenderInfo(Element* elem);
    static void HandleWindowRgn(Handle hd, Element* fe, int w, int h);

    Element* GetRootElement();
    void SetRootElement(Element* element);

    bool Invalidate(const Rect* lprc, bool bForce);

    void StartMouseTrack();
    void StopMouseTrack();

    bool NeedCreateBmp();
    void SetNeedRender(bool value);

    void RefleshUIShow(Rect* lpRect, bool bForce);
    void DisableSizeToContent();
    bool EnableSizeToContent();

    Size GetSizeFromHwnd();

    Bitmap* GetDrawingCanvas();

    void Close(bool bAsync);
    bool IsUsable();

    void Resize(Size newSize);

    int GetSizeToContent() const;
    void SetSizeToContent(int val);

    bool IsOwnUpdating() const
    {
        return _myOwnUpdate;
    }

    bool IsWindowInMinimizeState()
    {
        return _isWindowInMinimizeState;
    }

    void SetWindowInMinimizeState(bool val)
    {
        _isWindowInMinimizeState = val;
    }

    HwndAdapter* GetHwndAdapter();
    Handle GetHandle();
    void Clear();

    void CallDefWindowProc(MessageParam* mp);
    bool OnRenderFilterMessage(Element* rootElement, MessageParam* mp);
    bool OnPublicFilterMessage(Element* rootElement, MessageParam* mp);

protected:

    void SetLayoutSize();
    void OnLayoutUpdated(Object* sender, EventArg* e);

    void Process_WM_DESTROY(Element* rootElement, MessageParam* mp);
    void Process_WM_NCDESTROY(Element* rootElement, MessageParam* mp);
    bool Process_WM_ACTIVATEAPP(Element* rootElement, MessageParam* mp);
    bool Process_WM_TIMER(Element* rootElement, MessageParam* mp);
    bool Process_WM_ACTIVATE(Element* rootElement, MessageParam* mp);
    bool Process_WM_NCACTIVATE(Element* rootElement, MessageParam* mp);
    bool Process_WM_SYSCOMMAND(Element* rootElement, MessageParam* mp);

    bool Process_WM_CREATE(Element* rootElement, MessageParam* mp);
    bool Process_WM_SIZE(Element* rootElement, MessageParam* mp);
    bool Process_WM_WINDOWPOSCHANGING(Element* rootElement, MessageParam* mp);
    bool Process_WM_GETMINMAXINFO(Element* rootElement, MessageParam* mp);

    void InitializeElements(Element* element);

protected:

    int _immtimerid;
    HwndAdapter* _hwnd;
    Element* _rootElement;

    Point _maxSize;
    Size _previousSize;
    int _sizeToContent;

    bool _myOwnUpdate;
    bool _isActivate;
    bool _mouseTracking;
    bool _isWindowInMinimizeState;

    bool _needRender;
    bool _isInSizeMode;

    friend class HwndMouseFilter;
};

inline Element* HwndObject::GetRootElement()
{
    return _rootElement;
}

inline HwndAdapter* HwndObject::GetHwndAdapter()
{
    return _hwnd;
}

inline bool HwndObject::NeedCreateBmp()
{
    return _needRender;
}

inline void HwndObject::SetNeedRender(bool value)
{
    _needRender = value;
}

}

#endif
