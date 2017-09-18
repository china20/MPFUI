/////////////////////////////////////////////////////////////////////////
// UIPopup.cpp

#include <Framework/Controls/Popup.h>
#include <Framework/Controls/Window.h>
#include <Framework/Controls/TextBlock.h>
#include <Framework/Controls/Application.h>

#include <System/Windows/Assigner.h>
#include <System/Windows/CoreTool.h>
#include <System/Windows/HwndSubclass.h>

#include <System/Input/Mouse.h>
#include <System/Input/Keyboard.h>
#include <System/Input/MouseDevice.h>
#include <System/Input/FocusManager.h>
#include <System/Input/KeyboardNavigation.h>

#include <System/Interop/System.h>
#include <System/Interop/InternalOper.h>
#include <System/Interop/InternalWindowOper.h>

#include <System/Tools/CoreHelper.h>
#include <System/Tools/HwndHelper.h>
#include <System/Tools/VisualTreeOp.h>

namespace suic
{

static bool HandleMouseDown(Object* sender, Popup* popup, HWND hwnd, bool& handled)
{
    VisualHost* visualHost = VisualHost::GetVisualHost(popup->GetPopupRoot());
    bool bIsOver = false;
    Element* owner = popup->GetPlacementTarget();
    Rect rect;
    Point curPt;

    HWND hpop = NULL;

    if (NULL == visualHost)
    {
        return false;
    }

    hpop = HANDLETOHWND(visualHost->GetHandle());

    ::GetCursorPos(&curPt);
    ::GetWindowRect(hpop, &rect);

    if (rect.PointIn(curPt) && hpop == hwnd)
    {
        handled = true;
    }

    if (owner)
    {
        bIsOver = IsMouseInElement(owner);
    }

    if (!bIsOver)
    {
        if (!rect.PointIn(curPt) ||  hwnd != hpop)
        {
            Debug::Trace(_T("popup is closing--------------[%ld]\n"), (DWORD)popup);
            popup->Close();
        }
    }

    return bIsOver;
}

//============================================================
//
ImplementRTTIOfClass(PopupRoot, FrameworkElement)

PopupRoot::PopupRoot()
    : _child(NULL)
{

}
PopupRoot::~PopupRoot()
{
    FREEREFOBJ(_child);
}

Element* PopupRoot::GetChild()
{
    return _child;
}

void PopupRoot::SetChild(Element* elem)
{
    if (_child)
    {
        RemoveVisualChild(_child);
        RemoveLogicalChild(_child);
    }

    SETREFOBJ(_child, elem);

    if (_child)
    {
        AddVisualChild(_child);
        AddLogicalChild(_child);
    }

    SetWidth(-1);
    SetHeight(-1);
}

int PopupRoot::GetVisualChildrenCount()
{
    if (_child != NULL)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
Element* PopupRoot::GetVisualChild(int index)
{
    return _child;
}

Element* PopupRoot::GetLogicalChild(int index)
{
    return _child;
}

int PopupRoot::GetLogicalChildrenCount()
{
    return GetVisualChildrenCount();
}

void PopupRoot::OnTextInput(KeyboardEventArg* e)
{
    if (GetChild()) 
    {
        GetChild()->OnTextInput(e);
    }
    else
    {
        FrameworkElement::OnTextInput(e);
    }
}

void PopupRoot::OnKeyDown(KeyboardEventArg* e)
{
    if (GetChild()) 
    {
        GetChild()->OnKeyDown(e);
    }
    else
    {
        FrameworkElement::OnTextInput(e);
    }
}

void PopupRoot::OnKeyUp(KeyboardEventArg* e)
{
    if (GetChild()) 
    {
        GetChild()->OnKeyUp(e);
    }
    else
    {
        FrameworkElement::OnTextInput(e);
    }
}

void PopupRoot::SetupLayoutBindings(Popup* popup)
{
    Binding* binding = new Binding();

    binding->SetPath(PropertyPath("Width"));
    binding->SetBindMode(BindingMode::OneWay);
    binding->SetSource(popup);
    SetBinding(FrameworkElement::WidthProperty, binding);

    binding = new Binding();

    binding->SetPath(PropertyPath("Height"));
    binding->SetBindMode(BindingMode::OneWay);
    binding->SetSource(popup);
    SetBinding(FrameworkElement::HeightProperty, binding);

    binding->SetPath(PropertyPath("MinWidth"));
    binding->SetBindMode(BindingMode::OneWay);
    binding->SetSource(popup);
    SetBinding(FrameworkElement::MinWidthProperty, binding);

    binding->SetPath(PropertyPath("MinHeight"));
    binding->SetBindMode(BindingMode::OneWay);
    binding->SetSource(popup);
    SetBinding(FrameworkElement::MinHeightProperty, binding);

    binding->SetPath(PropertyPath("MaxWidth"));
    binding->SetBindMode(BindingMode::OneWay);
    binding->SetSource(popup);
    SetBinding(FrameworkElement::MaxWidthProperty, binding);

    binding->SetPath(PropertyPath("MaxHeight"));
    binding->SetBindMode(BindingMode::OneWay);
    binding->SetSource(popup);
    SetBinding(FrameworkElement::MaxHeightProperty, binding);
}

Size PopupRoot::ComputeChildDesiredSize(Size restrictedSize)
{
    GetChild()->Measure(restrictedSize);
    Size desiredSize = GetChild()->GetDesiredSize();
    return Size(min(restrictedSize.Width(), desiredSize.Width()), min(restrictedSize.Height(), desiredSize.Height()));
}

Size PopupRoot::ComputePopupSizeRestrictions(Popup* popup, Size desiredSize, bool& bWidth, bool& bHeight)
{
    Size size = popup->GetPositionInfo()->ChildSize;

    if (size.cx != Numeric::MeasureInt)
    {
        bWidth = size.Width() != desiredSize.Width();
    }

    if (size.cy != Numeric::MeasureInt)
    {
        bHeight = size.Height() != desiredSize.Height();
    }

    return size;
}

Size PopupRoot::OnMeasure(const Size& constraint)
{
    bool bHori = false;
    bool bVert = false;
    Popup* parent = RTTICast<Popup>(GetParent());
    Size availableSize(constraint);
    Element* child = GetChild();

    if (NULL == child)
    {
        return constraint;
    }

    SetMeasureDataOnChild(child, constraint);
    child->Measure(availableSize);
    availableSize = child->GetDesiredSize();

    if (parent == NULL)
    {
        return availableSize;
    }

    Size restrictedSize = ComputePopupSizeRestrictions(parent, availableSize, bHori, bVert);

    if (!bHori && !bVert)
    {
        return availableSize;
    }

    if (bHori != bVert)
    {
        Size size3(bHori ? restrictedSize.Width() : Numeric::MeasureInt, bVert ? restrictedSize.Height() : Numeric::MeasureInt);
        child->Measure(size3);
        availableSize = child->GetDesiredSize();
        restrictedSize = ComputePopupSizeRestrictions(parent, availableSize, bHori, bVert);

        if (!bHori && !bVert)
        {
            return availableSize;
        }
    }

    restrictedSize = ComputeChildDesiredSize(restrictedSize);

    return restrictedSize;
}

void PopupRoot::OnArrange(const Size& arrangeSize)
{
    Element* child = GetChild();
    if (child)
    {
        child->Arrange(Rect(Point(), arrangeSize));
    }
}

//============================================================
//

class PlacementConvert : public IConvertValue
{
public:

    AutoObj ConvertValue(DpProperty* dp, Object* prop)
    {
        String val = prop->ToString();

        if (val.Equals(_U("Left")))
        {
            return PlacementModeBox::LeftBox;
        }
        else if (val.Equals(_U("Top")))
        {
            return PlacementModeBox::TopBox;
        }
        else if (val.Equals(_U("Right")))
        {
            return PlacementModeBox::RightBox;
        }
        else if (val.Equals(_U("Bottom")))
        {
            return PlacementModeBox::BottomBox;
        }
        else if (val.Equals(_U("Center")))
        {
            return PlacementModeBox::CenterBox;
        }
        else if (val.Equals(_U("MousePoint")))
        {
            return PlacementModeBox::MousePointBox;
        }
        else if (val.Equals(_U("Custom")))
        {
            return PlacementModeBox::CustomBox;
        }
        else
        {
            return PlacementModeBox::MouseBox;
        }
    }
};

Integer* PlacementModeBox::LeftBox = Integer::GetPosInt(PlacementMode::pmLeft);
Integer* PlacementModeBox::TopBox = Integer::GetPosInt(PlacementMode::pmTop);
Integer* PlacementModeBox::RightBox = Integer::GetPosInt(PlacementMode::pmRight);
Integer* PlacementModeBox::BottomBox = Integer::GetPosInt(PlacementMode::pmBottom);
Integer* PlacementModeBox::CenterBox = Integer::GetPosInt(PlacementMode::pmCenter);
Integer* PlacementModeBox::MouseBox = Integer::GetPosInt(PlacementMode::pmMouse);
Integer* PlacementModeBox::MousePointBox = Integer::GetPosInt(PlacementMode::pmMousePoint);
Integer* PlacementModeBox::CustomBox = Integer::GetPosInt(PlacementMode::pmCustom);

DpProperty* Popup::ChildProperty;
DpProperty* Popup::IsOpenProperty;
DpProperty* Popup::CoercedPosProperty;
DpProperty* Popup::PlacementProperty;
DpProperty* Popup::PlacementRectangleProperty;
DpProperty* Popup::PlacementTargetProperty;
DpProperty* Popup::VerticalOffsetProperty;
DpProperty* Popup::HorizontalOffsetProperty;
DpProperty* Popup::AllowsTransparencyProperty;

ImplementRTTIOfClass(Popup, FrameworkElement)

Popup::Popup()
    : _onMouseIn(false)
    , _onTimer(false)
    , _onDownClose(true)
    , _isMouseIn(false)
    , _isClosing(false)
    , _frame(NULL)
    , _popupRoot(NULL)
    , _parentPopup(NULL)
{
    _positionInfo.x = 0;
    _positionInfo.y = 0;
    _positionInfo.ChildSize.cx = Numeric::MeasureInt;
    _positionInfo.ChildSize.cy = Numeric::MeasureInt;

    SetVisibilityCache(Visibility::Collapsed);
    WriteFlag(CoreFlags::IsVisibleCache, GetVisibility() == Visibility::Visible);
}

Popup::~Popup()
{
    FREEREFOBJ(_popupRoot);
}

void Popup::OnIsOpenPropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    bool bOpen = e->GetNewValue()->ToBool();
    Popup* popup = RTTICast<Popup>(d);

    if (NULL != popup)
    {
        if (bOpen)
        {
            if (popup->InternalCreate(0, 0) == 0)
            {
                ;
            }
        }
        else
        {
            popup->Close();
        }
    }
}

void Popup::OnPlacementPropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    Popup* popup = DynamicCast<Popup>(d);
}

