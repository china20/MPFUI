
#include "invmanager.h"

#include <System/Types/Interface.h>
#include <System/Windows/CoreTool.h>

#include <System/Tools/Array.h>
#include <System/Tools/Thread.h>
#include <System/Tools/HwndHelper.h>

#include <System/Windows/HwndObject.h>
#include <System/Windows/HwndSubclass.h>

#include <System/Animation/TimerThread.h>

#include <System/Tools/SSE.h>

namespace suic
{

class RenderInvInvoker : public AssignerInvoker
{
public:

    RenderInvInvoker()
        : _force(false)
    {
        SetPriority((InvokerPriority)eRender);
        setAutoDelete(false);
    }

    void Init(VisualHostPtr& obj, Rect rc, bool force)
    {
        _obj = obj;
        _rect = rc;
        _force = force;
    }

    void Invoke()
    {
        if (_obj && ::IsWindow(HANDLETOHWND(_obj->GetHandle())))
        {
            _obj->Invalidate(&_rect, _force);
            _obj = VisualHostPtr();
        }
    }

protected:

    VisualHostPtr _obj;
    Rect _rect;
    bool _force;
};

InvManager::InvManager()
    : _target(NULL)
    , _lastUpdate(0)
{
    _curIndex = -1;
    _topIndex = -1;
}

InvManager::~InvManager()
{

}

void InvManager::SetHwndObject(HwndObject* target)
{
    _target = target;
}

int InvManager::GetUpdateTick() const
{
    return (int)(Environment::GetSystemTick() - _lastUpdate);
}

int InvManager::GetClipCount() const
{
    return _curIndex + 1;
}

bool InvManager::PopClip(Rect& rect)
{
    Mutex::Locker sync(_mutex);

    _lastUpdate = Environment::GetSystemTick();

    if (_curIndex >= 0)
    {
        rect = _rects[0];
        for (int i = 0; i < _curIndex; ++i)
        {
            _rects[i] = _rects[i + 1];
        }

        --_curIndex;

        //Debug::Trace(_U("InvManager::PopClip: %d %d %d %d\n"), rect.left, rect.top, rect.right, rect.bottom);
        return true;
    }
    else
    {
        return false;
    }
}

bool InvManager::PeekClip(Rect& rect)
{
    Mutex::Locker sync(_mutex);

    if (_curIndex >= 0)
    {
        rect = _rects[0];
        return true;
    }
    else
    {
        return false;
    }
}

void InvManager::CombineClips()
{
    if (_curIndex > 0)
    {
        for (int i = 1; i <= _curIndex; ++i)
        {
            _rects[0].Union(_rects[i]);
        }
        _curIndex = 0;
    }
}

int InvManager::AddClip(Rect rect)
{
    Mutex::Locker sync(_mutex);
    int iIndex = _curIndex;

    rect.left = max(0, rect.left);
    rect.top = max(0, rect.top);

    if (rect.Width() > 0 && rect.Height() > 0)
    {
        if (_curIndex < MAXCLIPCOUNT - 1)
        {
            if (_curIndex >= 0)
            {
                int i = _curIndex;
                Rect rcMerge = rect;
                Rect rcLast = _rects[i];
                int iFirst = RECTMULITY(rcMerge) + RECTMULITY(rcLast);
                rcMerge.UnionOnly(&rcLast);
                int iSecond = RECTMULITY(rcMerge);

                if (iFirst >= iSecond)
                {
                    _rects[i] = rcMerge;
                    return iIndex;
                }

                ++_curIndex;
                _rects[_curIndex] = rect;
            }
            else
            {
                ++_curIndex;
                _rects[_curIndex] = rect;
            }
        }
        else
        {
            Rect rcMerge = rect;

            for (int i = 0; i <= _curIndex; ++i)
            {
                rcMerge.Union(_rects[i]);
            }

            _rects[0] = rcMerge;
            _curIndex = 0;
        }
    }

    return iIndex;
}

void InvManager::OnTimeout(int id, Object*)
{
    Invalidate();
}

void InvManager::Invalidate()
{
}

void InvManager::InvalidateElement(Element* elem, const Rect* lprc, bool force)
{
    // 最小化不做刷新
    if (!elem || HwndHelper::IsWindowMinimize(elem))
    {
        return;
    }

    Point pt;
    Rect rect;
    fRect rcTmp;
    Element* fe = elem;
    Element* root = elem;
    LayoutTransformInfo* data = NULL;

    if (lprc == NULL)
    {
        rect = elem->GetDescendantBounds();
    }
    else
    {
        rect = *lprc;
        rect.Offset(elem->GetCanvasOffset());
    }

    rcTmp = rect.TofRect();
    root = elem->TransformAncestorRect(rcTmp, NULL);
    rect = rcTmp.ToRect();

    //rect.Inflate(1, 1);

    VisualHost* pHost = VisualHost::GetVisualHost(root);

    if (NULL != pHost && rect.Width() > 0 && rect.Height() > 0)
    {
        pHost->Invalidate(&rect, force);
    }
}

}
