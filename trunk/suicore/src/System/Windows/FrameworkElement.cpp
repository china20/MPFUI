/////////////////////////////////////////////////////////////////////////
// uiwidget.cpp

#include <System/Windows/FrameworkElement.h>
#include <System/Windows/MemberVisitor.h>
#include <System/Windows/FrameworkElementFactory.h>
#include <System/Windows/ResourceReferenceExpression.h>

#include <Framework/Controls/Menu.h>
#include <Framework/Controls/HwndHost.h>
#include <Framework/Controls/Application.h>
#include <Framework/Controls/TextBlock.h>

#include <Framework/Animation/Storyboard.h>

#include <System/Input/Mouse.h>
#include <System/Input/Keyboard.h>
#include <System/Input/KeyboardNavigation.h>
#include <System/Input/FocusManager.h>

#include <System/Tools/Debug.h>
#include <System/Tools/Thread.h>
#include <System/Tools/EventHelper.h>
#include <System/Tools/StyleHelper.h>
#include <System/Tools/DesignHelper.h>
#include <System/Tools/VisualTreeOp.h>
#include <System/Tools/LogicalTreeOp.h>

#include <System/Windows/CoreTool.h>
#include <System/Windows/TextInput.h>
#include <System/Windows/VisualStates.h>

#include <System/Graphics/Solidcolorbrush.h>

/*--------内部使用头文件--------*/
#include <System/Interop/System.h>
#include <System/Interop/InternalOper.h>

#include <System/Render/RenderManager.h>
#include <System/Render/Skia/SkiaDrawing.h>

#include <System/Windows/HwndSubclass.h>

#include <typeinfo>

namespace suic
{

//========================================================================================
//
RoutedEvent* FrameworkElement::FindToolTipEvent;
RoutedEvent* FrameworkElement::SizeChangedEvent;
RoutedEvent* FrameworkElement::ScrollPosChangedEvent;
RoutedEvent* FrameworkElement::ContextMenuClosingEvent;
RoutedEvent* FrameworkElement::ContextMenuOpeningEvent;
RoutedEvent* FrameworkElement::RequestBringIntoViewEvent;

DpProperty* FrameworkElement::WidthProperty;
DpProperty* FrameworkElement::HeightProperty;
DpProperty* FrameworkElement::MinWidthProperty;
DpProperty* FrameworkElement::MinHeightProperty;
DpProperty* FrameworkElement::MaxWidthProperty;
DpProperty* FrameworkElement::MaxHeightProperty;

DpProperty* FrameworkElement::ActualWidthProperty;
DpProperty* FrameworkElement::ActualHeightProperty;

DpProperty* FrameworkElement::FlowDirectionProperty;
DpProperty* FrameworkElement::FocusVisualStyleProperty;
DpProperty* FrameworkElement::HorizontalAlignmentProperty;
DpProperty* FrameworkElement::VerticalAlignmentProperty;

DpProperty* FrameworkElement::MarginProperty;

DpProperty* FrameworkElement::CursorProperty;
DpProperty* FrameworkElement::ToolTipProperty;

RoutedEvent* FrameworkElement::ToolTipOpeningEvent;
RoutedEvent* FrameworkElement::ToolTipClosingEvent;

DpProperty* FrameworkElement::StyleProperty;
DpProperty* FrameworkElement::TagProperty;
DpProperty* FrameworkElement::LanguageProperty;
DpProperty* FrameworkElement::LayoutTransformProperty;
DpProperty* FrameworkElement::ContextMenuProperty;

PropMetadata* FrameworkElement::_actualWidthMetadata;
PropMetadata* FrameworkElement::_actualHeightMetadata;

Style* FrameworkElement::DefaultFocusVisualStyle;

UncommonField<OSize> FrameworkElement::UnclippedDesiredSizeField;

FrameworkInvoker::FrameworkInvoker(FrameworkElement* sender, Object* data, int type)
    : _target(sender)
    , _data(data)
    , _type(type)
{
    if (_data)
    {
        _data->ref();
    }
}

FrameworkInvoker::~FrameworkInvoker()
{
    if (_data)
    {
        _data->unref();
    }
}

void FrameworkInvoker::CallInvoke(FrameworkElement* elem, Object* data, int type)
{

}

void FrameworkInvoker::Invoke()
{
    FrameworkElement* target = _target.GetElement<FrameworkElement>();
    if (target)
    {
        target->InvalidateDataProp(_data);
    }
}

class FlowDirectionConvert : public IConvertValue
{
public:

    static FlowDirectionConvert* Convert;

    FlowDirectionConvert()
    {
    }

    AutoObj ConvertValue(DpProperty* dp, Object* prop)
    {
        String val = prop->ToString();
        if (val.Equals(_U("RightToLeft")))
        {
            return Integer::GetPosInt(FlowDirection::RightToLeft);
        }
        else
        {
            return Integer::GetPosInt(FlowDirection::LeftToRight);
        }
    }

    String ToString(DpProperty* dp, Object* prop)
    {
        String val;
        int iVal = prop->ToInt();

        switch (iVal)
        {
        case FlowDirection::RightToLeft:
            val = _U("RightToLeft");
            break;

        default:
            val = _U("LeftToRight");
            break;
        }
        return val;
    }
};

FlowDirectionConvert* FlowDirectionConvert::Convert = new FlowDirectionConvert();

MinMaxInfo::MinMaxInfo()
{
    maxHeight = Numeric::MeasureInt;
    minHeight = -1;
    maxWidth = Numeric::MeasureInt;
    minWidth = -1;
}

void MinMaxInfo::UpdateMinMax(FrameworkElement* fe, int w, int h) 
{
    maxHeight = SystemParameters::TransformYToDevice(fe->GetMaxHeight());
    minHeight = SystemParameters::TransformYToDevice(fe->GetMinHeight());
    maxWidth = SystemParameters::TransformXToDevice(fe->GetMaxWidth());
    minWidth = SystemParameters::TransformXToDevice(fe->GetMinWidth());

    int l  = h;
    int height = (l < 0 ? Numeric::MeasureInt : l);

    if (maxHeight < 0)
    {
        maxHeight = Numeric::MeasureInt;
    }

    maxHeight = max(min(height, maxHeight), minHeight);

    height = (l < 0 ? 0 : l); 
    minHeight = max(min(maxHeight, height), minHeight);

    l = w;

    int width = (l < 0 ? Numeric::MeasureInt : l);

    if (maxWidth < 0)
    {
        maxWidth = Numeric::MeasureInt;
    }

    maxWidth = max(min(width, maxWidth), minWidth); 

    width = (l < 0 ? 0 : l);
    minWidth = max(min(maxWidth, width), minWidth); 
}

void FrameworkElement::OnStylePropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    FrameworkElement* fe = RTTICast<FrameworkElement>(d);
    // bool hasStyleLocal = e->GetNewEntry()->GetBaseValueType() == BaseValueType::Local;
    Style* oldStyle = RTTICast<Style>(e->GetOldValue());
    Style* newStyle = RTTICast<Style>(e->GetNewValue());
    // fe->WriteInnerFlag(InnerFlags::HasLocalStyle, hasStyleLocal);
    StyleHelper::UpdateStyleCache(fe, oldStyle, newStyle, fe->_styleCache);
    fe->SetIsStyleSetFromGenerator(false);
}

static bool IsWidthHeightValid(DpObject* d, Object* val)
{
    int num = (int)val->ToInt();
    return (num == -1 || num >= 0);
}

static bool IsMinWidthHeightValid(DpObject* d, Object* val)
{
    int num = (int)val->ToInt();
    return (num >= 0);
}

void FrameworkElement::UpdateMinMaxInfo(int w, int h)
{
    _mmInfo.UpdateMinMax(this, w, h);
}

void FrameworkElement::UpdateMinMaxInfo()
{
    int w = GetWidth();
    int h = GetHeight();

    if (w > 0 && w < Numeric::MeasureInt)
    {
        w = SystemParameters::TransformXToDevice(w);
    }

    if (h > 0 && h < Numeric::MeasureInt)
    {
        h = SystemParameters::TransformXToDevice(h);
    }

    UpdateMinMaxInfo(w, h);
}

Int32 FrameworkElement::CoercedWidth(int w)
{
    if (w < 0)
    {
        w = _width;
    }

    if (w > 0)
    {
        w = SystemParameters::TransformXToDevice(w);
    }

    int width = max(w, _mmInfo.minWidth);
    width = min(w, _mmInfo.maxWidth);

    return width;
}

Int32 FrameworkElement::CoercedHeight(int h)
{
    if (h < 0)
    {
        h = _height;
    }

    if (h > 0)
    {
        h = SystemParameters::TransformXToDevice(h);
    }

    int height = max(h, _mmInfo.minHeight);
    height = min(h, _mmInfo.maxHeight);

    return height;
}

/*Int32 FrameworkElement::GetCoercedWidth()
{
    return CoercedWidth(_width);
}

Int32 FrameworkElement::GetCoercedHeight()
{
    return CoercedHeight(_height);
}*/

void FrameworkElement::OnWidthPropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    FrameworkElement* elem = RTTICast<FrameworkElement>(d);
    elem->_width = e->GetNewValue()->ToInt();
    elem->UpdateMinMaxInfo();
}