void Popup::OnOffsetPropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    Popup* popup = DynamicCast<Popup>(d);
}

void Popup::OnPlacementTargetPropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    Popup* popup = DynamicCast<Popup>(d);
}

void Popup::OnChildPropChanged(DpObject* d, DpPropChangedEventArg* e)
{
}

void Popup::OnAllowsTransparencyPropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    Popup* popup = DynamicCast<Popup>(d);
    HWND hwnd = HANDLETOHWND(HwndHelper::GetHostHwnd(popup));

    if (::IsWindow(hwnd))
    {
        DWORD dwExStyle = ::GetWindowLong(hwnd, GWL_EXSTYLE);

        if (e->GetNewValue()->ToBool())
        {
            ::SetWindowLong(hwnd, GWL_EXSTYLE, dwExStyle | 0x80000);
        }
        else
        {
            ::SetWindowLong(hwnd, GWL_EXSTYLE, dwExStyle & (~0x80000));
        }

        int w = SystemParameters::TransformXToDevice(popup->GetWidth());
        int h = SystemParameters::TransformXToDevice(popup->GetHeight());

        HwndObject::HandleWindowRgn(HWNDTOHANDLE(hwnd), popup, w, h);

        if (popup->IsLoaded())
        {
            popup->InvalidateVisual();
        }
    }
}

