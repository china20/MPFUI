// 华勤科技版权所有 2008-2022
// 
// 文件名：uihwndobject.h
// 功  能：封装windows的句柄。
// 
// 作  者：MPF开发组
// 时  间：2010-07-02
// 
// ============================================================================

#ifndef _UIRENDERINFO_H_
#define _UIRENDERINFO_H_

#include <System/Types/Interface.h>
#include <System/Tools/Array.h>

#include <System/Graphics/Bitmap.h>
#include <System/Windows/RenderTarget.h>
#include <System/Windows/FrameworkElement.h>

#include <System/Input/HwndMouseFilter.h>
#include <System/Input/HwndKeyboardFilter.h>

#include <System/Tools/SSE.h>

namespace suic
{

class RenderInfo
{
public:

    RenderInfo();
    ~RenderInfo();

    bool OnFilterMessage(Element* rootElement, MessageParam* mp);
    bool OnHookFilterMessage(Element* rootElement, MessageParam* mp, bool& interrupt);

    Bitmap* GetDib();
 
    bool DrawDebugLine() const;
    void SetDrawDebugLine(bool val);

    bool IsNeedRender();
    void SetNeedRender(bool val);
    HwndObject* GetHwndObject();
    HWND GetHostHwnd();

    void InitHwndObject(HwndObject* obj);

    void RegisterHook(Object* obj, MessageHook hook);
    void RemmoveHook(Object* obj, MessageHook hook);

    void AddClip(Rect rect);
    void PopClip(Rect& rect);

    void SetSizeToContent(int val)
    {
        if (NULL != _hwndObj)
        {
            _hwndObj->SetSizeToContent(val);
        }
    }

    void AddArrangeElement(Element* elem)
    {
        _arrangs.Add(WeakRef(elem));
    }

public:

    HwndMouseFilter _mouseFilter;
    HwndKeyboardFilter _keyboardFilter;

    Bitmap _dib;
    HookInfo _hooks;

    bool _drawDebugLine;
    bool _needRender;
    bool _isRendering;

    StringAutoDic<Object> Context;
    Array<WeakRef, true> _arrangs;

protected:

    HwndObject* _hwndObj;
    Rect _rectClip;
    // InvManager _clipRect;
};

inline HwndObject* RenderInfo::GetHwndObject() 
{ 
    return _hwndObj; 
}

inline HWND RenderInfo::GetHostHwnd()
{
    if (NULL == _hwndObj)
    {
        return NULL;
    }
    else
    {
        return HANDLETOHWND(_hwndObj->GetHandle());
    }
}

inline Bitmap* RenderInfo::GetDib()
{
    return &_dib;
}

inline bool RenderInfo::DrawDebugLine() const
{
    return _drawDebugLine;
}

inline void RenderInfo::SetDrawDebugLine(bool val)
{
    _drawDebugLine = val;
}

inline bool RenderInfo::IsNeedRender()
{
    return _needRender;
}

inline void RenderInfo::SetNeedRender(bool val)
{
    _needRender = val;
}

inline void RenderInfo::AddClip(Rect rect)
{
    _rectClip.Union(rect);
}

inline void RenderInfo::PopClip(Rect& rect)
{
    rect = _rectClip;
    _rectClip.SetEmpty();
}

class SysRenderSource : public VisualHost
{
public:

    RTTIOfClass(SysRenderSource)

    SysRenderSource(HwndParam hp);
    SysRenderSource();
    virtual ~SysRenderSource();

    Handle GetHandle();

    bool Initialize(Element* rootElement);

    void SetHwnd(Handle hwnd);
    FrameworkElement* GetRootElement();

    void StopMouseTrack();
    void StartMouseTrack();

    void Close(bool bAsync);

    Point GetScreenPoint();

    HwndObject* GetHwndObject();
    Bitmap* GetDib();
};

inline HwndObject* SysRenderSource::GetHwndObject()
{
    RenderInfo* pInfo(GetRenderInfo());
    return pInfo->GetHwndObject();
}

inline Bitmap* SysRenderSource::GetDib()
{
    RenderInfo* pInfo(GetRenderInfo());
    return pInfo->GetDib();
}

}

#endif