void FrameworkElement::OnHeightPropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    FrameworkElement* elem = RTTICast<FrameworkElement>(d);
    elem->_height = e->GetNewValue()->ToInt();
    elem->UpdateMinMaxInfo();
}

void FrameworkElement::OnMinWidthPropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    FrameworkElement* elem = RTTICast<FrameworkElement>(d);
    elem->UpdateMinMaxInfo();
}

void FrameworkElement::OnMinHeightPropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    FrameworkElement* elem = RTTICast<FrameworkElement>(d);
    elem->UpdateMinMaxInfo();
}

void FrameworkElement::OnMaxWidthPropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    FrameworkElement* elem = RTTICast<FrameworkElement>(d);
    elem->UpdateMinMaxInfo();
}

void FrameworkElement::OnMaxHeightPropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    FrameworkElement* elem = RTTICast<FrameworkElement>(d);
    elem->UpdateMinMaxInfo();
}

void FrameworkElement::OnHorizontalAlignmentPropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    FrameworkElement* elem = RTTICast<FrameworkElement>(d);
    elem->_horizontalAlignment = e->GetNewValue()->ToInt();
}

void FrameworkElement::OnVerticalAlignmentPropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    FrameworkElement* elem = RTTICast<FrameworkElement>(d);
    elem->_verticalAlignment = e->GetNewValue()->ToInt();
}

void FrameworkElement::OnMarginPropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    FrameworkElement* elem = RTTICast<FrameworkElement>(d);
    elem->_margin = ORect::FromObj(e->GetNewValue());
}

void FrameworkElement::OnFlowDirectionPropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    FrameworkElement* elem = RTTICast<FrameworkElement>(d);
    if (FlowDirection::LeftToRight == e->GetNewValue()->ToInt())
    {
        elem->WriteInnerFlag(InnerFlags::IsRightToLeft, false);
    }
    else
    {
        elem->WriteInnerFlag(InnerFlags::IsRightToLeft, true);
    }
}

void FrameworkElement::OnLayoutTransformPropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    FrameworkElement* elem = RTTICast<FrameworkElement>(d);
    elem->_layoutTrans = RTTICast<Transform>(e->GetNewValue());
    if (NULL == elem->_layoutTrans)
    {
        elem->_layoutTrans = Transform::GetIdentity();
    }
}

static bool ToolTipIsEnabled(Element* target)
{
    if (NULL == target || !target->IsEnabled() || 
        target->GetValue(FrameworkElement::ToolTipProperty) == DpProperty::UnsetValue())
    {
        return false;
    }
    else
    {
        return true;
    }
}

void FrameworkElement::OnFindToolTip(Object* target, RoutedEventArg* e)
{
    FindToolTipEventArg* fte = (FindToolTipEventArg*)&e;
    if (fte->GetTargetElement() == NULL)
    {
        Element* d = RTTICast<Element>(target);

        if (NULL != d && ToolTipIsEnabled(d))
        {
            fte->SetTargetElement(target);
            e->SetHandled(true);
        }
    }
}

void FrameworkElement::OnPreviewGotKeyboardFocusThunk(Element* sender, KeyboardFocusEventArg* e)
{
    if (sender == e->GetOriginalSource())
    {
        FrameworkElement* fe = CASTTOFE(sender);
        Element* activeElement = FocusManager::GetFocusedElement(fe, true);

        if (activeElement != NULL && activeElement != sender && 
            Keyboard::IsFocusable(activeElement))
        { 
            Element* oldFocus = Keyboard::GetFocusedElement();
            activeElement->Focus();

            if (Keyboard::GetFocusedElement() == activeElement || Keyboard::GetFocusedElement() != oldFocus) 
            {
                e->SetHandled(true);
            }
        } 
    }
}

void FrameworkElement::OnGotKeyboardFocusThunk(Element* sender, KeyboardFocusEventArg* e)
{
    if (sender == e->GetOriginalSource()) 
    {
        FrameworkElement* fe = CASTTOFE(sender);
        KeyboardNavigation::UpdateFocusedElement(fe);
        KeyboardNavigation* keyNav = KeyboardNavigation::Current();
        
        keyNav->ShowFocusVisual(); 
        keyNav->NotifyFocusChanged(fe, e); 
        keyNav->UpdateActiveElement(fe);
    } 
}

void FrameworkElement::LostKeyboardFocusThunk(Element* sender, KeyboardFocusEventArg* e)
{
    if (sender == e->GetOriginalSource())
    { 
        KeyboardNavigation::Current()->HideFocusVisual(); 

        if (e->GetNewFocus() == NULL) 
        {
            KeyboardNavigation::Current()->NotifyFocusChanged(sender, e);
        }
    }
}

void FrameworkElement::OnToolTipOpeningThunk(Element* sender, ToolTipEventArg* e)
{
    FrameworkElement* fe = CASTTOFE(sender);
    fe->OnToolTipOpening(e);
}

void FrameworkElement::OnToolTipClosingThunk(Element* sender, ToolTipEventArg* e)
{
    FrameworkElement* fe = CASTTOFE(sender);
    fe->OnToolTipClosing(e);
}

void FrameworkElement::OnRequestBringIntoViewThunk(DpObject* sender, RoutedEventArg* e)
{
    FrameworkElement* fe = CASTTOFE(sender);
    RequestBringIntoViewEventArg* re = (RequestBringIntoViewEventArg*)e;
    fe->OnRequestBringIntoView(re);
}