Integer* PlacementModeBox::From(int index)
{
    Integer* val = NULL;

    switch (index)
    {
    case PlacementMode::pmLeft:
        val = PlacementModeBox::LeftBox;
        break;

    case PlacementMode::pmTop:
        val = PlacementModeBox::TopBox;
        break;

    case PlacementMode::pmRight:
        val = PlacementModeBox::RightBox;
        break;

    case PlacementMode::pmBottom:
        val = PlacementModeBox::BottomBox;
        break;

    case PlacementMode::pmCenter:
        val = PlacementModeBox::CenterBox;
        break;

    case PlacementMode::pmMousePoint:
        val = PlacementModeBox::MousePointBox;
        break;

    case PlacementMode::pmCustom:
        val = PlacementModeBox::CustomBox;
        break;

    default:
        val = PlacementModeBox::MouseBox;
    }

    return val;
}

void Popup::StaticInit()
{
    if (NULL == ChildProperty)
    {
        Window::StaticInit();

        VisibilityProperty->OverrideMetadata(RTTIType()
            , DpPropMemory::GetPropMeta(VisibilityBox::CollapsedBox, PropMetadataOptions::AffectsNone, &Element::OnVisibilityPropChanged));

        ChildProperty = DpProperty::Register(_U("Child"), RTTIType(), Element::RTTIType(), 
            DpPropMemory::GetPropMeta(NULL, 0, &Popup::OnChildPropChanged));
        IsOpenProperty = DpProperty::Register(_U("IsOpen"), RTTIType(), Boolean::RTTIType(), 
            DpPropMemory::GetPropMeta(Boolean::False, 0, &Popup::OnIsOpenPropChanged));
        CoercedPosProperty = DpProperty::Register(_U("CoercedPos"), RTTIType(), Boolean::RTTIType(), 
            DpPropMemory::GetPropMeta(Boolean::True, 0));
        PlacementProperty = DpProperty::Register(_U("Placement"), RTTIType(), Integer::RTTIType(), 
            DpPropMemory::GetPropMeta(PlacementModeBox::MouseBox, 0, &Popup::OnPlacementPropChanged));
        PlacementRectangleProperty = DpProperty::Register(_U("PlacementRectangle"), RTTIType(), ORect::RTTIType(), 
            DpPropMemory::GetPropMeta(new ORect(), 0, &Popup::OnOffsetPropChanged));
        PlacementTargetProperty = DpProperty::Register(_U("PlacementTarget"), RTTIType(), Element::RTTIType(), 
            DpPropMemory::GetPropMeta(NULL, 0, &Popup::OnPlacementTargetPropChanged));
        VerticalOffsetProperty = DpProperty::Register(_U("VerticalOffset"), RTTIType(), Integer::RTTIType(), 
            DpPropMemory::GetPropMeta(Integer::GetZeroInt(), 0, &Popup::OnOffsetPropChanged));
        HorizontalOffsetProperty = DpProperty::Register(_U("HorizontalOffset"), RTTIType(), Integer::RTTIType(), 
            DpPropMemory::GetPropMeta(Integer::GetZeroInt(), 0, &Popup::OnOffsetPropChanged));

        FocusableProperty->OverrideMetadata(RTTIType(), DpPropMemory::GetPropMeta(Boolean::False, PropMetadataOptions::AffectsNone));
        PlacementProperty->SetConvertValueCb(new PlacementConvert());
        AllowsTransparencyProperty = Window::AllowsTransparencyProperty->AddOwner(RTTIType(), DpPropMemory::GetPropMeta(Boolean::True, PropMetadataOptions::AffectsRender, &Popup::OnAllowsTransparencyPropChanged));
    }
}

