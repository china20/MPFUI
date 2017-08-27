//////////////////////////////////////////////////////////////////////////////
// Visual.cpp

#include <System/Windows/View.h>
#include <System/Windows/Binding.h>
#include <System/Windows/FrameworkElement.h>

#include <System/Input/Keyboard.h>
#include <System/Input/Mouse.h>
#include <System/Input/MouseDevice.h>

#include <System/Tools/Debug.h>
#include <System/Tools/Thread.h>
#include <System/Tools/ViewTreeHelper.h>
#include <Framework/Animation/Storyboard.h>

/*--------内部使用头文件--------*/
#include <System/Interop/Packaged.h>
#include <System/Interop/InternalOper.h>

#include <typeinfo>

namespace suic
{

ImplementTypeofClass(Element, DpObject)

DpProperty* Element::HitTestFilterActionProperty = NULL;

bool Element::StaticInit()
{
    if (IsStaticInit())
    {
        return true;
    }

    HitTestFilterActionProperty = DpProperty::Register(_T("HitTestFilterAction")
        , ClassType(), Integer::ClassType()
        , new PropMetadata(new Integer(HitTestFilterAction::Continue), PropMetadataOptions::AffectsNone));

    return true;
}

Element::Element()
    : _parentIndex(-1)
    , _vflag(0)
    , _treeLevel(0)
    ,_indent(0)
{
    WriteViewFlag(ViewFlags::IsClipToBounds, true);
}

Element::~Element()
{
    if (_assigner)
    {
        _assigner->__release();
    }
}

Element* Element::Clone()
{
    Element* pView(CastElem<Element>(DpObject::Clone()));

    pView->_vflag = _vflag;
    pView->_treeLevel = _treeLevel;
    pView->_indent = _indent;

    //pView->_descendantBounds = _descendantBounds;

    //pView->_offset = _offset;
    //pView->_canvasOffset = _canvasOffset;

    //pView->_rectClip = _rectClip;
    //pView->_parentIndex = _parentIndex;

    return pView;
}

void Element::Dispose()
{
    if (_assigner)
    {
        _assigner->Dispose();
    }
    __super::Dispose();
}

int Element::GetHitTestFilterAction()
{
    return GetValue(HitTestFilterActionProperty)->ToInt();
}

void Element::SetHitTestFilterAction(int val)
{
    SetValue(HitTestFilterActionProperty, new Integer(val));
}

void Element::WriteViewFlag(int key, bool add)
{
    bool tmp = ReadViewFlag(key);

    if (tmp != add)
    {
        if (add)
        {
            _vflag |= key;
        }
        else
        {
            _vflag &= ~key;
        }
    }
}

const Rect& Element::GetDescendantBounds()
{
    return _descendantBounds;
}

bool Element::GetClipGeometry(Geometry* clip)
{
    return false;
}

int Element::HitTestFilter()
{
    return GetHitTestFilterAction();
}

bool Element::HitTestCore(HitTestResult* hitResult)
{
    hitResult->hitResult = this;
    return true;
}

int Element::HitTest(HitTestResult* hitResult)
{
    return HitTestPoint(hitResult);
}

int Element::HitTestPoint(HitTestResult* hitResult)
{
    return HitTestFilterAction::Continue;
}

int Element::HitTestGeometry(HitTestResult* hitResult)
{
    return HitTestFilterAction::Continue;
}

}