void FrameworkElement::StaticInit()
{
    if (NULL == WidthProperty)
    {
        if (Element::NameProperty == NULL)
        {
            Element::StaticInit();
        }
        StyleHelper::StaticInit();

        DefaultFocusVisualStyle = new Style();
        DefaultFocusVisualStyle->ref();

        Uint32 flags = PropMetadataOptions::AffectsMeasure | PropMetadataOptions::AffectsParentMeasure;
        RTTIOfInfo* typeInfo = FrameworkElement::RTTIType();

        SizeChangedEvent = EventHelper::RegisterRoutedEvent(_U("SizeChanged"), RoutingStrategy::Bubble, typeInfo, typeInfo);
        FindToolTipEvent = EventHelper::RegisterRoutedEvent(_U("FindToolTip"), RoutingStrategy::Bubble, typeInfo, typeInfo);
        RequestBringIntoViewEvent = EventHelper::RegisterRoutedEvent(_U("RequestBringIntoView"), RoutingStrategy::Bubble, typeInfo, typeInfo);
        ScrollPosChangedEvent  = EventHelper::RegisterRoutedEvent(_U("ScrollPosChanged"), RoutingStrategy::Tunnel, typeInfo, typeInfo);
        // ContextMenuClosingEvent  = EventHelper::RegisterClassHandler(_U("ContextMenuClosing"), RoutingStrategy::Tunnel, typeInfo, typeInfo);
        // ContextMenuOpeningEvent  = EventHelper::RegisterClassHandler(_U("ContextMenuOpening"), RoutingStrategy::Tunnel, typeInfo, typeInfo);

        ToolTipOpeningEvent = EventHelper::RegisterRoutedEvent(_U("ToolTipOpening"), RoutingStrategy::Direct, ToolTipEventHandler::RTTIType(), typeInfo);
        ToolTipClosingEvent = EventHelper::RegisterRoutedEvent(_U("ToolTipClosing"), RoutingStrategy::Direct, ToolTipEventHandler::RTTIType(), typeInfo);

        EventHelper::RegisterClassHandler(RTTIType(), FindToolTipEvent, new RoutedEventHandler(&FrameworkElement::OnFindToolTip), false);

        WidthProperty = DpProperty::Register(_U("Width"), typeInfo, Integer::RTTIType(), DpPropMemory::GetPropMeta(Integer::GetNegInt(-1), flags, &OnWidthPropChanged), IsWidthHeightValid);
        HeightProperty = DpProperty::Register(_U("Height"), typeInfo, Integer::RTTIType(), DpPropMemory::GetPropMeta(Integer::GetNegInt(-1), flags, &OnHeightPropChanged), IsWidthHeightValid);
        MinWidthProperty = DpProperty::Register(_U("MinWidth"), typeInfo, Integer::RTTIType(), DpPropMemory::GetPropMeta(Integer::GetZeroInt(), flags, &OnMinWidthPropChanged));
        MinHeightProperty = DpProperty::Register(_U("MinHeight"), typeInfo, Integer::RTTIType(), DpPropMemory::GetPropMeta(Integer::GetZeroInt(), flags, &OnMinHeightPropChanged));
        MaxWidthProperty = DpProperty::Register(_U("MaxWidth"), typeInfo, Integer::RTTIType(), DpPropMemory::GetPropMeta(Integer::GetNegInt(-1), flags, &OnMaxWidthPropChanged));
        MaxHeightProperty = DpProperty::Register(_U("MaxHeight"), typeInfo, Integer::RTTIType(), DpPropMemory::GetPropMeta(Integer::GetNegInt(-1), flags, &OnMaxHeightPropChanged));

        _actualWidthMetadata = DpPropMemory::GetPropMeta(Integer::GetZeroInt(), PropMetadataOptions::AffectsNone);
        _actualHeightMetadata = DpPropMemory::GetPropMeta(Integer::GetZeroInt(), PropMetadataOptions::AffectsNone);

        ActualWidthProperty = DpProperty::RegisterReadOnly(_U("ActualWidth"), typeInfo, Integer::RTTIType(), _actualWidthMetadata);
        ActualHeightProperty = DpProperty::RegisterReadOnly(_U("ActualHeight"), typeInfo, Integer::RTTIType(), _actualHeightMetadata);

        FlowDirectionProperty = DpProperty::Register(_U("FlowDirection"), typeInfo, Integer::RTTIType(), DpPropMemory::GetPropMeta(Integer::GetPosInt(FlowDirection::LeftToRight), flags, &OnFlowDirectionPropChanged));
        HorizontalAlignmentProperty = DpProperty::Register(_U("HorizontalAlignment"), RTTIType(), Integer::RTTIType(), DpPropMemory::GetPropMeta(HoriAlignBox::StretchBox, PropMetadataOptions::AffectsParentArrange, &OnHorizontalAlignmentPropChanged));
        VerticalAlignmentProperty = DpProperty::Register(_U("VerticalAlignment"), RTTIType(), Integer::RTTIType(), DpPropMemory::GetPropMeta(VertAlignBox::StretchBox, PropMetadataOptions::AffectsParentArrange, &OnVerticalAlignmentPropChanged));

        MarginProperty = DpProperty::Register(_U("Margin"), RTTIType(), ORect::RTTIType(), DpPropMemory::GetPropMeta(new ORect(), PropMetadataOptions::AffectsParentMeasure, &OnMarginPropChanged));

        CursorProperty = DpProperty::Register(_T("Cursor"), typeInfo, OCursor::RTTIType(), DpPropMemory::GetPropMeta(new OCursor(), PropMetadataOptions::AffectsNone));
        ToolTipProperty = DpProperty::Register(_T("ToolTip"), typeInfo, Object::RTTIType(), DpPropMemory::GetPropMeta(NULL, PropMetadataOptions::AffectsNone));

        // AnimageTagProperty = DpProperty::Register(_U("AnimageTag"), typeInfo, Object::RTTIType(), DpPropMemory::GetPropMeta(NULL, PropMetadataOptions::AffectsNone));

        StyleProperty = DpProperty::Register(_U("Style"), typeInfo, Style::RTTIType(), DpPropMemory::GetPropMeta(NULL, PropMetadataOptions::AffectsParentMeasure, FrameworkElement::OnStylePropChanged));
        FocusVisualStyleProperty = DpProperty::Register(_U("FocusVisualStyle"), typeInfo, Style::RTTIType(), DpPropMemory::GetPropMeta(DefaultFocusVisualStyle, PropMetadataOptions::AffectsNone));

        TagProperty = DpProperty::Register(_U("Tag"), typeInfo, Object::RTTIType(), DpPropMemory::GetPropMeta(NULL, PropMetadataOptions::AffectsNone));
        LanguageProperty = DpProperty::Register(_U("Language"), typeInfo, Object::RTTIType(), DpPropMemory::GetPropMeta(NULL, PropMetadataOptions::AffectsNone));
        LayoutTransformProperty = DpProperty::Register(_U("LayoutTransform"), typeInfo, Transform::RTTIType(), 
            DpPropMemory::GetPropMeta(Transform::GetIdentity(), PropMetadataOptions::AffectsParentMeasure, FrameworkElement::OnLayoutTransformPropChanged));

        ContextMenuProperty = DpProperty::Register(_U("ContextMenu"), typeInfo, ContextMenu::RTTIType(), DpPropMemory::GetPropMeta(NULL, PropMetadataOptions::AffectsNone));
        
        FlowDirectionProperty->SetConvertValueCb(FlowDirectionConvert::Convert);
        HorizontalAlignmentProperty->SetConvertValueCb(AlignmentConvert::Convert);
        VerticalAlignmentProperty->SetConvertValueCb(AlignmentConvert::Convert);
        // CursorProperty->SetConvertValueCb(CurosrConvert::Convert);

        WidthProperty->SetConvertValueCb(AutoN1Convert::Convert);
        HeightProperty->SetConvertValueCb(AutoN1Convert::Convert);

        MinWidthProperty->SetConvertValueCb(AutoN1Convert::Convert);
        MinHeightProperty->SetConvertValueCb(AutoN1Convert::Convert);

        MaxWidthProperty->SetConvertValueCb(AutoN1Convert::Convert);
        MaxHeightProperty->SetConvertValueCb(AutoN1Convert::Convert);

        EventHelper::RegisterClassHandler(RTTIType(), GotKeyboardFocusEvent, new KeyboardFocusEventHandler(&FrameworkElement::OnGotKeyboardFocusThunk), false);
        EventHelper::RegisterClassHandler(RTTIType(), PreGotKeyboardFocusEvent, new KeyboardFocusEventHandler(&FrameworkElement::OnPreviewGotKeyboardFocusThunk), false);
        EventHelper::RegisterClassHandler(RTTIType(), LostKeyboardFocusEvent, new KeyboardFocusEventHandler(&FrameworkElement::LostKeyboardFocusThunk), false);

        EventHelper::RegisterClassHandler(RTTIType(), ToolTipOpeningEvent, new ToolTipEventHandler(&FrameworkElement::OnToolTipOpeningThunk), false);
        EventHelper::RegisterClassHandler(RTTIType(), ToolTipClosingEvent, new ToolTipEventHandler(&FrameworkElement::OnToolTipClosingThunk), false);

        EventHelper::RegisterClassHandler(RTTIType(), RequestBringIntoViewEvent, new RoutedEventHandler(&FrameworkElement::OnRequestBringIntoViewThunk), false);
    }
}

ToolTipEventArg::ToolTipEventArg(bool opening, HwndParam* hp)
    : _hp(hp)
{
    if (opening)
    {
        SetRoutedEvent(FrameworkElement::ToolTipOpeningEvent);
    }
    else
    {
        SetRoutedEvent(FrameworkElement::ToolTipClosingEvent);
    }
}

HwndParam* ToolTipEventArg::GetHwndParam() const
{
    return _hp;
}

void ClickEventArg::CallEventHandler(Object* handler, Object* target)
{
    InternalCall<Element, ClickEventHandler, RoutedEventArg>(handler, target);
}

void ToolTipEventArg::CallEventHandler(Object* handler, Object* target)
{
    InternalCall<Element, ToolTipEventHandler, ToolTipEventArg>(handler, target);
}

void FrameworkElement::AddSizeChanged(RoutedEventHandler* h)
{
    AddHandler(SizeChangedEvent, h);
}

void FrameworkElement::RemoveSizeChanged(RoutedEventHandler* h)
{
    RemoveHandler(SizeChangedEvent, h);
}

OCursor* FrameworkElement::GetCursor()
{
    return RTTICast<OCursor>(GetValue(CursorProperty));
}

void FrameworkElement::SetCursor(OCursor* val)
{
    SetValue(CursorProperty, val);
}

void FrameworkElement::SetFlowDirection(FlowDirection val)
{
    SetValue(FlowDirectionProperty, Integer::GetPosInt(val));
}

Style* FrameworkElement::GetFocusVisualStyle()
{
    Style* style = RTTICast<Style>(GetValue(FocusVisualStyleProperty));
    if (style == NULL)
    {
        style = DefaultFocusVisualStyle;
    }
    return style;
}

void FrameworkElement::SetFocusVisualStyle(Style* style)
{
    SetValue(FocusVisualStyleProperty, style);
}

bool FrameworkElement::MoveFocus(FocusNavDirection req)
{
    return KeyboardNavigation::Current()->Navigate(this, req, (ModifierKey)Keyboard::GetModifiers(), NULL);
}

void FrameworkElement::SetWidth(Int32 value)
{
    if (value < 0)
    {
        SetValue(WidthProperty, new Integer(Numeric::NanInt32));
    }
    else if (value <= GetMinWidth())
    {
        SetValue(WidthProperty, new Integer(GetMinWidth()));
    }
    else
    {
        SetValue(WidthProperty, new Integer(value));
    }
}

void FrameworkElement::SetHeight(Int32 value)
{
    if (value < 0)
    {
        SetValue(HeightProperty, new Integer(Numeric::NanInt32));
    }
    else if (value <= GetMinHeight())
    {
        SetValue(HeightProperty, new Integer(GetMinHeight()));
    }
    else
    {
        SetValue(HeightProperty, new Integer(value));
    }
}

void FrameworkElement::SetMaxWidth(Int32 value)
{
    if (value < 0)
    {
        SetValue(MaxWidthProperty, new Integer(Numeric::NanInt32));
    }
    else
    {
        SetValue(MaxWidthProperty, new Integer(value));
    }
}

void FrameworkElement::SetMaxHeight(Int32 value)
{
    if (value < 0)
    {
        SetValue(MaxHeightProperty, new Integer(Numeric::NanInt32));
    }
    else
    {
        SetValue(MaxHeightProperty, new Integer(value));
    }
}

void FrameworkElement::SetMinWidth(Int32 value)
{
    SetValue(MinWidthProperty, new Integer(value));
}

void FrameworkElement::SetMinHeight(Int32 value)
{
    SetValue(MinHeightProperty, new Integer(value));
}

void FrameworkElement::SetMargin(Rect rc)
{
    SetValue(MarginProperty, new ORect(&rc));
}