bool Popup::OnFilterMessage(Object* sender, MessageParam* mp, bool& interrupt)
{
    ElementPtr rootElement(sender);
    bool handled = false;
    HWND hwnd = HANDLETOHWND(mp->hwnd);

    switch (mp->message)
    { 
    case WM_MOUSEACTIVATE: 
        handled = true;
        mp->result = MA_NOACTIVATE;
        break;

    case WM_ACTIVATEAPP: 
        if (mp->wp == 0)
        { 
            Close();
        }
        break;

    case WM_MOUSEWHEEL:
        if (GetPopupRoot())
        {
            MouseWheelEventArg e(mp->point, mp->timestamp, (short)HIWORD(mp->wp));
            GetPopupRoot()->OnMouseWheel(&e);
        }
        handled = true;
        break;

    case WM_NCDESTROY:
        if (GetPlacementTarget())
        {
            GetPlacementTarget()->InvalidateVisual();
        }

        SetIsOpen(false);
        RemoveWindowFilter(GetPopupRoot(), MessageHook());
        Assigner::Current()->RemoveTrackPopup(this);

        if (_frame)
        {
            _frame->SetContinue(false);
        }

        if (sender != GetPopupRoot())
        {
            return false;
        }
 
        break;

    case WM_CLOSE:
        {
            if (IsOpen())
            {
                _isClosing = true;
                CancelEventArg ce(false);
                OnClosing(ce);

                if (!ce.IsCancel())
                {
                    EventArg e;
                    OnClosed(&e);
                    Assigner::Current()->RemoveTrackPopup(this);
                }
                else
                {
                    handled = true;
                }
                _isClosing = false;
            }
        }
        break;

    case WM_LBUTTONDOWN:
        handled = false;
        break;

    case WM_ACTIVATE:
        //if (GetOwner() && (1 == (int)mp->wp))
        //{
            //HWND hwnd(HANDLETOHWND(GetHostHwnd(GetOwner())));
            //SetForegroundWindow(hwnd);
        //}
        break;
    }

    return handled;
}

static bool Process_Popup_WM_KEYDOWN(Popup* pPopup, MessageParam* mp)
{
    FocusManager::SetCurrentInputDevice(KeyboardDeice::Current());

    Element* currElem = pPopup->GetPopupRoot();
    bool handled = false;

    KeyboardEventArg ke(currElem, (int)mp->wp, CoreHelper::ToKeyFlags(mp->lp));

    ke.SetRoutedEvent(Element::PreKeyDownEvent);
    currElem->RaiseEvent(&ke);

    ke.SetRoutedEvent(Element::KeyDownEvent);
    currElem->RaiseEvent(&ke);

    if (!handled)
    {
        KeyboardNavigation::Current()->ProcessInput(currElem, mp);
    }

    return handled;
}

bool Popup::OnSysFilterMessage(Object* sender, MessageParam* mp, bool& interrupt)
{
    ElementPtr rootElement(sender);
    bool handled = false;
    HWND hwnd = HANDLETOHWND(mp->hwnd);

    if (!IsOpen())
    {
        return false;
    }

    switch (mp->message)
    {
    case WM_NCLBUTTONDOWN:
    case WM_LBUTTONDOWN:
    case WM_LBUTTONDBLCLK:
        HandleMouseDown(sender, this, hwnd, interrupt);
        break;

    case WM_MOUSEWHEEL:
        break;

    case WM_LBUTTONUP:
        handled = false;
        break;

    case WM_NCRBUTTONDOWN:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONDBLCLK:
        HandleMouseDown(sender, this, hwnd, interrupt);
        break;

    case WM_MOUSEMOVE:
    case WM_NCMOUSEMOVE:
    //case WM_SETCURSOR:
        if (GetPopupRoot() == sender)
        {
            handled = false;
            interrupt = true;
        }
        else if (!IsMouseInElement(GetPopupRoot()))
        {
            handled = true;
        }
        else
        {
            interrupt = true;
        }
        break;

    case WM_MOUSEACTIVATE:
        mp->result = MA_NOACTIVATE;
        handled = true;
        break;

    case WM_KEYDOWN:
        if ((int)mp->wp == Key::kEscape)
        {
            AsyncClose();
            interrupt = true;
            handled = true;
        }
        else
        {
            handled = Process_Popup_WM_KEYDOWN(this, mp);
            interrupt = true;
        }
        break;

    case WM_CLOSE:
        break;

    case WM_NCDESTROY:
        if (sender == this)
        {
            if (_frame)
            {
                _frame->SetContinue(false);
                _frame->RemoveHookFromObject(this);
            }

            SetIsOpen(false);
            GetAssigner()->RemoveHookFromObject(this);
        }

        break;
    }

    return handled;
}

Size Popup::OnMeasure(const Size& constratint)
{
    return Size();
}

void Popup::OnArrange(const Size& arrangeSize)
{
}

void Popup::OnHitTest(HitResultEventArg* e)
{
    HitResult& ret = e->GetHitResult();

    ret.HitTestCode(CoreFlags::HitClient);
    e->SetHandled(true);
}

void Popup::OnMouseWheel(MouseWheelEventArg* e)
{
}

void Popup::OnMouseLeftButtonDown(MouseButtonEventArg* e)
{
    e->SetHandled(true);
}

void Popup::OnMouseRightButtonDown(MouseButtonEventArg* e)
{
    e->SetHandled(true);
}

void Popup::OnTextInput(KeyboardEventArg* e)
{
    if (GetPopupRoot()) 
    {
        GetPopupRoot()->OnTextInput(e);
    }
    else
    {
        FrameworkElement::OnTextInput(e);
    }
}

