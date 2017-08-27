
#include <System/Windows/CoreTool.h>
#include <System/Tools/Thread.h>

#include <System/Interop/System.h>
#include <System/Render/RenderManager.h>
#include <System/Interop/InternalOper.h>

#include <System/Windows/HwndObject.h>
#include <System/Windows/HwndSubclass.h>

#include <Framework/Controls/Application.h>

#include <Skia/core/SkImageDecoder.h>

namespace suic
{

void* uiMalloc(int size)
{
    return malloc(size);
}

void uiFree(void* ptr)
{
    free(ptr);
}

void UIInit(int level)
{
    CoreSystem::Ins()->SetLogLevel(level);
}

void UIClear()
{
    suiInternalClear();
}

Handle GetUIFont()
{
    return (Handle)(DWORD_PTR)CoreSystem::Ins()->GetDefUIFont();
}

Point GetCursorPoint(Element* element)
{
    Point point;

    ::GetCursorPos(&point);

    if (element != NULL)
    {
        HWND hwnd = __HwndFromElement(element);

        if (::IsWindow(hwnd))
        {
            Rect rect;

            GetWindowRect(hwnd, &rect);

            point.x -= rect.left;
            point.y -= rect.top;
        }
    }

    return point;
}

bool PointInWindow(Element* element)
{
    bool isEqual = false;
    Point point;
    HWND hwnd = __HwndFromElement(element);

    if (hwnd == NULL)
    {
        return false;
    }
    else
    {
        ::GetCursorPos(&point);
        HWND ptHwnd = WindowFromPoint(point);

        while (ptHwnd)
        {
            if (ptHwnd == hwnd)
            {
                isEqual = true;
                break;
            }

            ptHwnd = ::GetParent(ptHwnd);
        }
    }

    return isEqual;
}

Point CalcElementAlign(Element* element, int flag)
{
    return __CalcElementAlignPoint(element, flag);
}

Point ToScreenPoint(Element* element, Point pt)
{
    return __ToScreenPoint(element, pt);
}

ElementPtr GetMouseOverElement(Element* root)
{
    SysRenderSource* obj(HwndSubclass::MatchHwndObject(root));

    if (!obj || !obj->GetRootElement())
    {
        return ElementPtr();
    }
    else
    {
        Point curPt;

        ::GetCursorPos(&curPt);

        if (HANDLETOHWND(obj->GetHandle()) != WindowFromPoint(curPt))
        {
            return ElementPtr();
        }

        Point pt = GetCursorPoint(root);
        Element* hostPtr(obj->GetRootElement());
        HitTestCtx hitResult(pt);

        hostPtr->HitTest(&hitResult);
        return hitResult.hitRes.get();
    }
}

Point CalcuCusorPoint(Element* element)
{
    HWND hwnd = __HwndFromElement(element);

    Point pt;

    if (hwnd)
    {
        ::GetCursorPos(&pt);
        RECT rc;
        ::GetWindowRect(hwnd, &rc);
        pt.x -= rc.left;
        pt.y -= rc.top;
    }

    return pt;
}

short _ntohs(const short word)
{
    static short test = 1;
    static bool bLittleEndianHost = (*((char *) &test) == 1);

    if (bLittleEndianHost) return word;
    return ( (word & 0xff) << 8 ) | ( (word >> 8) & 0xff );
}

int RegisterWindowFilter(Element* element, MessageHook hook)
{
    VisualHost* obj = HwndSubclass::MatchRenderSource(element);
    SysRenderSource* pObj = dynamic_cast<SysRenderSource*>(obj);

    if (pObj != NULL)
    {
        RenderInfo* ri(obj->GetRenderInfo());
        ri->RegisterHook(element, hook);
    }

    return 0;
}

void RemoveWindowFilter(Element* element, MessageHook hook)
{
    VisualHost* obj = HwndSubclass::MatchRenderSource(element);
    SysRenderSource* pObj = dynamic_cast<SysRenderSource*>(obj);

    if (pObj != NULL)
    {
        RenderInfo* ri(obj->GetRenderInfo());
        ri->RemmoveHook(element, hook);
    }
}

void CloseAssignerFrame(AssignerFrame* frame)
{
    if (frame)
    {
        //frame->RemoveAssignerFrame();
    }
}

void SetElementTopMost(Element*, bool)
{
}

}