int FrameworkElement::GetHorizontalAlignment()
{
    return _horizontalAlignment;
    //return GetValue(HorizontalAlignmentProperty)->ToInt();
}

int FrameworkElement::GetVerticalAlignment()
{
    return _verticalAlignment;
    // return GetValue(VerticalAlignmentProperty)->ToInt();
}

void FrameworkElement::SetHorizontalAlignment(int val)
{
    SetValue(HorizontalAlignmentProperty, HoriAlignBox::From(val));
}

void FrameworkElement::SetVerticalAlignment(int val)
{
    SetValue(VerticalAlignmentProperty, VertAlignBox::From(val));
}

ImplementRTTIOfClass(FrameworkElement, Element)

//======================================================
//
FrameworkElement::FrameworkElement()
    : _innerFlag(0)
    , _styleCache(NULL)
    , _resources(NULL)
    , _templateChild(NULL)
{
    _width = -1;
    _height = -1;
    _unclipSize.cx = -99999;

    _layoutTrans = Transform::GetIdentity();
    _verticalAlignment = VertAlign::vaStretch;
    _horizontalAlignment = HoriAlign::haStretch;

    SetResources(new ResourceDictionary());
}

FrameworkElement::~FrameworkElement()
{
    RemoveVisualChild(_templateChild);
    FREEREFOBJ(_templateChild);

    //_parent = NULL;

    FREEREFOBJ(_styleCache);
    FREEREFOBJ(_resources);
}

void FrameworkElement::Dispose()
{
    StyleHelper::TemplatedBindField->ClearValue(this);

    RemoveVisualChild(_templateChild);
    FREEREFOBJ(_templateChild);

    FREEREFOBJ(_styleCache);
    FREEREFOBJ(_resources);

    Element::Dispose();
}

void FrameworkElement::BeginInit()
{
    Element::BeginInit();
}

void FrameworkElement::WriteInnerFlag(Uint32 flag, bool val)
{
    if (val)
    {
        _innerFlag |= flag;
    }
    else
    {
        _innerFlag &= (~flag);
    }
}

void FrameworkElement::BringIntoView()
{
    BringIntoView(Rect()); 
}

void FrameworkElement::BringIntoView(Rect targetRect) 
{
    RequestBringIntoViewEventArg arg(RequestBringIntoViewEventArg(this, targetRect));

    arg.SetRoutedEvent(RequestBringIntoViewEvent);
    RaiseEvent(&arg);
} 

void FrameworkElement::AddRequestBringIntoView(RoutedEventHandler* handler)
{
    AddHandler(RequestBringIntoViewEvent, handler, false);
}

void FrameworkElement::RemoveRequestBringIntoView(RoutedEventHandler& handler)
{
    RemoveHandler(RequestBringIntoViewEvent, &handler);
}

void FrameworkElement::UpdateLeftMouseDownState(bool isDown)
{
    if (ReadInnerFlag(InnerFlags::IsLeftMouseDown) != isDown)
    {
        WriteInnerFlag(InnerFlags::IsLeftMouseDown, isDown);
        UpdateVisualState(true);
    }
}

void FrameworkElement::OnAncestorChanged(TreeChangeInfo& parentTreeState)
{

}

void FrameworkElement::InvalidateChildrenResourceReference()
{
    int count = GetVisualChildrenCount();

    for (int j = 0; j < count; ++j)
    {
        FrameworkElement* fe = CASTTOFE(GetVisualChild(j));
        if (fe != NULL)
        {
            fe->InvalidateResourceReference(false);
        }
    }
}

void FrameworkElement::InvalidateResourceReference(bool onlySelf)
{
    for (int i = 0; i < _effects.Length(); ++i)
    {
        EffectiveValue effVal = _effects[i];

        if (effVal.IsExpression())
        {
            RRExpression* rrExpr = NULL;

            rrExpr = RTTICast<RRExpression>(effVal.GetReviseValue()->GetBaseValue());

            if (rrExpr)
            {
                rrExpr->ref();

                ObjectPtr val;
                DpProperty* dp = effVal.GetProp();
                PropMetadata* propMeta = dp->GetMetadata(GetRTTIType());

                rrExpr->GetValue(this, dp, val);

                if (val.get() == Expression::NoValue)
                {
                    val = propMeta->GetDefaultValue();
                }

                //if (val != Expression::NoValue)
                {
                    EffectiveValue newEntry(dp, effVal.GetBaseValueType());

                    MemberVisitor::SetValue(&newEntry, rrExpr);
                    MemberVisitor::SetExprValue(&newEntry, val.get(), rrExpr);

                    if (!effVal.GetReviseValue()->GetExprValue()->Equals(val.get()))
                    {
                        _effects[i] = newEntry;

                        Object* oldVal = effVal.GetFlattenedValue(RequestFlags::rfFullyResolved);
                        Object* newVal = newEntry.GetFlattenedValue(RequestFlags::rfFullyResolved);

                        // 值已经改变，触发事件
                        DpPropChangedEventArg e(dp, dp->GetMetadata(GetRTTIType()), oldVal, newVal);
                        NotifyDpPropertyChanged(&e);
                    }
                }

                rrExpr->unref();
            }
        }
    }

    if (!onlySelf)
    {
        InvalidateChildrenResourceReference();
    }
}

void FrameworkElement::OnNewParent(FrameworkElement* newParent)
{
    _parent = newParent;
}

/*
void OnAncestorChanged(FrameworkElement* fe, TreeChangeInfo info)
{
    if (fe != null)
    {
        fe->OnAncestorChangedInternal(info);
    }
}
*/

void FrameworkElement::PrivateInitialized()
{ 
    // 这里处理元素的事件触发器，目前版本不支持，忽略
    Element::PrivateInitialized();
}

void FrameworkElement::TryFireInitialized()
{
    if (!ReadFlag(CoreFlags::IsInitPending) && 
        !ReadFlag(CoreFlags::IsInitialized))
    {        
        UpdateStyleProperty();

        WriteFlag(CoreFlags::IsInitialized, true);
        PrivateInitialized(); 
        OnInitialized(&EventArg::Empty);
    }
}

void FrameworkElement::ChangeLogicalParent(FrameworkElement* newParent)
{ 
    VerifyAccess(__FILELINE__);

    if (newParent != NULL)
    { 
        newParent->VerifyAccess(__FILELINE__);
    } 

    if (_parent != NULL && newParent != NULL && _parent != newParent)
    {
        InvalidOperationException e(_U("HasLogicalParent"), __FILELINE__);
    } 

    if (newParent == this) 
    {
        throw InvalidOperationException(_U("CannotBeSelfParent"), __FILELINE__); 
    }

    WeakRef oldParent = _parent;

    OnNewParent(newParent);

    if (NULL != newParent && newParent->IsInitialized())
    {
        TryFireInitialized();
    }
}

void FrameworkElement::AddLogicalChild(Object* child)
{
    FrameworkElement* elem = RTTICast<FrameworkElement>(child);
    if (elem)
    {
        elem->ChangeLogicalParent(this);
    }
}

void FrameworkElement::RemoveLogicalChild(Object* child)
{
    FrameworkElement* elem = RTTICast<FrameworkElement>(child);

    // 是一个逻辑子元素
    if (elem && elem->_parent == this)
    {
        elem->_parent = NULL;
    }
}

ICommandSource* FrameworkElement::GetCommandSource()
{
    return NULL;
}

IScrollInfo* FrameworkElement::GetScrollInfo()
{
    return NULL;
}

Rect FrameworkElement::MakeVisible(Element* visual, Rect rect)
{
    return Rect();
}

Element* FrameworkElement::GetLogicalChild(int index)
{
    return NULL;
}

int FrameworkElement::GetLogicalChildrenCount()
{
    return 0;
}

Element* FrameworkElement::GetTemplateChild(const String& childName)
{
    Element* elem = NULL;
    FrameworkTemplate* templateInternal = GetTemplateInternal();

    if (templateInternal != NULL)
    {
        elem = templateInternal->FindName(childName, this);
    }

    return elem;
}

void FrameworkElement::SetTemplateChild(FrameworkElement* elem)
{
    if (elem != _templateChild)
    {
        RemoveVisualChild(_templateChild);
        SETREFOBJ(_templateChild, elem);
        AddVisualChild(elem);
        OnTemplateChildChanged();
    }
}

FrameworkTemplate* FrameworkElement::GetTemplateInternal()
{ 
    return NULL;
}

FrameworkTemplate* FrameworkElement::GetTemplateCache()
{
    return NULL;
}

void FrameworkElement::SetTemplateCache(FrameworkTemplate* temp)
{
}

void FrameworkElement::OnTemplateChangedInternal(FrameworkTemplate* oldTemplate, FrameworkTemplate* newTemplate)
{ 
    WriteInnerFlag(InnerFlags::HasTemplateChanged, true);
}

void FrameworkElement::EndInit()
{
    if (!ReadFlag(CoreFlags::IsInitPending))
    {
        throw InvalidOperationException(_U("EndInitWithoutBeginInitNotSupported"), __FILELINE__);
    }

    WriteFlag(CoreFlags::IsInitPending, false);
    TryFireInitialized();

    _offset.x = SystemParameters::TransformXToDevice(GetMargin().left);
    _offset.y = SystemParameters::TransformYToDevice(GetMargin().top);
}