void Popup::OnKeyDown(KeyboardEventArg* e)
{
    if (GetPopupRoot()) 
    {
        GetPopupRoot()->OnKeyDown(e);
    }
    else
    {
        FrameworkElement::OnKeyDown(e);
    }

    if (!e->IsHandled())
    {
        //
        // Esc键压下，关闭窗口
        //
        if (e->GetKey() == Key::kEscape )
        {
            e->SetHandled(true);

            HWND hwnd = __HwndFromElement(this);

            if (::IsWindow(hwnd))
            {
                PostMessage(hwnd, WM_CLOSE, 0, 0);
            }
        }
    }
}

void Popup::OnKeyUp(KeyboardEventArg* e)
{
    if (GetPopupRoot()) 
    {
        GetPopupRoot()->OnKeyUp(e);
    }
    else
    {
        FrameworkElement::OnTextInput(e);
    }
}

bool Popup::IsChildPopup()
{
    return false;
}

void Popup::SetChildPopup(bool value)
{
}

Popup* Popup::GetParentPopup()
{
    return _parentPopup;
}

void Popup::SetParentPopup(Popup* val)
{
    _parentPopup = val;
}

bool Popup::IsOpen()
{
    return GetValue(IsOpenProperty)->ToBool();
}

void Popup::SetIsOpen(bool val)
{
    SetValue(IsOpenProperty, BOOLTOBOOLEAN(val));
}

bool Popup::GetCoercedPos()
{
    Boolean* pBool = RTTICast<Boolean>(GetValue(CoercedPosProperty));
    if (NULL != pBool)
    {
        return pBool->ToBool();
    }
    else
    {
        return true;
    }
}

void Popup::SetCoercedPos(bool val)
{
    SetValue(CoercedPosProperty, BOOLTOBOOLEAN(val));
}

PlacementMode Popup::GetPlacement()
{
    return (PlacementMode)GetValue(PlacementProperty)->ToInt();
}

void Popup::SetPlacement(PlacementMode val)
{
    SetValue(PlacementProperty, PlacementModeBox::From(val));
}

Rect Popup::GetPlacementRectangle()
{
    return ORect::FromObj(GetValue(PlacementRectangleProperty));
}

void Popup::GetPlacementRectangle(Rect val)
{
    SetValue(PlacementRectangleProperty, new ORect(val));
}

Element* Popup::GetPlacementTarget()
{
    return RTTICast<Element>(GetValue(PlacementTargetProperty));
}

void Popup::SetPlacementTarget(Element* target)
{
    SetValue(PlacementTargetProperty, target);
}

int Popup::GetVerticalOffset()
{
    return GetValue(VerticalOffsetProperty)->ToInt();
}

void Popup::SetVerticalOffset(int val)
{
    SetValue(VerticalOffsetProperty, new Integer(val));
}

int Popup::GetHorizontalOffset()
{
    return GetValue(HorizontalOffsetProperty)->ToInt();
}

void Popup::SetHorizontalOffset(int val)
{
    SetValue(HorizontalOffsetProperty, new Integer(val));
}

bool Popup::AllowsTransparency()
{
    return GetValue(AllowsTransparencyProperty)->ToBool();
}

void Popup::SetAllowsTransparency(bool val)
{
    if (AllowsTransparency() != val)
    {
        SetValue(AllowsTransparencyProperty, BOOLTOBOOLEAN(val));
    }
}

void Popup::InternalClose(bool async)
{
    HWND hwnd = __HwndFromElement(_popupRoot);

    if (::IsWindow(hwnd))
    {
        if (async)
        {
            PostMessage(hwnd, WM_CLOSE, 0, 0);
        }
        else
        {
            SendMessage(hwnd, WM_CLOSE, 0, 0);
        }
    }
}

static void HandleMouseOverScreen(PositionInfo* posInfo, const Size& offset, const Rect& rect, const Size& desiredSize)
{
    // 大于右边屏幕
    if (posInfo->x + desiredSize.cx > rect.right)
    {
        posInfo->x = rect.right - desiredSize.cx - offset.cx;
    }
    if (posInfo->x < rect.left)
    {
        posInfo->x = rect.left;
    }
    // 大于下边屏幕
    if (posInfo->y + desiredSize.cy > rect.bottom)
    {
        posInfo->y = rect.bottom - desiredSize.cy - offset.cy;
    }
    if (posInfo->y < rect.top)
    {
        posInfo->y = rect.top;
    }
}

