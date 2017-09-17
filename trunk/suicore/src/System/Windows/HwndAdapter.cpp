
#include <System/Windows/HwndAdapter.h>
#include <System/Windows/HwndObject.h>
#include <System/Windows/HwndSubclass.h>
#include <System/Graphics/Brush.h>

namespace suic
{

HwndParam::HwndParam()
{
    fSize size(200, 200);
    style = WS_POPUP | WS_MINIMIZEBOX | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_SYSMENU | WS_TABSTOP;
    exstyle = 0;//WS_EX_OVERLAPPEDWINDOW;
    state = 0;
    classStyle = CS_DBLCLKS | CS_CLASSDC | CS_OWNDC | CS_SAVEBITS | CS_NOCLOSE;
    classStyle |= CS_VREDRAW | CS_HREDRAW;

    x = 0;
    y = 0;
    hicon = 0;

    size.cx = SystemParameters::TransformXToDevice(size.cx);
    size.cy = SystemParameters::TransformYToDevice(size.cy);

    width = size.Width();
    height = size.Height();

    parent = 0;
}

MessageParam::MessageParam()
{
    hwnd = NULL;
    message = 0;
    wp = 0;
    lp = 0;
    result = 0;
    timestamp = 0;
}

HookItem::HookItem(Object* o, const MessageHook& m)
    : owner(o)
    , mf(m)
{
}

HookInfo::HookInfo()
{

}

HookInfo::~HookInfo()
{
    Clear();
}

void HookInfo::Clear()
{
    for (int i = 0; i < (int)_hooks.Length(); ++i)
    {
        HookItem* hif(_hooks[i]);
        hif->unref();
    }

    _hooks.Clear();
}

int HookInfo::GetCount() const
{
    return _hooks.Length();
}

HookItem* HookInfo::GetHookItem(int index)
{
    return _hooks[index];
}

void HookInfo::RemoveHookFromObject(Object* obj)
{
    for (int i = 0; i < (int)_hooks.Length(); ++i)
    {
        HookItem* hif(_hooks[i]);
        if (obj == hif->owner)
        {
            _hooks.RemoveAt(i);
            hif->unref();

            --i;
        }
    }
}

void HookInfo::RegisterHook(Object* obj, MessageHook& hook)
{
    if (hook.safe_bool())
    {
        for (int i = 0; i < (int)_hooks.Length(); ++i)
        {
            HookItem* hif(_hooks[i]);
            if (obj == hif->owner && hif->mf.Equals(const_cast<MessageHook*>(&hook)))
            {
                hif->mf = hook;
                return;
            }
        }

        HookItem* hi = new HookItem(obj, hook);
        _hooks.Add(hi);
        hi->ref();
    }
}

void HookInfo::RemoveHook(Object* obj, MessageHook& hook)
{
    for (int i = 0; i < (int)_hooks.Length(); ++i)
    {
        HookItem* hif(_hooks[i]);
        if (hif->mf.Equals(const_cast<MessageHook*>(&hook)))
        {
            _hooks.RemoveAt(i);
            hif->unref();
            break;
        }
    }
}

bool HookInfo::DoMessageHook(Object* root, MessageParam* mp, bool& interrupt)
{
    int i = 0;
    bool handled = false;
    int iCount = GetCount();
    Array<HookItem*, false> hooks;

    for (i = 0; i < iCount; ++i)
    {
        HookItem* hif(GetHookItem(i));
        if (hif)
        {
            hooks.Add(hif);
            hif->ref();
        }
    }

    for (i = iCount - 1; i >= 0; --i)
    {
        HookItem* hif(hooks[i]);

        if (hif->mf.safe_bool())
        {
            handled = hif->mf(root, mp, interrupt);
        }

        if (handled || interrupt)
        {
            break;
        }
    }

    for (i = 0; i < iCount; ++i)
    {
        HookItem* hif(hooks[i]);
        hif->unref();
    }

    return handled;
}

HwndAdapter::HwndAdapter(HwndParam hp)
{
    _hp = hp;
    _value = NULL;
}

HwndAdapter::HwndAdapter()
{
    _value = NULL;
}

HwndAdapter::~HwndAdapter()
{
    HWND hwnd = HANDLETOHWND(GetHwnd());
    if (hwnd)
    {
        ::DestroyWindow(hwnd);
    }
}

bool HwndAdapter::OnFilterMessage(Object* sender, MessageParam* mp)
{
    ElementPtr rootElement(sender);
    bool handled = false;

    /*for (int i = 0; i < _hooks.size(); ++i)
    {
        HwndAdapterHookPtr hook(_hooks[i]);

        if (hook.get())
        {
            handled = hook->OnFilterMessage(rootElement, mp);
        }

        if (handled)
        {
            break;
        }
    }*/

    return handled;
}

Handle HwndAdapter::GetHwnd()
{
    return _value;
}

void HwndAdapter::SetHwnd(Handle hwnd)
{
    _value = hwnd;
}

bool HwndAdapter::BuildHwnd(Object* param)
{
    return BuildHwnd(_hp, param);
}

bool HwndAdapter::BuildHwnd(HwndParam hp, Object* param)
{
    const Char* className = _T("MPFUIHwnd");
    Uint32 styleEx = hp.exstyle;
    
    styleEx |= WS_EX_NOPARENTNOTIFY;

    Uint32 style = hp.style | WS_TABSTOP | hp.state;

    SysRenderSource* visual = RTTICast<SysRenderSource>(param);
    HINSTANCE hIns = (HINSTANCE)::GetModuleHandle(NULL);
    WNDCLASS wndcls;

    if (!::GetClassInfo(hIns, className, &wndcls))
    {
        wndcls.style = hp.classStyle;
        wndcls.cbClsExtra = 0;
        wndcls.cbWndExtra = 0;
        wndcls.hInstance = hIns;
        wndcls.hIcon = (HICON)hp.hicon;//LoadIcon(NULL, IDI_APPLICATION);
        wndcls.hCursor = (HCURSOR)(DWORD_PTR)OCursor::Arrow->GetHandle();
        wndcls.hbrBackground = NULL;//(HBRUSH)GetStockObject(COLOR_WINDOW);
        wndcls.lpszMenuName = NULL;
        wndcls.lpszClassName = className;

        wndcls.lpfnWndProc = HwndSubclass::SubclassWndProc;
        RegisterClass(&wndcls);
    }

    if (NULL == visual)
    {
        return false;
    }

    fSize size(hp.x, hp.y);
    fSize whSize(hp.width, hp.height);

    //size = SystemParameters::TransformToDevice(size);
    //whSize = SystemParameters::TransformToDevice(whSize);

    HWND hwnd = CreateWindowEx(styleEx/* | WS_EX_APPWINDOW*/,
        className,
        NULL,
        style, 
        size.cx,
        size.cy,
        whSize.cx,
        whSize.cy,
        HANDLETOHWND(hp.parent),
        NULL,
        hIns,
        (LPVOID)(LONG_PTR)param);

    _value = HWNDTOHANDLE(hwnd);

    if (IsWindow(hwnd))
    {
        SetWindowLongPtr(hwnd, GWLP_WNDPROC, (DWORD_PTR)HwndSubclass::SubclassWndProc);
        return true;//visual->AttachToHwnd(hwnd);
    }
    else
    {
        return false;
    }
}

void HwndAdapter::Close(bool bAsync)
{
    HWND hwnd = HANDLETOHWND(_value);

    if (::IsWindow(hwnd))
    {
        if (bAsync)
        {
            PostMessage(hwnd, WM_CLOSE, 0, 0);
        }
        else
        {
            SendMessage(hwnd, WM_CLOSE, 0, 0);
        }
    }
}

void HwndAdapter::Clear()
{
    HWND hwnd = HANDLETOHWND(_value);

    if (hwnd)
    {
        DestroyWindow(hwnd);
    }

    _value = NULL;
}

};