void FrameworkElement::OnSetCursor(CursorEventArg* e)
{
    OCursor* cursor = GetCursor();

    if (cursor && cursor->GetHandle())
    {
        e->SetCursor(cursor);
        e->SetHandled(true);
    }
}

void FrameworkElement::OnMouseEnter(MouseButtonEventArg* e)
{
    Element::OnMouseEnter(e);
}

bool FrameworkElement::OnReadingChild(IXamlNode* node)
{
    return false;
}

void FrameworkElement::OnToolTipOpening(ToolTipEventArg* e)
{

}

void FrameworkElement::OnToolTipClosing(ToolTipEventArg* e)
{

}

void FrameworkElement::OnRequestBringIntoView(RequestBringIntoViewEventArg* e)
{

}

void FrameworkElement::OnDataContextChanged(DpPropChangedEventArg* e)
{
    Element::OnDataContextChanged(e);

    Object* oldVal = e->GetOldValue();
    Object* newVal = e->GetNewValue();

    NotifyPropChanged* oldPropVal = RTTICast<NotifyPropChanged>(oldVal);
    NotifyPropChanged* newPropVal = RTTICast<NotifyPropChanged>(newVal);

    if (oldPropVal)
    {
        oldPropVal->RemoveListener(this);
    }

    if (NULL != newPropVal)
    {
        newPropVal->AddListener(this);
    }
}

void FrameworkElement::OnDataPropertyChanged(Object* data, PropertyChangedEventArg* e)
{
    if (GetAssigner()->GetThreadId() != Thread::CurrentThreadId())
    {
        FrameworkInvoker* invoker = new FrameworkInvoker(this, NULL, 0);
        invoker->ref();
        GetAssigner()->BeginInvoke(invoker);
        invoker->unref();
    }
    else 
    {
        InvalidateDataProp(data);
    }
}

void FrameworkElement::InvalidateDataProp(Object* data)
{
    if (NULL != data)
    {
        data->ref();
        InvalidateDataContextDeps(data, true);
        data->unref();
        //InvalidateVisual();
    }
}

void FrameworkElement::OnRenderSizeChanged(SizeChangedInfo& sizeInfo)
{
    Element::OnRenderSizeChanged(sizeInfo);

    SizeChangedEventArg e(sizeInfo);
    e.SetRoutedEvent(SizeChangedEvent);

    if (sizeInfo.WidthChanged())
    {
        DpPropChangedEventArg ew(ActualWidthProperty, _actualWidthMetadata, 
            new Integer(sizeInfo.GetOldSize().cx), 
            new Integer(sizeInfo.GetNewSize().cx));
        NotifyDpPropertyChanged(&ew);
    }

    if (sizeInfo.HeightChanged())
    {
        DpPropChangedEventArg ew(ActualHeightProperty, _actualHeightMetadata, 
            new Integer(sizeInfo.GetOldSize().cy), 
            new Integer(sizeInfo.GetNewSize().cy));
        NotifyDpPropertyChanged(&ew);
    }

    RaiseEvent(&e);
}

void FrameworkElement::DoPropertyAffects(DpProperty* dp, PropMetadata* meta)
{
    int iAction = 0;

    if (dp == VisibilityProperty)
    {
        iAction = 0;
    }

    if (ReadFlag(CoreFlags::IsInvalidateLocked) || !IsInitialized())
    {
        return;
    }

    if (meta->IsAffectsParentMeasure())
    {
        iAction |= InvTags::Measure;
    }

    if (meta->IsAffectsParentArrange())
    {
        iAction |= InvTags::Arrange;
    }

    if (iAction > 0 || meta->IsAffectsParentRender())
    {
        iAction |= InvTags::Render;
    }

    if (meta->IsAffectsMeasure())
    {
        InvalidateMeasure();
    }

    if (meta->IsAffectsArrange())
    {
        InvalidateArrange();
    }

    if (meta->IsAffectsRender())
    {
        InvalidateVisual();
    }

    if (iAction > 0)
    {
        FrameworkElement* prevParent = NULL;
        FrameworkElement* parent = NULL;

        parent = (FrameworkElement*)GetUIParent();

        while (NULL != parent)
        {
            prevParent = parent;

            if (parent->ReadFlag(CoreFlags::IsInvalidateLocked))
            {
                return;
            }

            if (/*parent->GetRTTIType() == ScrollViewer::RTTIType() || */
                (parent->GetWidth() >= 0 && parent->GetHeight() >= 0))
            {
                break;
            }

            parent = (FrameworkElement*)parent->GetUIParent();
        }

        if (NULL != prevParent)
        {
            if ((iAction & InvTags::Measure) == InvTags::Measure)
            {
                prevParent->InvalidateMeasure();
            }

            if ((iAction & InvTags::Arrange) == InvTags::Arrange)
            {
                prevParent->InvalidateArrange();
            }

            prevParent->InvalidateVisual();
        }
    }
}

void FrameworkElement::OnDpPropertyChanged(DpPropChangedEventArg* e)
{
    DpProperty* dp = e->GetProp();
    Element::OnDpPropertyChanged(e);

    if (e->IsAValueChange())
    {
        // 检查trigger
        if (dp != StyleProperty && dp != Control::TemplateProperty)
        {
            FrameworkTemplate* tfe = GetTemplateInternal();
            FrameworkTemplate* tpfe = RTTICast<FrameworkTemplate>(GetTemplatedParent());
            Style* style = GetStyle();

            if (NULL != style)
            {
                StyleHelper::InvalidateStyleTriggerDependents(this, style, dp, NULL, e, true);
            }

            if (NULL != tfe)
            {
                StyleHelper::InvalidateTemplateTriggerDependents(this, tfe, dp, tfe->GetTrgDepRecords(), e, false, 0);
            }
        }

        DoPropertyAffects(dp, e->GetMetadata());
    }
}

void FrameworkElement::OnStyleChanged(Style* oldStyle, Style* newStyle)
{

}

Point FrameworkElement::ComputeAlignmentOffset(Size clientSize, Size inkSize)
{
    Point offset;
    int horiAlign = GetHorizontalAlignment();
    int vertAlign = GetVerticalAlignment();

    if ((horiAlign == HoriAlign::haStretch) && (inkSize.Width() > clientSize.Width()))
    {
        horiAlign = HoriAlign::haLeft;
    }

    if ((vertAlign == VertAlign::vaStretch) && (inkSize.Height() > clientSize.Height()))
    {
        vertAlign = VertAlign::vaTop;
    }

    switch (horiAlign)
    {
    case HoriAlign::haCenter:
    case HoriAlign::haStretch:
        offset.x = (clientSize.Width() - inkSize.Width()) * 0.5;
        break;

    default:
        if (horiAlign == HoriAlign::haRight)
        {
            offset.x = clientSize.Width() - inkSize.Width();
        }
        else
        {
            offset.x = 0;
        }
        break;
    }

    switch (vertAlign)
    {
    case VertAlign::vaCenter:
    case VertAlign::vaStretch:
        offset.y = (clientSize.Height() - inkSize.Height()) * 0.5;
        return offset;

    case VertAlign::vaBottom:
        offset.y = clientSize.Height() - inkSize.Height();
        return offset;
    }

    offset.y = 0;
    return offset;
}

#pragma optimize("t", on)

void FrameworkElement::Render(Drawing* drawing)
{
    if (ReadFlag(CoreFlags::IsArrangeDirty))
    {
        UpdateArrange();
    }

    // 处理偏移和裁剪
    if (GetVisibility() == Visibility::Visible && !ReadFlag(CoreFlags::IsNotRender))
    {
        fRect elemClip;

        //
        // 设置裁剪区域
        //
        bool bClip = true;
        bool bClipToBounds = IsClipToBounds();
        fPoint offset = GetVisualOffset().TofPoint();
        LayoutTransformInfo* layData = GetLayoutTransformInfo();

        if (NULL != layData)
        {
            elemClip = layData->transform.TransformRect(fRect(Point(), GetRenderSize()));
            elemClip.SetLTRB(0, 0, elemClip.Width(), elemClip.Height());
        }
        else
        {
            elemClip.right = GetActualWidth();
            elemClip.bottom = GetActualHeight();
        }

        // 
        // 转换坐标(元素偏移都是相对父元素，需要转换到窗口坐标)
        // 
        drawing->PushOffset(offset);

        bClip = drawing->ContainsClip(elemClip);

        if (bClip)
        {
            if (bClipToBounds)
            {
                drawing->ClipRect(&elemClip, ClipOp::OpIntersect);
                if (GetClip())
                {
                    drawing->ClipGeometry(GetClip(), ClipOp::OpIntersect, true);
                }
            }

            Transform* trans = GetVisualTransform();
            if (GetOpacity() < 1 || trans != Transform::GetIdentity())
            {
                Bitmap dib;
                Byte alpha = (Byte)(GetOpacity() * 255);
                fPoint bOffset = drawing->GetOffset();
                fRect clip = drawing->GetTopClipBound();

                clip.Offset(-bOffset.x, -bOffset.y);
               
                if (clip.left < 0)
                {
                    clip.left = 0;
                }

                if (clip.top < 0)
                {
                    clip.top = 0;
                }

                bOffset = RenderEngine::RenderShot(this, dib, clip.ToRect());

                // dib.Save(_U("d:\\88888.png"), Bitmap::ImageType::kPNG);

                DrawCtx drawCtx(suic::DrawCtx::eDrawLevel::flLow, true, alpha);

                if (Transform::GetIdentity() != trans)
                {
                    Matrix m;
                    fPoint dOffset(-bOffset.x, -bOffset.y);
                    trans->ComputeMatrix(m, elemClip);

                    drawing->PushOffset(dOffset);
                    
                    drawing->DrawImage(&drawCtx, &dib, &m);
                    drawing->PopOffset();
                }
                else
                {
                    drawing->DrawImage(&drawCtx, &dib, -bOffset.x, -bOffset.y);
                }
            }
            else
            {
                // 执行绘制
                OnRender(drawing);
                RenderChildren(drawing);
            }

#ifdef _DEBUG
            if (CoreSystem::Ins()->GetDrawDebugLine())
            {
                Rect rect = GetRenderRect();
                Pen pen;
                pen.SetBrush(SolidColorBrush::Red);
                pen.SetThickness(1);
                rect.Deflate(1);
                drawing->DrawRect(DrawCtx::DefDraw, NULL, &pen, &rect.TofRect());
            }
#endif
        }

        drawing->PopOffset();
    }
}