void Popup::ComputePosition(Size desiredSize)
{
    Point pos;
    Rect rect;

    Element* placeElem = GetPlacementTarget();
    PositionInfo* posInfo = GetPositionInfo();

    ::GetCursorPos(&pos);

    MonitorInfo mi = Environment::GetMonitorBoundByPoint(&pos);

    rect = mi.rcWork;

    Size offset(GetHorizontalOffset(), GetVerticalOffset());
    PlacementMode pmode = GetPlacement();
    offset.cx = SystemParameters::TransformXToDevice(offset.cx);
    offset.cy = SystemParameters::TransformYToDevice(offset.cy);

    // 
    // 按绝对坐标计算位置
    //
    if (placeElem == NULL || 
        PlacementMode::pmMouse == pmode || 
        PlacementMode::pmMousePoint == pmode || 
        pmode == PlacementMode::pmCustom)
    {
        if (pmode != PlacementMode::pmCustom)
        {
            posInfo->x = pos.x;
            posInfo->y = pos.y;
        }

        if (PlacementMode::pmMouse == pmode)
        {
            //ci.cbSize = sizeof(CURSORINFO);
            //::GetCursorInfo(&ci);
            //ICONINFO iconif;
            //BITMAP bitmap;

            //::GetIconInfo(ci.hCursor, &iconif);
            //::GetObject(iconif.hbmMask, sizeof(BITMAP), (LPSTR)&bitmap);
            posInfo->x += SystemParameters::TransformXToDevice(16);//bitmap.bmWidth;
            posInfo->y += SystemParameters::TransformXToDevice(16);//bitmap.bmHeight;
            //::DeleteObject(iconif.hbmMask);
            //::DeleteObject(iconif.hbmColor);
        }

        posInfo->x += offset.cx;
        posInfo->y += offset.cy;
        HandleMouseOverScreen(posInfo, offset, rect, desiredSize);
    }
    else
    {
        bool coercedPos = GetCoercedPos();
        Point ptCanvas = placeElem->GetCanvasOffset();
        Rect peRect(ToScreenPoint(placeElem, ptCanvas), placeElem->GetRenderSize());

        if (pmode == PlacementMode::pmLeft)
        {
            posInfo->x = peRect.left - desiredSize.Width() - offset.cx;
            posInfo->y = peRect.top + offset.cy;

            // 校正
            if (coercedPos)
            {
                if (posInfo->x < rect.left)
                {
                    posInfo->x = peRect.right + offset.cx;
                }
                if (posInfo->y + desiredSize.Height() > rect.bottom)
                {
                    posInfo->y = peRect.bottom - desiredSize.Height() - offset.cy;
                }
                if (posInfo->y < rect.top)
                {
                    posInfo->y = rect.top;
                }
            }
        }
        else if (pmode == PlacementMode::pmRight)
        {
            posInfo->x = peRect.right + offset.cx;
            posInfo->y = peRect.top + offset.cy;

            // 校正
            if (coercedPos)
            {
                if (posInfo->x + desiredSize.Width() > rect.right)
                {
                    posInfo->x = peRect.left - desiredSize.Width() - offset.cx;
                }
                if (posInfo->y + desiredSize.Height() > rect.bottom)
                {
                    posInfo->y = peRect.bottom - desiredSize.Height() - offset.cy;
                }
                if (posInfo->y < rect.top)
                {
                    posInfo->y = rect.top;
                }
            }
        }
        else if (pmode == PlacementMode::pmTop)
        {
            posInfo->x = peRect.left + offset.cx;
            posInfo->y = peRect.top - desiredSize.cy - offset.cy;
            if (coercedPos)
            {
                if (posInfo->x + desiredSize.Width() > rect.right)
                {
                    posInfo->x = rect.right - desiredSize.Width();
                }
                if (posInfo->x < rect.left)
                {
                    posInfo->x = rect.left;
                }
                if (posInfo->y < rect.top)
                {
                    int newY = peRect.bottom + desiredSize.Height() + offset.cy;
                    if (newY <= rect.bottom)
                    {
                        posInfo->y = peRect.bottom + offset.cy;
                    }
                }
            }
        }
        else if (pmode == PlacementMode::pmBottom)
        {
            posInfo->x = peRect.left + offset.cx;
            posInfo->y = peRect.bottom + offset.cy;

            if (coercedPos)
            {
                if (posInfo->x + desiredSize.Width() > rect.right)
                {
                    posInfo->x = rect.right - desiredSize.Width();
                }
                if (posInfo->x < rect.left)
                {
                    posInfo->x = rect.left;
                }
                if (posInfo->y + desiredSize.Height() > rect.bottom)
                {
                    int newY = peRect.top - desiredSize.cy - offset.cy;
                    if (newY >= rect.top)
                    {
                        posInfo->y = newY;
                    }
                }
            }
        }
        else if (pmode == PlacementMode::pmCenter)
        {
            posInfo->x = peRect.left + (Float) peRect.Width() / 2.0f + offset.cx;
            posInfo->y = peRect.top + (Float) peRect.Height() / 2.0f + offset.cy;
            HandleMouseOverScreen(posInfo, offset, rect, desiredSize);
        }
    }
}

void Popup::InvalidatePopupRoot()
{
    if (NULL == _popupRoot)
    {
        return;
    }

    PositionInfo* posInfo = GetPositionInfo();
    Element* child = GetChild();
    HWND hwnd = __HwndFromElement(_popupRoot);

    if (_popupRoot->GetChild() != child)
    {
        _popupRoot->SetChild(child);
        FrameworkElement* parent = CASTTOFE(GetPlacementTarget());
        if (parent && parent->GetParent())
        {
            parent->GetParent()->AddLogicalChild(this);
        }
    }

    child = _popupRoot->GetChild();

    if (child != NULL && ::IsWindow(hwnd))
    {
        Size desiredSize;
        int flags = SWP_NOACTIVATE | SWP_SHOWWINDOW | SWP_FRAMECHANGED | SWP_NOCOPYBITS;
 
        SetWidth(Numeric::NanInt32);
        SetHeight(Numeric::NanInt32);

        _popupRoot->Measure(Size(Numeric::MeasureInt, Numeric::MeasureInt));

        desiredSize = _popupRoot->GetDesiredSize();

        suic::Rect finalRect(Point(), desiredSize);
        ComputePosition(desiredSize);
        _popupRoot->Arrange(finalRect);

        ::SetWindowPos(hwnd, HWND_TOPMOST, posInfo->x, posInfo->y, desiredSize.Width(), desiredSize.Height(), flags);

        _popupRoot->InvalidateVisual();
    }
}

