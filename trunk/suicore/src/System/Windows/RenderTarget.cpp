/////////////////////////////////////////////////////////////////////////
// uirendertarget.cpp

#include <System/Windows/RenderTarget.h>
#include <System/Windows/CoreTool.h>
#include <System/Windows/RenderInfo.h>

#include <System/Render/RenderManager.h>
#include <System/Tools/Thread.h>
#include <System/Tools/VisualTreeOp.h>
#include <System/Windows/HwndSubclass.h>

#include <Framework/Controls/Menu.h>
#include <Framework/Controls/Popup.h>
#include <Framework/Controls/Window.h>

namespace suic
{

ImplementRTTIOfClass(VisualHost, Object)

VisualHost::VisualHost()
    : _handle(0)
    , _rootElement(NULL)
{
    _renderInfo = new RenderInfo();
}

VisualHost::~VisualHost()
{
    if (_rootElement)
    {
        _rootElement->unref();
    }
    delete _renderInfo;
}

VisualHost* VisualHost::GetVisualHost(Element* elem)
{
    Element* root = VisualTreeOp::GetVisualRoot(elem);

    if (NULL == root)
    {
        return NULL;
    }
    else
    {
        return RTTICast<VisualHost>(root->GetValue(RootSourceProperty));
    }
}

FrameworkElement* VisualHost::FromHwnd(Handle h)
{
    VisualHost* visualHost = suic::HwndSubclass::FindRenderTarget(HANDLETOHWND(h));
    if (NULL != visualHost)
    {
        return visualHost->GetRootElement();
    }
    else
    {
        return NULL;
    }
}

void VisualHost::SetNeedRender(bool val)
{
    RenderInfo* pInfo(GetRenderInfo());
    pInfo->SetNeedRender(val);
}

bool VisualHost::IsNeedRender()
{
    RenderInfo* pInfo(GetRenderInfo());
    return pInfo->IsNeedRender();
}

void VisualHost::SetValue(const String& key, Object* obj)
{
    RenderInfo* pInfo(GetRenderInfo());
    pInfo->Context.Add(key, obj);
}

Object* VisualHost::GetValue(const String& key)
{
    Object* obj = NULL;
    RenderInfo* pInfo(GetRenderInfo());
    pInfo->Context.TryGetValue(key, obj);
    
    return obj;
}

Handle VisualHost::GetHandle()
{
    return _handle;
}

void VisualHost::SetHandle(Handle h)
{
    _handle = h;
}

void VisualHost::SetRootElement(FrameworkElement* host)
{
    Window* pWindow = NULL;

    if (_rootElement)
    {
        _rootElement->SetValue(RootSourceProperty, NULL);
    }

    _rootElement = host;
    _rootElement->ref();
    _rootElement->SetValue(RootSourceProperty, this);
    pWindow = RTTICast<Window>(host);
    if (NULL != pWindow)
    {
        GetRenderInfo()->SetSizeToContent(pWindow->GetSizeToContent());
    }
}

FrameworkElement* VisualHost::GetRootElement()
{
    return _rootElement;
}

Point VisualHost::GetScreenPoint()
{
    Rect rect;
    HWND hwnd = HANDLETOHWND(GetHandle());

    GetClientRect(hwnd, &rect);

    ClientToScreen(hwnd, (LPPOINT)&rect);
    ClientToScreen(hwnd, (LPPOINT)&rect + 1);

    return Point(rect.left, rect.top);
}

void VisualHost::SetSize(Size size)
{
    _size = size;

    if (_rootElement)
    {
        if (_rootElement->GetWidth() != _size.cx || _rootElement->GetHeight() != _size.cy)
        {
            _rootElement->SetWidth(_size.cx);
            _rootElement->SetHeight(_size.cy);
            _rootElement->Measure(_size);

            Rect finalRect(0, 0, _size.cx, _size.cy);

            _rootElement->Arrange(finalRect);
            SetNeedRender(true);
        }
    }
}

Size VisualHost::GetSize()
{
    return _size;
}

void VisualHost::UpdateLayout()
{
}

RenderInfo* VisualHost::GetRenderInfo()
{
    return _renderInfo;
}

Menu* VisualHost::GetMainMenu()
{
    return suic::DynamicCast<Menu>(_mainMenu.GetTarget());
}

void VisualHost::Invalidate(const Rect* lprc, bool force)
{
    if (_rootElement)
    {
        Rect rect;
        RenderInfo* pInfo(GetRenderInfo());

        if (lprc)
        {
            rect = *lprc;
        }
        else
        {
            rect.SetXYWH(0, 0, _rootElement->GetActualWidth(), _rootElement->GetActualHeight());
        }

        pInfo->AddClip(&rect);

        if (force)
        {
            OnRender(&rect);
        }
        else
        {
            HWND hwnd = HANDLETOHWND(GetHandle());
            if (::IsWindow(hwnd))
            {
                ::InvalidateRect(hwnd, rect, FALSE);
            }
        }
    }
}

void VisualHost::SetMouseCapture()
{
    HWND hwnd = HANDLETOHWND(GetHandle());
    if (::IsWindow(hwnd))
    {
        ::SetCapture(hwnd);
    }
}

void VisualHost::ReleaseMouseCapture()
{
    if (GetCapture() == HANDLETOHWND(GetHandle()))
    {
        ::ReleaseCapture();
    }
}

bool VisualHost::HandleEvent(MessageParam* mp)
{
    if (!_rootElement)
    {
        return false;
    }

    HWND hwnd = NULL;
    RenderInfo* pInfo(GetRenderInfo());
    FrameworkElement* rootElem = GetRootElement();

    switch (mp->message)
    {
    case WM_NCPAINT:
        return true;

    case WM_ERASEBKGND:
        mp->result = 1;
        return true;

    case 0x00AE:
    case 0x00AF:
    case WM_PRINTCLIENT:
    case WM_PRINT:
    {
        return true;
    }

    case WM_PAINT:
        hwnd = HANDLETOHWND(GetHandle());
        if (IsWindow(hwnd))
        {
            PAINTSTRUCT pts;
            HDC hdc = ::BeginPaint(hwnd, &pts);
            Rect rcDraw;
            Rect rect(pts.rcPaint);
            pts.fErase = FALSE;

            OnRender(HDCTOHANDLE(hdc), &rect);

            ::EndPaint(hwnd, &pts);
        }
        return true;

    default:
        return pInfo->OnFilterMessage(rootElem, mp);
    }
}

bool VisualHost::HandleEvent(Handle h, Uint32 message, Uint32 wp, Uint32 lp, Uint32& result)
{
    MessageParam mp;
    Rect rect;
    HWND hwnd = HANDLETOHWND(h);

    GetCursorPos(&mp.point);
    GetClientRect(hwnd, &rect);

    ClientToScreen(hwnd, (LPPOINT)&rect);
    ClientToScreen(hwnd, (LPPOINT)&rect + 1);

    fSize logSize(mp.point.x - rect.left, mp.point.y - rect.top);
    // logSize = SystemParameters::TransformFromDevice(logSize);

    mp.hwnd = h;
    mp.message = message;
    mp.lp = (Uint32)lp;
    mp.wp = (Uint32)wp;

    mp.point.x = logSize.cx;
    mp.point.y = logSize.cy;

    bool bRet = HandleEvent(&mp);

    result = mp.result;

    return bRet;
}

void VisualHost::OnRender(Handle h, const Rect* lprc)
{
    Rect rect;
    RenderInfo* pInfo(GetRenderInfo());

    pInfo->PopClip(rect);

    if (NULL == lprc)
    {
        RenderEngine render;
        render.RenderToScreen(this, NULL, (HDC)(DWORD_PTR)h);
    }
    else
    {
        rect.Union(lprc);
        if (!rect.Empty())
        {
            RenderEngine render;
            render.RenderToScreen(this, rect.TofRect(), (HDC)(DWORD_PTR)h);
        }
    }
}

void VisualHost::OnRender(const Rect* lprc)
{
    HWND hwnd(HANDLETOHWND(GetHandle()));
    HDC hdc = ::GetDC(hwnd);
    OnRender(HDCTOHANDLE(hdc), lprc);
    ::ReleaseDC(hwnd, hdc);
}

}