void FrameworkElement::ArrangeCore(const Rect& arrangeRect)
{
    if (ReadFlag(CoreFlags::IsMeasureDirty))
    {
        Measure(_measureSize);
    }

    Size desiredSize;
    Size size;
    Rect margin = SystemParameters::TransformToDevice(GetMargin());
    Element* parent = GetUIParent();
    LayoutTransformInfo* data = GetLayoutTransformInfo();

    int marginWidth = margin.SumLR();
    int marginHeight = margin.SumTB();

    SetIsNeedsClipBounds(false);

    _renderSize.cx = arrangeRect.Width();
    _renderSize.cy = arrangeRect.Height();
    
    // 父元素给出的实际大小
    _renderSize.cx = max(0, _renderSize.cx - marginWidth);
    _renderSize.cy = max(0, _renderSize.cy - marginHeight);

    size.cx = _renderSize.cx;
    size.cy = _renderSize.cy;

    if (_unclipSize.cx == -99999)
    {
        desiredSize = GetDesiredSize();
        // 元素本身计算的实际大小
        desiredSize.cx = max(0, desiredSize.Width() - marginWidth);
        desiredSize.cy = max(0, desiredSize.Height() - marginHeight);
    }
    else
    {
        desiredSize = _unclipSize;
    }

    // 如果父元素给出的宽度小于元素本身计算的宽度
    // 进行调整（使用元素本身计算的宽度）
    if (_renderSize.Width() < desiredSize.Width())
    {
        SetIsNeedsClipBounds(true);
        _renderSize.cx = desiredSize.cx;
    }

    if (_renderSize.Height() < desiredSize.Height())
    {
        SetIsNeedsClipBounds(true);
        _renderSize.cy = desiredSize.cy;
    }

    // 如果元素不拉升至父元素区域，则使用元素的计算尺寸
    // 注意，如果元素计算尺寸大于父元素给出的尺寸并且需要
    // 拉升则使用元素计算的尺寸（不是父元素给出的尺寸）
    if (GetHorizontalAlignment() != HoriAlign::haStretch)
    {
        _renderSize.cx = desiredSize.cx;
    }

    if (GetVerticalAlignment() != VertAlign::vaStretch)
    {
        _renderSize.cy = desiredSize.cy;
    }

    WriteDoFlag(ViewFlags::IsSubtreeDirtyForPrecompute, true);

    _finalArrange = arrangeRect;

    if (NULL != data)
    {
        Size sizeTrans = CalcMaximalSpaceRect(data->transform, _renderSize);
        
        _renderSize = sizeTrans;
        desiredSize = data->untransformedSize;

        if ((sizeTrans.cx != 0 && sizeTrans.cy != 0) && 
            (sizeTrans.cx < desiredSize.cx || sizeTrans.cy < desiredSize.cy))
        {
            _renderSize = desiredSize;
        }

        if (_renderSize.cx < desiredSize.cx)
        {
            SetIsNeedsClipBounds(true);
            _renderSize.cx = desiredSize.cx;
        }

        if (_renderSize.cy < desiredSize.cy)
        {
            SetIsNeedsClipBounds(true);
            _renderSize.cy = desiredSize.cy;
        }
    }

    desiredSize.cx = max(desiredSize.cx, _mmInfo.maxWidth);
    desiredSize.cy = max(desiredSize.cy, _mmInfo.maxHeight);

    if (FloatUtil::LessThan(desiredSize.cx, _renderSize.cx))
    {
        SetIsNeedsClipBounds(true);
        _renderSize.cx = desiredSize.cx;
    }

    if (FloatUtil::LessThan(desiredSize.cy, _renderSize.cy))
    {
        SetIsNeedsClipBounds(true);
        _renderSize.cy = desiredSize.cy;
    }

    Size calcSize(min(_renderSize.cx, _mmInfo.maxWidth), min(_renderSize.cy, _mmInfo.maxHeight));

    if (NULL != data)
    {
        fRect rcTmp(0, 0, _renderSize.cx, _renderSize.cy);
        rcTmp = data->transform.TransformRect(rcTmp);
        calcSize = rcTmp.ToRect().ToSize();
    }
    
    _offset = ComputeAlignmentOffset(size, calcSize);

    //
    // 相对于父界面元素的偏移
    //
    _offset.x += arrangeRect.left + margin.left;
    _offset.y += arrangeRect.top + margin.top;

    if (parent != NULL)
    {
        _canvasOffset = parent->GetCanvasOffset();
        _canvasOffset.x += _offset.x;
        _canvasOffset.y += _offset.y;

        _descendantBounds.SetXYWH(_canvasOffset.x, _canvasOffset.y, _renderSize.cx, _renderSize.cy);

        Rect& parentDescendantBounds = parent->GetDescendantBounds();
        OnArrange(_renderSize);
        parentDescendantBounds.UnionOnly(&_descendantBounds);
    }
    else
    {
        _canvasOffset.x = 0;
        _canvasOffset.y = 0;

        _descendantBounds.SetLTRB(0, 0, _renderSize.cx, _renderSize.cy);

        OnArrange(_renderSize);
    }

    Transform* renderTransform = GetRenderTransform();
    if (renderTransform != Transform::GetIdentity() || data != NULL)
    {
        TransformGroup* pGroup = new TransformGroup();

        if (NULL != data)
        {
            Matrix matrix;
            fRect rcTmp(0, 0, _renderSize.cx, _renderSize.cy);

            matrix.SetMatrix(data->transform);
            
            rcTmp = data->transform.TransformRect(rcTmp);
            data->offset.x = rcTmp.left;
            data->offset.y = rcTmp.top;
            matrix.PostTranslate(-data->offset.x, -data->offset.y);

            pGroup->Add(new MatrixTransform(matrix));
        }

        if (Transform::GetIdentity() != renderTransform)
        {
            fPoint origin = GetRenderTransformOrigin();
            bool flag = FloatUtil::Equals(origin.x, 0.0) && FloatUtil::Equals(origin.y, 0.0);

            if (!flag)
            {
                pGroup->Add(new TranslateTransform((origin.x * _renderSize.Width()), (origin.y * _renderSize.Height())));
                pGroup->Add(renderTransform);
                pGroup->Add(new TranslateTransform(-(origin.x * _renderSize.Width()), -(origin.y * _renderSize.Height())));
            }
            else
            {
                pGroup->Add(renderTransform);
            }
        }

        SetVisualTransform(pGroup);
    }
    else
    {
        SetVisualTransform(Transform::GetIdentity());
    }
}