int Popup::InternalCreate(int wid, int hei)
{
    int iRet = 0;
    HwndParam hp;
    Size desiredSize;
    HWND ownerHwnd = NULL;
    Element* target = NULL;
    Element* child = GetChild();
    PositionInfo* posInfo = GetPositionInfo();

    EnsurePopupRoot();

    if (_popupRoot->GetChild() != child)
    {
        _popupRoot->SetChild(child);
        FrameworkElement* parent = CASTTOFE(GetPlacementTarget());
        if (parent && parent->GetParent())
        {
            parent->GetParent()->AddLogicalChild(this);
        }
    }

    Debug::Trace(_U("Popup::InternalCreate\n"));

    if (NULL == _popupRoot->GetChild())
    {
        SetIsOpen(false);
        return 0;
    }

    _popupRoot->WriteFlag(CoreFlags::IsInitialized, false);
    target = GetTarget();
    
    if (NULL != target)
    {
        ownerHwnd = __HwndFromElement(target);
    }

    Size sizeScreen = Environment::GetScreenClient();
    MeasureData* data = _popupRoot->GetMeasureData();

    SetWidth(Numeric::NanInt32);
    SetHeight(Numeric::NanInt32);

    data->SetViewPort(Rect(0, 0, sizeScreen.cx, sizeScreen.cy));
    _popupRoot->Measure(Size(Numeric::MeasureInt, Numeric::MeasureInt));
    child->InvalidateArrange();

    desiredSize = _popupRoot->GetDesiredSize();
    if (desiredSize.cx <= 0 || desiredSize.cy <= 0)
    {
        SetIsOpen(false);
        return -1;
    }

    ComputePosition(desiredSize);

    hp.state = 0;
    hp.style |= WS_CLIPSIBLINGS | WS_SYSMENU | WS_SIZEBOX | WS_POPUPWINDOW/* | WS_VISIBLE*/; 
    hp.exstyle |= WS_EX_TOOLWINDOW | /*WS_EX_NOACTIVATE | */WS_EX_NOPARENTNOTIFY; 
    hp.width = desiredSize.cx;
    hp.height = desiredSize.cy;
    hp.x = posInfo->x;
    hp.y = posInfo->y;

    if (Opening.safe_bool())
    {
        OpeningEventArg oea(&hp);
        Opening(this, &oea);
    }

    //_popupRoot->SetWidth(hp.width);
    //_popupRoot->SetHeight(hp.height);

    if (IsChildPopup())
    {
        hp.style |= WS_CHILD;
        hp.style &= ~WS_POPUP;
    }
    else
    {
        hp.style &= ~WS_CHILD;
        hp.style |= WS_POPUP;
        hp.exstyle |= WS_EX_TOOLWINDOW | WS_EX_LAYERED;//WS_EX_TOPMOST  | WS_EX_CONTROLPARENT | WS_EX_NOINHERITLAYOUT;
    }

    if (!AllowsTransparency())
    {
        hp.exstyle &= ~WS_EX_LAYERED;
    }

    _isClosing = false;

    HWND hwnd = __HwndFromElement(_popupRoot);
    int flags = /*SWP_NOMOVE | */SWP_NOACTIVATE | SWP_SHOWWINDOW | SWP_FRAMECHANGED | SWP_NOCOPYBITS/* | SWP_NOZORDER | SWP_NOOWNERZORDER*/; 

    if (!::IsWindow(hwnd))
    {
        SysRenderSource* obj = new SysRenderSource(hp);
        obj->ref();

        if (!obj->Initialize(_popupRoot))
        {
            iRet = -1;
        }
        else
        {
            hwnd = HANDLETOHWND(obj->GetHandle());
            if (ownerHwnd != NULL && IsWindow(ownerHwnd) && ownerHwnd != hwnd)
            {
                __SetParentByLongWithHwnd(ownerHwnd, hwnd);
            }
            else
            {
                ownerHwnd = __HwndFromElement(GetPlacementTarget());
                if (IsWindow(ownerHwnd) && hwnd != ownerHwnd)
                {
                    __SetParentByLongWithHwnd(ownerHwnd, hwnd);
                }
            }
            RegisterWindowFilter(GetPopupRoot(), MessageHook(this, &Popup::OnFilterMessage));
        }
        obj->unref();
    }

    if (IsWindow(hwnd))
    {
        ::SetWindowPos(hwnd, HWND_TOPMOST,  hp.x, hp.y, hp.width, hp.height, flags);
        OnShowWindow();
        OnOpened(&EventArg::Empty);
        _popupRoot->InvalidateVisual();
    }
    else
    {
        SetIsOpen(false);
    }

    return iRet;
}

bool Popup::BlockVisualState()
{
    return true;
}

void Popup::EnsurePopupRoot()
{
    if (_popupRoot == NULL)
    {
        _popupRoot = new PopupRoot();
        _popupRoot->ref();
        AddLogicalChild(_popupRoot);
        _popupRoot->SetupLayoutBindings(this);
        _popupRoot->RecursiveInit();
    }
}