Size FrameworkElement::MeasureCore(const Size& availableSize)
{
    // 
    // 如果模版没有构建，先构建模板
    //
    ApplyTemplate();

    Rect margin = SystemParameters::TransformToDevice(GetMargin());
    int lrSum = margin.left + margin.right;
    int tbSum = margin.top + margin.bottom;
    Transform* layTransform = GetLayoutTransform();

    Size desiredSize;
    Size measureSize;

    if (_mmInfo.minWidth < 0)
    {
        UpdateMinMaxInfo();
    }

    //
    // 控件大小限制应该减掉离父窗口的边距
    //
    measureSize.cx = max(0, (availableSize.cx - lrSum));
    measureSize.cy = max(0, (availableSize.cy - tbSum));

    // 如果有布局转换，先计算最终最大区域
    if (!layTransform->IsIdentity())
    {
        if (NULL == _ltData)
        {
            _ltData = new LayoutTransformInfo();
            layTransform->GetMatrix(_ltData->transform);
        }
    }
    else if (NULL != _ltData)
    {
        delete _ltData;
        _ltData = NULL;
    }

    if (NULL != _ltData)
    {
        measureSize = CalcMaximalSpaceRect(_ltData->transform, measureSize);
    }

    measureSize.cx = max(_mmInfo.minWidth, min(measureSize.cx, _mmInfo.maxWidth));
    measureSize.cy = max(_mmInfo.minHeight, min(measureSize.cy, _mmInfo.maxHeight));

    UpdateMeasureSize(availableSize, measureSize);

    desiredSize = OnMeasure(measureSize);
    
    //
    // 再次校正由派生类计算的大小
    //
    desiredSize.cx = max(desiredSize.cx, _mmInfo.minWidth);
    desiredSize.cy = max(desiredSize.cy, _mmInfo.minHeight);

    bool greatThan = false;
    if (desiredSize.cx > _mmInfo.maxWidth)
    {
        desiredSize.cx = _mmInfo.maxWidth;
        greatThan = true;
    }

    if (desiredSize.cy > _mmInfo.maxHeight)
    {
        desiredSize.cy = _mmInfo.maxHeight;
        greatThan = true;
    }

    Size size = desiredSize;

    if (NULL != _ltData)
    {
        _ltData->untransformedSize = desiredSize;
        size = Transform::TransformSize(_ltData->transform, size);
    }

    /// 
    /// 如果有布局转换，先把测量得到的大小转换
    ///
    if (NULL != _ltData)
    {
        desiredSize = Transform::TransformSize(_ltData->transform, desiredSize);
    }

    //
    // 期望尺寸大小包括其和父窗口的边距
    //
    desiredSize.cx += lrSum;
    desiredSize.cy += tbSum;

    //
    // 最终计算的大小不能超出父窗口给出的大小
    //
    if (desiredSize.cx > availableSize.cx)
    {
        desiredSize.cx = availableSize.cx;
        greatThan = true;
    }

    if (desiredSize.cy > availableSize.cy)
    {
        desiredSize.cy = availableSize.cy;
        greatThan = true;
    }

    /// 
    /// 如果有布局转换，先把测量得到的大小转换
    ///
    if (NULL != _ltData)
    {
        _ltData->transformedSize = desiredSize;
    }

    if (greatThan || desiredSize.Width() < 0 || desiredSize.Height() < 0)
    {
        _unclipSize = size;
    }
    else
    {
        _unclipSize.cx = -99999;
    }

    return desiredSize;
}

void FrameworkElement::UpdateMeasureSize(const Size& availableSize, Size& measureSize)
{
    //
    // 校正控件最终的大小限制
    //
    measureSize.cx = max(_mmInfo.minWidth, min(measureSize.cx, _mmInfo.maxWidth));
    measureSize.cy = max(_mmInfo.minHeight, min(measureSize.cy, _mmInfo.maxHeight));
}

Size FrameworkElement::CalcMaximalSpaceRect(const Matrix& matrix, Size size)
{
    Float width = size.Width();
    Float height = size.Height();
    if (FloatUtil::IsZero(width) || FloatUtil::IsZero(height))
    {
        return Size();
    }

    bool flag = width < 0 || width > (Numeric::MeasureInt - 5);//FloatUtil::IsPosInfinity(width);
    bool flag2 = height < 0 || height > (Numeric::MeasureInt - 5);//FloatUtil::IsPosInfinity(height);

    if (flag && flag2)
    {
        return Size(Numeric::MeasureInt, Numeric::MeasureInt);
    }

    if (flag)
    {
        width = height;
    }
    else if (flag2)
    {
        height = width;
    }

    Float scaleX = matrix.GetValue(0);
    Float skewX = matrix.GetValue(1);
    Float skewY = matrix.GetValue(3);
    Float scaleY = matrix.GetValue(4);
    Float calcWid = 0.0f;
    Float calcHei = 0.0f;

    if (FloatUtil::IsZero(skewX) || FloatUtil::IsZero(skewY))
    {
        Float scaleHei = flag2 ? FloatUtil::PosInfinity : Math::Abs(height / scaleY);
        Float scaleWid = flag ? FloatUtil::PosInfinity : Math::Abs(width / scaleX);

        if (FloatUtil::IsZero(skewX))
        {
            if (FloatUtil::IsZero(skewY))
            {
                calcHei = scaleHei;
                calcWid = scaleWid;
            }
            else
            {
                calcHei = Math::Min(0.5f * Math::Abs(width / skewY), scaleHei);
                calcWid = scaleWid - ((skewY * calcHei) / scaleX);
            }
        }
        else
        {
            calcWid = Math::Min(0.5f * Math::Abs(height / skewX), scaleWid);
            calcHei = scaleHei - ((skewX * calcWid) / scaleY);
        }
    }
    else if (FloatUtil::IsZero(scaleX) || FloatUtil::IsZero(scaleY))
    {
        Float skewHei = Math::Abs((Float)(height / skewX));
        Float skewWid = Math::Abs((Float)(width / skewY));
        if (FloatUtil::IsZero(scaleX))
        {
            if (FloatUtil::IsZero(scaleY))
            {
                calcHei = skewWid;
                calcWid = skewHei;
            }
            else
            {
                calcHei = Math::Min(0.5 * Math::Abs((Float)(height / scaleY)), skewWid);
                calcWid = skewHei - ((scaleY * calcHei) / skewX);
            }
        }
        else
        {
            calcWid = Math::Min(0.5 * Math::Abs((Float)(width / scaleX)), skewHei);
            calcHei = skewWid - ((scaleX * calcWid) / skewY);
        }
    }
    else
    {
        float scaleWidRatio = Math::Abs((Float)(width / scaleX));
        float skewWidRatio = Math::Abs((Float)(width / skewY));
        float skewHeiRatio = Math::Abs((Float)(height / skewX));
        float scaleHeiRatio = Math::Abs((Float)(height / scaleY));

        calcWid = Math::Min(skewHeiRatio, scaleWidRatio) * 0.5;
        calcHei = Math::Min(skewWidRatio, scaleHeiRatio) * 0.5;
        if ((FloatUtil::GreaterThanOrClose(scaleWidRatio, skewHeiRatio) && FloatUtil::LessThanOrClose(skewWidRatio, scaleHeiRatio)) || 
            (FloatUtil::LessThanOrClose(scaleWidRatio, skewHeiRatio) && FloatUtil::GreaterThanOrClose(skewWidRatio, scaleHeiRatio)))
        {
            fRect rect(0, 0, calcWid, calcHei);
            rect = matrix.TransformRect(rect);
            Float d = min((Float)(width / rect.Width()), (Float)(height / rect.Height()));

            if (!FloatUtil::IsNaN(d) && !FloatUtil::IsInfinity(d))
            {
                calcWid *= d;
                calcHei *= d;
            }
        }
    }

    return Size(calcWid, calcHei);
}

FrameworkElement* FrameworkElement::GetParent() const
{
    return (FrameworkElement*)(_parent.GetTarget());
}

#pragma optimize("", on)

void FrameworkElement::SetToolTip(Object* tooltip)
{
    SetValue(ToolTipProperty, tooltip);
}

void FrameworkElement::SetToolTip(const String& val)
{
    if (!val.Empty())
    {
        SetToolTip(new OString(val));
    }
    else
    {
        SetToolTip(NULL);
    }
}

Object* FrameworkElement::GetToolTip()
{
    return GetValue(ToolTipProperty);
}

void FrameworkElement::ShowToolTip(Point pt)
{
    if (ToolTip::Ins() != this)
    {
        if (MouseDevice::GetLeftButton() != MouseButtonState::mbMousePress && 
            MouseDevice::GetRightButton() != MouseButtonState::mbMousePress)
        {
            FindToolTipEventArg e;
            FrameworkElement* targetElem = NULL;

            targetElem = this;

            if (NULL != targetElem/* && ToolTip::Ins()->GetOwner() != targetElem*/)
            {
                Object* pToolTip = targetElem->GetToolTip();
                OString* pToolStr = RTTICast<OString>(pToolTip);
                VisualHostPtr obj = HwndSubclass::MatchRenderSource(this);

                if (!pToolTip || !obj || pToolTip == DpProperty::UnsetValue()) 
                {
                    return;
                }

                if (pToolStr && pToolStr->ToString().Empty())
                {
                    return;
                }

                Style* toolTipStyle = RTTICast<Style>(FindRes(ToolTip::RTTIName()));
                Point ptScr = obj->GetScreenPoint();

                if (NULL == toolTipStyle)
                {
                    return;
                }

                ptScr.y += 16;

                ptScr.x = SystemParameters::TransformXFromDevice(ptScr.x);
                ptScr.y = SystemParameters::TransformYFromDevice(ptScr.y);

                pt.Offset(ptScr);

                ToolTip::Ins()->SetStyle(toolTipStyle);
                ToolTip::Ins()->SetOwner(targetElem);
                ToolTip::Ins()->SetContent(pToolTip);
                ToolTip::Ins()->Show(pt);
            }
        }
    }
}

ContextMenu* FrameworkElement::GetContextMenu()
{
    return RTTICast<ContextMenu>(GetValue(ContextMenuProperty));
}

void FrameworkElement::SetContextMenu(ContextMenu* val)
{
    SetValue(ContextMenuProperty, val);
}

Transform* FrameworkElement::GetLayoutTransform()
{
    return _layoutTrans;
}

void FrameworkElement::SetLayoutTransform(Transform* val)
{
    SetValue(LayoutTransformProperty, val);
}

LayoutTransformInfo* Element::GetLayoutTransformInfo()
{
    return _ltData;
}

void FrameworkElement::SetStyle(Style* val)
{
    SetValue(StyleProperty, val);
}