Element* Popup::GetTarget()
{
    Element* placementTarget = GetPlacementTarget();
    if (placementTarget == NULL)
    {
        placementTarget = VisualTreeOp::GetParent(this);
    }
    return placementTarget;
}

void Popup::Close()
{
    InternalClose(false);
}

void Popup::Hide()
{
    HWND hwnd = __HwndFromElement(_popupRoot);

    if (::IsWindow(hwnd))
    {
        ::ShowWindow(hwnd, SW_HIDE);
    }
}

void Popup::Show(Point pt)
{
    HWND hwnd = __HwndFromElement(_popupRoot);
    PositionInfo* posInfo = GetPositionInfo();

    posInfo->x = SystemParameters::TransformXToDevice(pt.x);
    posInfo->y = SystemParameters::TransformYToDevice(pt.y);

    if (::IsWindow(hwnd))
    {
        InvalidatePopupRoot();
    }
    else
    {
        SetIsOpen(true);
    }
}

void Popup::AsyncClose()
{
    InternalClose(true);
}

bool Popup::IsClosing()
{
    return _isClosing;
}

PopupRoot* Popup::GetPopupRoot()
{
    EnsurePopupRoot();
    return _popupRoot;
}

PositionInfo* Popup::GetPositionInfo()
{
    return &_positionInfo;
}

Element* Popup::GetChild()
{
    return RTTICast<Element>(GetValue(ChildProperty));
}

void Popup::SetChild(Element* elem)
{
    SetValue(ChildProperty, elem);
}

void Popup::AddChild(Object* obj)
{
    ObjectPtr tmp = obj;
    SetChild(RTTICast<Element>(obj));
}

void Popup::AddText(String val)
{
    TextBlock* text = new TextBlock();

    text->SetText(val);
    AddChild(text);
}

void Popup::RemoveChild(Object* child)
{
    SetChild(NULL);
}

bool Popup::OnShowingWindow()
{
    return false;
}

void Popup::OnShowWindow()
{

}

void Popup::OnHideWindow()
{

}

void Popup::TrackingPopup(MessageHook& hook)
{
    HWND selfhwnd = __HwndFromElement(_popupRoot);

    if (!::IsWindow(selfhwnd))
    {
        // 创建窗口
        SetIsOpen(true);
        selfhwnd = __HwndFromElement(_popupRoot);
    }

    if (!::IsWindow(selfhwnd))
    {
        AsyncClose();
        return;
    }

    // 释放鼠标控制权
    if (Mouse::GetCaptured())
    {
        Mouse::GetCaptured()->ReleaseCaptureMouse();
    }

    _frame = new AssignerFrame();
    _frame->ref();

    if (!hook)
    {
        _frame->RegisterHook(this, MessageHook(this, &Popup::OnSysFilterMessage));
    }
    else
    {
        _frame->RegisterHook(this, hook);
    }

    Assigner::Current()->PushTrackPopup(this);
    Assigner::Current()->PushMessageLoop(_frame);
    Assigner::Current()->RemoveTrackPopup(this);

    if (_frame)
    {
        _frame->SetContinue(false);
        _frame->RemoveHookFromObject(this);
        _frame->unref();
        _frame = NULL;
    }
}

void Popup::OnClosing(CancelEventArg& e)
{
    _isClosing = true;

    if (Closing.safe_bool())
    {
        Closing(this, &e);
    }
}

void Popup::OnClosed(EventArg* e)
{
    if (Closed.safe_bool())
    {
        Closed(this, e);
    }
}

void Popup::OnOpened(EventArg* e)
{
    if (Opened.safe_bool())
    {
        Opened(this, e);
    }
}

void Popup::CreateRootPopup(Popup* popup, Element* child)
{
    if (popup == NULL)
    {
        throw ArgumentNullException("popup", __FILELINE__);
    }
    if (child == NULL)
    {
        throw ArgumentNullException("child", __FILELINE__);
    }

    popup->SetChild(child);

    Binding* binding = new Binding("PlacementTarget");
    binding->SetBindMode(BindingMode::OneWay);
    binding->SetSource(child);
    popup->SetBinding(PlacementTargetProperty, binding);

    binding = new Binding("VerticalOffset");
    binding->SetBindMode(BindingMode::OneWay);
    binding->SetSource(child);
    popup->SetBinding(VerticalOffsetProperty, binding);

    binding = new Binding("HorizontalOffset");
    binding->SetBindMode(BindingMode::OneWay);
    binding->SetSource(child);
    popup->SetBinding(HorizontalOffsetProperty, binding);

    binding = new Binding("PlacementRectangle");
    binding->SetBindMode(BindingMode::OneWay);
    binding->SetSource(child);
    popup->SetBinding(PlacementRectangleProperty, binding);

    binding = new Binding("Placement");
    binding->SetBindMode(BindingMode::OneWay);
    binding->SetSource(child);
    popup->SetBinding(PlacementProperty, binding);

    binding = new Binding("CoercedPos");
    binding->SetBindMode(BindingMode::OneWay);
    binding->SetSource(child);
    popup->SetBinding(CoercedPosProperty, binding);

    /*binding = new Binding("StaysOpen");
    binding->SetBindMode(BindingMode::OneWay);
    binding->SetSource(child);
    popup->SetBinding(StaysOpenProperty, binding);*/

    binding = new Binding("IsOpen");
    binding->SetBindMode(BindingMode::OneWay);
    binding->SetSource(child);
    popup->SetBinding(IsOpenProperty, binding);
}

}