bool FrameworkElement::InvalidateStyle()
{
    StylePtr newStyle = FindRes(GetRTTIType()->typeName);
    if (!newStyle)
    {
        RTTIOfInfo* rttiInfo = GetRTTIType()->BaseType();
        if (RTTIType() != GetRTTIType())
        {
            while (rttiInfo != RTTIType())
            {
                newStyle = FindRes(rttiInfo->typeName);
                if (newStyle.get() != NULL)
                {
                    break;
                }
                rttiInfo = rttiInfo->BaseType();
            }
        }
    }
    else if (GetRTTIType() != newStyle->GetTargetType())
    {
        newStyle = new Style();
        newStyle->SetTargetType(GetRTTIType());
    }

    // 这里可能使用的id和类名相同
    // 因此需要处理这种情况
    if (newStyle.get() != NULL)
    {
        EffectiveValue entry(StyleProperty, BaseValueType::btStyle);
        EntryIndex entryi = MemberVisitor::FindEntryIndex(this, StyleProperty->GetIndex());

        MemberVisitor::SetValue(&entry, newStyle.get());
        UpdateEV(entryi, StyleProperty, EffectiveValue(), entry, false, false, OperationType::otUnknown);
        return true;
    }

    return false;
}

void FrameworkElement::UpdateStyleProperty()
{
    if (!ReadInnerFlag(InnerFlags::HasStyleInvalidated))
    {
        if (!ReadInnerFlag(InnerFlags::IsStyleUpdateInProgress))
        {
            if (GetStyle() == NULL)
            {
                InvalidateStyle();
            }

            WriteInnerFlag(InnerFlags::IsStyleUpdateInProgress, false);
        }
        else
        {
            throw InvalidOperationException(_U("CyclicStyleReferenceDetected"), __FILELINE__);
        }
    }
}

void FrameworkElement::RecursiveInit()
{
    if (!ReadFlag(CoreFlags::IsInitialized))
    {
        BeginInit();

        for (int i = 0; i < GetVisualChildrenCount(); ++i)
        {
            Element* frame(GetVisualChild(i));

            if (frame != this)
            {
                frame->RecursiveInit();
            }
        }

        EndInit();
    }
}

void FrameworkElement::OnTemplateChildChanged()
{

}

bool FrameworkElement::ApplyTemplate()
{
    bool flag = false;

    if (!ReadInnerFlag(InnerFlags::IsTemplateGenerating))    
    {
        WriteInnerFlag(InnerFlags::IsTemplateGenerating, true);

        OnPreApplyTemplate();

        if (!ReadInnerFlag(InnerFlags::IsTemplateGenerated))
        {
            FrameworkTemplate* templateInternal = GetTemplateInternal();

            int num = 2;
            for (int i = 0; (templateInternal != NULL) && (i < num); i++)
            {
                if (ReadInnerFlag(InnerFlags::IsTemplateGenerated))
                {
                    break;
                }

                AutoObj rootElem = templateInternal->InstantiateTree(this);

                if (rootElem.get() != NULL)
                {
                    WriteInnerFlag(InnerFlags::IsTemplateGenerated, true);
                    OnApplyTemplate();
                }

                if (templateInternal == GetTemplateInternal())
                {
                    break;
                }
                templateInternal = GetTemplateInternal();
            }
        }

        OnPostApplyTemplate();

        WriteInnerFlag(InnerFlags::IsTemplateGenerating, false);
    }

    return flag;
}

FrameworkElement* FrameworkElement::Clone()
{
    FrameworkElement* pElem(DynamicCast<FrameworkElement>(Element::Clone()));

    pElem->_innerFlag = pElem->_innerFlag;

    if (_styleCache)
    {
        if (!pElem->_styleCache)
        {
            pElem->_styleCache = new Style();
            pElem->_styleCache->ref();
        }
    }

    return pElem;
}

//bool FrameworkElement::BeginRenderChildren(Drawing* drawing)
//{
//    return GetVisibleChildrenCount() > 0;
//}
//
//void FrameworkElement::EndRenderChildren(Drawing* drawing)
//{
//}

void FrameworkElement::OnPreApplyTemplate()
{
}

void FrameworkElement::OnApplyTemplate()
{
}

void FrameworkElement::OnPostApplyTemplate()
{
}

void FrameworkElement::OnContextMenu(ContextMenuEventArg* e)
{
    ContextMenu* menu = GetContextMenu();

    if (NULL != menu)
    {
        bool isFocus = IsFocused();

        e->SetHandled(true);

        if (!menu->IsInitialized())
        {
            menu->RecursiveInit();
        }

        menu->SetPlacement(PlacementMode::pmMousePoint);
        menu->SetPlacementTarget(this);
        menu->TrackContextMenu();

        if (isFocus)
        {
            Focus();
        }
    }
}

void FrameworkElement::OnChangeVisualState(bool useTransitions)
{
    String strGrp;
    if (!strGrp.Empty())
    {
        VisualState::GotoState(this, strGrp);
    }

    if (!IsEnabled())
    {
        VisualState::GotoState(this, _U("IsDisabled"));
    }
    else
    {
        if (IsMouseOver())
        {
            VisualState::GotoState(this, _U("IsMouseOver"));
        }

        if (IsFocused())
        {
            VisualState::GotoState(this, _U("IsFocused"));
        }

        if (IsKeyboardFocusWithin())
        {
            VisualState::GotoState(this, _U("IsKeyboardFocusWithin"));
        }

        if (ReadInnerFlag(InnerFlags::IsLeftMouseDown))
        {
            VisualState::GotoState(this, _U("IsPressed"));
        }
    }
}

void FrameworkElement::UpdateVisualState(bool useTransitions)
{
    InvalidateVisual();
}

void FrameworkElement::OnVisualStatePropChanged(DpObject* sender, DpPropChangedEventArg* e)
{
    FrameworkElement* control(dynamic_cast<FrameworkElement*>(sender));
    if (control)
    {
        control->UpdateVisualState(true);
    }
}

FrameworkElement* FrameworkElement::FindResTree(Object** res, const String& name)
{
    FrameworkElement* parent = NULL;
    FrameworkElement* finder = this;

    *res = DpProperty::UnsetValue();

    // 先从模版中查找

    while (finder)
    {
        parent = finder;

        if (finder->GetResources())
        {
            *res = finder->GetResources()->Lookup(name);
        }

        if ((*res) == DpProperty::UnsetValue())
        {
            finder = RTTICast<FrameworkElement>(finder->GetTemplatedParent());
        }
        else
        {
            return parent;
        }
    }

    finder = parent->GetParent();

    while (finder)
    {
        parent = finder;

        if (finder->GetResources())
        {
            *res = finder->GetResources()->Lookup(name);
        }
        if ((*res) == DpProperty::UnsetValue())
        {
            finder = RTTICast<FrameworkElement>(finder->GetParent());

            if (NULL == finder)
            {
                finder = RTTICast<FrameworkElement>(VisualTreeOp::GetParent(parent));
            }
        }
        else
        {
            break;
        }
    }

    return parent;
}

Object* FrameworkElement::FindRes(FrameworkElement* parent, const String& name)
{
    Object* ret = DpProperty::UnsetValue();
    FrameworkElement* root = FindResTree(&ret, name);
    bool inDesignMode = InDesignMode();

    if (ret == DpProperty::UnsetValue())
    {
        if (parent && (inDesignMode == parent->InDesignMode()))
        {
            parent->FindResTree(&ret, name);
        }

        if (ret != DpProperty::UnsetValue())
        {
            return ret;
        }

        Application* pApp = NULL;
        if (inDesignMode)
        {
            pApp = DesignHelper::GetDesignApplication();
        }
        else
        {
            pApp = Application::Current();
        }

        if (pApp != NULL)
        {
            ret = pApp->FindRes(name);
        }
    }

    return ret;
}

Object* FrameworkElement::FindRes(const String& name)
{
    return FindRes(NULL, name);
}

void FrameworkElement::SetResources(ResourceDictionary* resources)
{
    if (resources)
    {
        SETREFOBJ(_resources, resources);
    }
}

void FrameworkElement::ChangedTemplateParent(DpObject* parent)
{
    if (NULL != _templatedParent)
    {
        _templatedParent = parent;

        for (int i = 0; i < GetVisualChildrenCount(); ++i)
        {
            FrameworkElement* elem = CASTTOFE(GetVisualChild(i));

            if (NULL != elem)
            {
                elem->ChangedTemplateParent(parent);
            }
        }
    }
}

void FrameworkElement::OnVisualParentChanged(Element* oldParent)
{
    if (NULL == GetUIParent())
    {
        ChangedTemplateParent(NULL);
    }
}

//void FrameworkElement::OnDpPropertyChanged(DpPropChangedEventArg* e)
//{
//    if (e->GetName().Equals(_U("IsEnabled")))
//    {
//        UpdateVisualState(false);
//        e->SetHandled(true);
//    }
//    Element::OnDpPropertyChanged(e);
//}

//-----------------------------------------------------

TextChangedEventArg::TextChangedEventArg()
{

}

TextChangedEventArg::TextChangedEventArg(String newText)
    : _text(newText)
{

}

String TextChangedEventArg::GetText() const
{
    return _text;
}

}
