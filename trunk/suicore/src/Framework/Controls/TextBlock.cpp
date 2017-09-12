
#include <System/Input/MouseDevice.h>
#include <System/Tools/EventHelper.h>
#include <System/Tools/CommandOp.h>
#include <Framework/Controls/TextEditor.h>
#include <Framework/Controls/TextBlock.h>

namespace suic
{

class SUICORE_API TextBlockRun : public Run
{
public:

    RTTIOfClass(TextBlockRun)

    TextBlockRun()
    {
    }

    TextBlockRun(const suic::String& text)
        : Run(text)
    {
    }
};

suic::DpProperty* Run::TextProperty;

suic::DpProperty* Inline::BaselineAlignmentProperty;
suic::DpProperty* Inline::FlowDirectionProperty;
suic::DpProperty* Inline::TextDecorationsProperty;

suic::DpProperty* TextBlock::BackgroundProperty;
suic::DpProperty* TextBlock::ForegroundProperty;

suic::DpProperty* TextBlock::FontFamilyProperty;
suic::DpProperty* TextBlock::FontSizeProperty;
suic::DpProperty* TextBlock::FontStretchProperty;
suic::DpProperty* TextBlock::FontStyleProperty;
suic::DpProperty* TextBlock::FontWeightProperty;

suic::DpProperty* TextBlock::LineHeightProperty;
suic::DpProperty* TextBlock::LineSpacingProperty;

suic::DpProperty* TextBlock::PaddingProperty;
suic::DpProperty* TextBlock::TextAlignmentProperty;
suic::DpProperty* TextBlock::TextProperty;
suic::DpProperty* TextBlock::TextEffectsProperty;
suic::DpProperty* TextBlock::TextDecorationsProperty;
suic::DpProperty* TextBlock::TextTrimmingProperty;
suic::DpProperty* TextBlock::TextWrappingProperty;

ImplementRTTIOfClass(Inline, suic::TextElement)
ImplementRTTIOfClass(Run, Inline)
ImplementRTTIOfClass(TextBlockRun, Run)
ImplementRTTIOfClass(Span, Inline)
ImplementRTTIOfClass(LineBreak, Inline)
ImplementRTTIOfClass(Bold, Span)
ImplementRTTIOfClass(Italic, Span)
ImplementRTTIOfClass(Underline, Span)
ImplementRTTIOfClass(Hyperlink, Span)
ImplementRTTIOfClass(InlineUIContainer, Inline)
ImplementRTTIOfClass(TextBlock, FrameworkElement)

LineMeasure::LineMeasure()
{
    Clear();
}

void LineMeasure::Clear()
{
    trimWidth = 0;
    currWidth = 0;

    trimHeight = 0;
    currHeight = 0;

    maxWidth = 0;
    curMaxWidth = 0;

    lastItem = false;
    lineTrm = NULL;
    textObj = NULL;

    txtWrapping = suic::TextWrapping::tNoWrap;
    txtTrimming = suic::TextTrimming::tNone;
}

LineDrawing::LineDrawing()
    : start(0)
    , count(0)
    , right(0)
    , drawing(NULL)
    , textBlock(NULL)
{
}

LinePart::LinePart()
    : offset(0)
    , count(0)
    , start(0)
    , width(0)
    , height(0)
    , lineObj(0)
{
}

LineTrim::LineTrim()
    : _lineHeight(0)
    , _lineWidth(0)
    , _hasUIContainer(false)
{
}

LineTrim::~LineTrim()
{
    Clear();
}

int LineTrim::GetCount() const
{
    return _lineParts.GetCount();
}

LinePart* LineTrim::GetLinePart(int index)
{
    return _lineParts[index];
}

void LineTrim::AddLinePart(LinePart* linePart)
{
    if (linePart->lineObj)
    {
        linePart->lineObj->ref();
    }
    _lineParts.Add(linePart);
    SetLineHeight(linePart->height);
    _lineWidth += linePart->width;
}

void LineTrim::Clear()
{
    for (int i = 0; i < _lineParts.GetCount(); ++i)
    {
        if (_lineParts[i]->lineObj)
        {
            _lineParts[i]->lineObj->unref();
        }
        delete _lineParts[i];
    }

    _lineParts.Clear();
}

void LineTrim::SetLineHeight(int val)
{
    if (val > 0 && val > _lineHeight)
    {
        _lineHeight = val;
    }
}

int LineTrim::GetLineHeight() const
{
    return _lineHeight;
}

void LineTrim::SetHasUIContainer(bool val)
{
    _hasUIContainer = val;
}

bool LineTrim::HasUIContainer() const
{
    return _hasUIContainer;
}

//=======================================================
//
Inline::Inline()
    : _textDecoration(suic::TextDecorations::none)
{
}

Inline::~Inline()
{
}

suic::TextDecorationCollection* Inline::GetTextDecorations()
{
    return dynamic_cast<suic::TextDecorationCollection*>(GetValue(TextDecorationsProperty));
}

void Inline::SetTextDecorations(suic::TextDecorationCollection* val)
{
    SetValue(TextDecorationsProperty, val);
}

void Inline::OnTextDecorationsPropChanged(suic::DpObject* d, suic::DpPropChangedEventArg* e)
{
    Inline* pInline = suic::RTTICast<Inline>(d);
    if (NULL != pInline)
    {
        pInline->_textDecoration = suic::RTTICast<suic::TextDecorationCollection>(e->GetNewValue());
        if (NULL == pInline->_textDecoration)
        {
            pInline->_textDecoration = suic::TextDecorations::none;
        }
    }
}

void Inline::StaticInit()
{
    if (NULL == TextDecorationsProperty)
    {
        TextElement::StaticInit();

        BaselineAlignmentProperty = suic::DpProperty::Register(_T("BaselineAlignment"), RTTIType(), suic::Integer::RTTIType(), suic::DpPropMemory::GetPropMeta(suic::Integer::GetZeroInt(), suic::PropMetadataOptions::AffectsMeasure));
        TextDecorationsProperty = suic::DpProperty::Register(_T("TextDecorations"), RTTIType()
            , suic::TextDecorationCollection::RTTIType()
            , suic::DpPropMemory::GetPropMeta(suic::TextDecorationCollection::GetEmpty()
            , suic::PropMetadataOptions::AffectsRender, &Inline::OnTextDecorationsPropChanged));

        TextDecorationsProperty->SetConvertValueCb(suic::TextDecorationsConvert::Convert);

        /*TextDecoration::PenProperty;
        TextDecoration::LocationProperty;
        TextDecoration::PenOffsetProperty;

        TextDecorations::baseline;
        TextDecorations::overLine;
        TextDecorations::strikethrough;
        TextDecorations::underline;*/
    }
}

int Inline::GetCount()
{
    return 0;
}

Inline* Inline::GetInline(int index)
{
    return NULL;
}

void Inline::AddText(suic::String text)
{
}

void Inline::AddChild(suic::Object* child)
{
    child->ref();
    child->unref();
}

void Inline::RemoveChild(Object* child)
{

}

void Inline::Measure(LineMeasure& info)
{

}

void Inline::Render(LineDrawing& drawing)
{

}

//=================================================
// InlineCollection

InlineCollection::InlineCollection(suic::Element* p)
    : _parent(p)
{
}

InlineCollection::~InlineCollection()
{
    Clear();
}

suic::DpObject* InlineCollection::GetParent() const
{
    return _parent;
}

void InlineCollection::AddText(suic::String text)
{
    Run* run = NULL;
    run = new Run(text);
    AddChild(run);
}

void InlineCollection::AddChild(suic::Object* child)
{
    Inline* item = suic::DynamicCast<Inline>(child);

    child->ref();

    if (NULL != item)
    {
        CheckInnerText();

        item->ref();
        _inlines.Add(item);

        if (_parent != NULL)
        {
            _parent->AddVisualChild(item);
        }
    }

    child->unref();
}

void InlineCollection::RemoveAt(int index)
{
    _inlines[index]->unref();
    _inlines.RemoveAt(index);
}

void InlineCollection::RemoveChild(suic::Object* child)
{
    for (int i = 0; i < _inlines.GetCount(); ++i)
    {
        if (_inlines[i] == child)
        {
            child->unref();
            _inlines.RemoveAt(i);
            break;
        }
    }
}

void InlineCollection::Clear()
{
    for (int i = 0; i < _inlines.GetCount(); ++i)
    {
        if (_inlines[i] != NULL)
        {
            _inlines[i]->unref();
        }
    }
    _inlines.Clear();
}

void InlineCollection::CheckInnerText()
{
    TextBlock* textBlock = suic::DynamicCast<TextBlock>(GetParent());

    if (NULL != textBlock && _inlines.GetCount() == 1)
    {
        if (TextBlockRun::RTTIType() == _inlines[0]->GetRTTIType())
        {
            textBlock->SetText(L"");
            Clear();
        }
    }
}

//=================================================
// Run

Run::Run()
{
}

Run::Run(suic::String text)
{
    AddText(text);
}

Run::~Run()
{
}

void Run::OnTextPropChanged(suic::DpObject* d, suic::DpPropChangedEventArg* e)
{
    Run* pRun = suic::DynamicCast<Run>(d);
    if (pRun != NULL)
    {
        pRun->_text = e->GetNewValue()->ToString();
    }
}

void Run::StaticInit()
{
    if (NULL == TextProperty)
    {
        Inline::StaticInit();
        TextProperty = suic::DpProperty::Register(_T("Text"), RTTIType(), suic::OString::RTTIType()
            , suic::DpPropMemory::GetPropMeta(suic::OString::EmptyString, suic::PropMetadataOptions::AffectsParentMeasure, &Run::OnTextPropChanged));
    }
}


void Run::AddText(suic::String text)
{
    SetValue(TextProperty, new suic::OString(text));
}

void Run::AddChild(suic::Object* child)
{
    child->ref();
    if (child->GetRTTIType() == suic::OString::RTTIType())
    {
        SetValue(TextProperty, child);
    }
    child->unref();
}

suic::String Run::GetText()
{
    return _text;
}

void Run::SetText(suic::String text)
{
    VerifyAccess(__FILELINE__);
    AddText(text);
}

void Run::Measure(LineMeasure& info)
{
    suic::FormattedText tra;
    suic::Size size;
    suic::TmParam tm;
    int iCount = 0;
    int iStart = 0;             // 开始字符位置

    bool bFinish = false;
    bool bNewLine = false;
    LinePart* linePart = NULL;
    
    if (_text.Length() == 0)
    {
        return;
    }

#define TmAndInfoToLinePart() \
    linePart->count = tm.charCount;\
    linePart->offset = info.trimWidth -info.currWidth;\
    linePart->width = tm.size.Width();\
    linePart->height = tm.size.Height();\
    linePart->lineObj = this;\

    tra.SetFontSize(GetRealFontSize());
    tra.SetSingleline(true);
    tra.SetFontWeight(GetFontWeight());
    tra.SetFontFamily(GetFontFamily());

    while (!bFinish)
    {
        iCount = _text.Length() - iStart;

        if (info.txtWrapping == suic::TextWrapping::tNoWrap)
        {
            tm.constaint.cx = 0;
        }
        else
        {
            tm.constaint.cx = info.currWidth;
        }

        tra.MeasureString(tm, _text.c_str() + iStart, iCount);

        info.curMaxWidth += tm.size.cx;

        //
        // 给出的宽度不够
        //
        if (tm.charCount == 0)
        {
            if (info.currWidth == info.trimWidth)
            {
                bFinish = true;
                tm.charCount = _text.Length();
                tm.size.cx = info.trimWidth;
            }
            // 换行
            else
            {
                bNewLine = true;
                info.currWidth = info.trimWidth;
                continue;
            }
        }

        if (bNewLine)
        {
            bNewLine = false;
            info.lineTrm = new LineTrim();
            info.maxWidth = max(info.maxWidth, info.curMaxWidth);
            info.curMaxWidth = 0;
            info.textObj->AddLineTrim(info.lineTrm);
        }

        linePart = new LinePart();
        linePart->start = iStart;
        TmAndInfoToLinePart();

        info.lineTrm->AddLinePart(linePart);

        iStart += tm.charCount;

        if (iStart == _text.Length())
        {
            info.currWidth -= tm.size.cx;
            break;
        }
        // 这里不自动换行
        else if (info.txtWrapping == suic::TextWrapping::tNoWrap)
        {
            linePart = new LinePart();
            linePart->start = iStart;
            tm.size.cx = info.trimWidth - info.currWidth;
            tm.charCount = 1;
            TmAndInfoToLinePart();
            info.currWidth = 0;
            break;
        }
        else
        {
            bNewLine = true;
            info.currWidth = info.trimWidth;
            info.currHeight += info.lineTrm->GetLineHeight();
        }
    }

#undef TmAndInfoToLinePart
}

void Run::Render(LineDrawing& ldraw)
{
    suic::FormattedText tra;
    suic::TextTrimming txtTrim = ldraw.textBlock->GetTextTrimming();

    tra.SetFontSize(GetRealFontSize());
    tra.SetSingleline(true);
    tra.SetFontWeight(GetFontWeight());
    tra.SetFontFamily(GetFontFamily());
    tra.SetForeground(GetForeground());

    if (_textDecoration == suic::TextDecorations::underline)
    {
        tra.SetUnderline(true);
    }
    else if (_textDecoration == suic::TextDecorations::strikethrough)
    {
        tra.SetStrikeout(true);
    }

    suic::Brush* brush = GetBackground();
    if (NULL != brush)
    {
        ldraw.drawing->DrawRect(DrawCtx::DefDraw, brush, NULL, &ldraw.rect);
    }

    if (0 != txtTrim)
    {
        if ((int)ldraw.rect.right > ldraw.right)
        {
            ldraw.rect.right = ldraw.right;
            tra.SetTextTrimming(txtTrim);
        }
    }
    
    ldraw.drawing->DrawString(&tra, _text.c_str() + ldraw.start, ldraw.count, &ldraw.rect);
}

//=================================================
// Run
Span::Span()
    : _inlines(NULL)
{
}

Span::~Span()
{
    if (NULL != _inlines)
    {
        delete _inlines;
    }
}

int Span::GetCount()
{
    if (_inlines == NULL)
    {
        return 0;
    }
    else
    {
        return _inlines->GetCount();
    }
}

Inline* Span::GetInline(int index)
{
    return GetInlines()->GetChild(index);
}

void Span::AddText(suic::String text)
{
    Run* item = new Run(text);
    AddChild(item);
}

void Span::AddChild(Object* child)
{
    GetInlines()->AddChild(child);
}

void Span::RemoveChild(Object* child)
{
    GetInlines()->RemoveChild(child);
}

InlineCollection* Span::GetInlines()
{
    if (NULL == _inlines)
    {
        _inlines = new InlineCollection(this);
    }
    return _inlines;
}

LineBreak::~LineBreak()
{

}

void LineBreak::Measure(LineMeasure& info)
{
    static String LINEBREAK = _U("L");
    suic::FormattedText tra;
    suic::Size size;
    suic::TmParam tm;

    tra.SetFontSize(GetRealFontSize());
    tra.SetSingleline(true);
    tra.SetFontWeight(GetFontWeight());
    tra.SetFontFamily(GetFontFamily());

    tra.MeasureString(tm, LINEBREAK.c_str(), LINEBREAK.Length());

    info.lineTrm->SetLineHeight(tm.size.cy);
}

//=================================================
// Bold

Bold::Bold()
{
    SetFontWeight(800);
}

Bold::~Bold()
{
}

void Bold::StaticInit()
{
    static bool s_init = false;
    if (!s_init)
    {
        s_init = true;
        Uint32 flag = PropMetadataOptions::AffectsParentMeasure | PropMetadataOptions::AffectsParentRender | PropMetadataOptions::AffectsInherits;
        TextElement::FontWeightProperty->OverrideMetadata(RTTIType(), suic::DpPropMemory::GetPropMeta(new suic::Integer(suic::FontWeightStyle::fwExtraBold), flag));
    }
}

//=================================================
// Hyperlink

suic::RoutedEvent* Hyperlink::ClickEvent;
suic::RoutedEvent* Hyperlink::RequestNavigateEvent;

suic::DpProperty* Hyperlink::NavigateUriProperty;
suic::DpProperty* Hyperlink::IsHyperlinkPressedProperty;

Hyperlink::Hyperlink()
{
}

Hyperlink::~Hyperlink()
{
}

void Hyperlink::StaticInit()
{
    if (ClickEvent == NULL)
    {
        Inline::StaticInit();
        CursorProperty->OverrideMetadata(RTTIType(), suic::DpPropMemory::GetPropMeta(suic::OCursor::Hand));

        ClickEvent = suic::EventHelper::RegisterRoutedEvent(_U("Click"), suic::RoutingStrategy::Bubble, suic::RoutedEventHandler::RTTIType(), RTTIType());
        IsHyperlinkPressedProperty = suic::DpProperty::Register(_T("IsHyperlinkPressed"), RTTIType(), suic::Boolean::RTTIType(), suic::DpPropMemory::GetPropMeta(suic::Boolean::False));

        NavigateUriProperty = suic::DpProperty::Register(_T("NavigateUri"), RTTIType(), suic::OString::RTTIType(), suic::DpPropMemory::GetPropMeta(suic::OString::EmptyString));
    }
}

void Hyperlink::SetIsHyperlinkPressed(bool pressed)
{
    if (pressed)
    {
        SetValue(IsHyperlinkPressedProperty, BOOLTOBOOLEAN(pressed));
    }
    else
    {
        ClearValue(IsHyperlinkPressedProperty);
    }
}

bool Hyperlink::IsHyperlinkPressed()
{
    return GetValue(IsHyperlinkPressedProperty)->ToBool();
}

suic::String Hyperlink::GetNavigateUri()
{
    return GetValue(NavigateUriProperty)->ToString();
}

void Hyperlink::SetNavigateUri(suic::String uri)
{
    SetValue(NavigateUriProperty, new suic::OString(uri));
}

void Hyperlink::AddClick(suic::RoutedEventHandler* handler)
{
    AddHandler(ClickEvent, handler);
}

void Hyperlink::RemoveClick(suic::RoutedEventHandler* handler)
{
    RemoveHandler(ClickEvent, handler);
}

void Hyperlink::OnClick()
{
    suic::RoutedEventArg e(this, ClickEvent);
    RaiseEvent(&e);
    suic::CommandOp::ExecuteCommand(GetCommandSource());
}

void Hyperlink::OnMouseLeftButtonUp(suic::MouseButtonEventArg* e)
{
    ReleaseCaptureMouse();
    SetIsHyperlinkPressed(false);
    e->SetHandled(true);

    //if (HitTestMouse(e->GetMousePoint()))
    {
        OnClick();
    }
    Span::OnMouseLeftButtonUp(e);
}

void Hyperlink::OnMouseLeftButtonDown(suic::MouseButtonEventArg* e)
{
    e->SetHandled(true);
    Focus();

    if (suic::MouseDevice::GetLeftButton() == suic::MouseButtonState::mbMousePress)
    {
        SetCaptureMouse();
        if (!IsHyperlinkPressed())
        {
            SetIsHyperlinkPressed(true);
        }
    }

    Span::OnMouseLeftButtonDown(e);
}

//=================================================
// InlineUIContainer

InlineUIContainer::InlineUIContainer()
    : _index(-1)
    , _child(NULL)
{

}

InlineUIContainer::~InlineUIContainer()
{
}

suic::Element* InlineUIContainer::GetChild() const
{
    return _child; 
}

void InlineUIContainer::AddChild(suic::Object* child)
{
    if (child != NULL)
    {
        child->ref();
        SetChild(suic::DynamicCast<suic::Element>(child));
        child->unref();
    }
}

void InlineUIContainer::RemoveChild(Object* child)
{
    SetChild(NULL);
}

void InlineUIContainer::SetChild(suic::Element* child)
{
    if (child != NULL)
    {
        child->ref();
    }

    if (_child != NULL)
    {
        RemoveVisualChild(_child);
        _child->unref();
    }

    _child = child;
    AddVisualChild(_child);
}

int InlineUIContainer::GetVisualChildrenCount()
{
    if (NULL == _child)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

suic::Element* InlineUIContainer::GetVisualChild(int index)
{
    return _child;
}

void InlineUIContainer::Measure(LineMeasure& info)
{
    suic::Size constaint;
    LinePart* linePart = NULL;

    if (NULL != _child)
    {
        constaint.cx = suic::Numeric::MeasureInt;
        constaint.cy = suic::Numeric::MeasureInt;

        _child->Measure(constaint);

        if (_child->GetDesiredSize().Width() > info.currWidth && 
            info.currWidth < info.trimWidth && 
            info.txtWrapping != suic::TextWrapping::tNoWrap)
        {
            info.lineTrm = new LineTrim();
            info.currWidth = info.trimWidth;
            info.textObj->AddLineTrim(info.lineTrm);
        }

        linePart = new LinePart();
        linePart->start = 0;
        linePart->count = 1;
        linePart->offset = info.trimWidth -info.currWidth;
        linePart->width = _child->GetDesiredSize().Width();
        linePart->height = _child->GetDesiredSize().Height();
        linePart->lineObj = this;

        info.lineTrm->SetHasUIContainer(true);
        info.lineTrm->AddLinePart(linePart);
        info.textObj->AddComplexElement(this);

        info.curMaxWidth += linePart->width;
        info.currWidth -= linePart->width;

        if (info.currWidth <= 0)
        {
            info.currWidth = 0;
        }
    }
}

void InlineUIContainer::Render(LineDrawing& drawing)
{
}

//=================================================
// TextBlock

TextBlock::TextBlock()
    : _inlines(NULL)
{
}

TextBlock::~TextBlock()
{
    ClearComplexElement();
    ClearSeqLines();

    if (NULL != _inlines)
    {
        delete _inlines;
    }
}

void TextBlock::OnTextPropChanged(suic::DpObject* d, suic::DpPropChangedEventArg* e)
{
    TextBlock* pText = suic::DynamicCast<TextBlock>(d);
    suic::String strText = e->GetNewValue()->ToString();

    if (NULL != pText)
    {
        pText->Clear();
        if (!strText.Empty())
        {
            TextBlockRun* txtBlkRun = new TextBlockRun(strText);
            pText->GetInlines()->AddChild(txtBlkRun);
            txtBlkRun->SetTextDecorations(pText->GetTextDecorations());
        }
    }
}

void TextBlock::StaticInit()
{
    if (NULL == TextProperty)
    {
        Run::StaticInit();
        suic::TextEditor::StaticInit();

        //TextDecorations属性可以设置为：OverLine, Strikethrough, Baseline, Underline或None
        BackgroundProperty = suic::TextElement::BackgroundProperty->AddOwner(RTTIType());
        FontFamilyProperty = suic::TextElement::FontFamilyProperty->AddOwner(RTTIType());
        FontSizeProperty = suic::TextElement::FontSizeProperty->AddOwner(RTTIType());
        FontStretchProperty = suic::TextElement::FontStretchProperty->AddOwner(RTTIType());
        FontStyleProperty = suic::TextElement::FontStyleProperty->AddOwner(RTTIType());
        FontWeightProperty = suic::TextElement::FontWeightProperty->AddOwner(RTTIType());
        ForegroundProperty = suic::TextElement::ForegroundProperty->AddOwner(RTTIType());

        LineHeightProperty = suic::DpProperty::Register(_T("LineHeight"), RTTIType(), suic::OFloat::RTTIType(), suic::DpPropMemory::GetPropMeta(suic::OFloat::GetZeroFloat(), suic::PropMetadataOptions::AffectsMeasure));
        LineSpacingProperty = suic::DpProperty::Register(_T("LineSpacing"), RTTIType(), suic::OFloat::RTTIType(), suic::DpPropMemory::GetPropMeta(suic::OFloat::GetZeroFloat(), suic::PropMetadataOptions::AffectsMeasure));

        TextAlignmentProperty = suic::DpProperty::Register(_T("TextAlignment"), RTTIType(), suic::Integer::RTTIType(), suic::DpPropMemory::GetPropMeta(suic::Integer::GetZeroInt(), suic::PropMetadataOptions::AffectsParentMeasure));

        TextDecorationsProperty = suic::Inline::TextDecorationsProperty->AddOwner(RTTIType());
        TextEffectsProperty = suic::TextElement::TextEffectsProperty->AddOwner(RTTIType());
        PaddingProperty = suic::DpProperty::Register(_T("Padding"), RTTIType(), suic::ORect::RTTIType(), suic::DpPropMemory::GetPropMeta(suic::ORect::GetEmpty(), suic::PropMetadataOptions::AffectsParentMeasure));
        TextProperty = suic::DpProperty::Register(_T("Text"), RTTIType(), suic::OString::RTTIType(), suic::DpPropMemory::GetPropMeta(suic::OString::EmptyString, suic::PropMetadataOptions::AffectsParentMeasure, &TextBlock::OnTextPropChanged));
        TextTrimmingProperty = suic::DpProperty::Register(_T("TextTrimming"), RTTIType(), suic::Integer::RTTIType(), suic::DpPropMemory::GetPropMeta(suic::Integer::GetPosInt(TextTrimming::tCharacterEllipsis), suic::PropMetadataOptions::AffectsParentMeasure));
        TextWrappingProperty = suic::TextEditor::TextWrappingProperty->AddOwner(RTTIType(), suic::DpPropMemory::GetPropMeta(suic::Integer::GetZeroInt(), suic::PropMetadataOptions::AffectsParentMeasure));//suic::DpProperty::Register(_T("TextWrapping"), RTTIType(), suic::Integer::RTTIType(), suic::DpPropMemory::GetPropMeta(suic::Integer::GetZeroInt(), suic::PropMetadataOptions::AffectsParentMeasure));

        FontStyleProperty->SetConvertValueCb(suic::FontStyleConvert::Convert);
        TextTrimmingProperty->SetConvertValueCb(suic::TextTrimmingConvert::Convert);
        TextWrappingProperty->SetConvertValueCb(suic::TextWrappingConvert::Convert);
        TextDecorationsProperty->SetConvertValueCb(suic::TextDecorationsConvert::Convert);
        TextAlignmentProperty->SetConvertValueCb(suic::TextAlignmentConvert::Convert);
    }
}

void TextBlock::Clear()
{
    ClearSeqLines();
    ClearComplexElement();
    GetInlines()->Clear();
}

suic::String TextBlock::GetText()
{
    return GetValue(TextProperty)->ToString();
}

void TextBlock::SetText(suic::String text)
{
    VerifyAccess(__FILELINE__);
    SetValue(TextProperty, new suic::OString(text));
}

suic::String TextBlock::GetFontFamily()
{
    return GetValue(FontFamilyProperty)->ToString();
}

void TextBlock::SetFontFamily(suic::String val)
{
    SetValue(FontFamilyProperty, new suic::OString(val));
}

int TextBlock::GetFontSize()
{
    return GetValue(FontSizeProperty)->ToInt();
}

void TextBlock::SetFontSize(int val)
{
    SetValue(FontSizeProperty, new suic::Integer(val));
}

int TextBlock::GetFontWeight()
{
    return GetValue(FontWeightProperty)->ToInt();
}

void TextBlock::SetFontWeight(int val)
{
    SetValue(FontWeightProperty, new suic::Integer(val));
}

int TextBlock::GetFontStyle()
{
    return GetValue(FontStyleProperty)->ToInt();
}

void TextBlock::SetFontStyle(int val)
{
    SetValue(FontStyleProperty, new suic::Integer(val));
}

int TextBlock::GetFontStretch()
{
    return GetValue(FontStretchProperty)->ToInt();
}

void TextBlock::SetFontStretch(int val)
{
    SetValue(FontStretchProperty, new suic::Integer(val));
}

suic::Brush* TextBlock::GetForeground()
{
    return suic::RTTICast<suic::Brush>(GetValue(ForegroundProperty));
}

void TextBlock::SetForeground(suic::Brush* val)
{
    SetValue(ForegroundProperty, val);
}

suic::Brush* TextBlock::GetBackground()
{
    return suic::RTTICast<suic::Brush>(GetValue(BackgroundProperty));
}

void TextBlock::SetBackground(suic::Brush* val)
{
    SetValue(BackgroundProperty, val);
}

suic::TextTrimming TextBlock::GetTextTrimming()
{
    return (suic::TextTrimming)GetValue(TextTrimmingProperty)->ToInt();
}

void TextBlock::SetTextTrimming(suic::TextTrimming val)
{
    SetValue(TextTrimmingProperty, suic::Integer::GetPosInt(val));
}

suic::TextWrapping TextBlock::GetTextWrapping()
{
    return (suic::TextWrapping)GetValue(TextWrappingProperty)->ToInt();
}

void TextBlock::SetTextWrapping(suic::TextWrapping val)
{
    SetValue(TextWrappingProperty, suic::Integer::GetPosInt(val));
}

suic::TextAlignment TextBlock::GetTextAlignment()
{
    return (suic::TextAlignment)GetValue(TextAlignmentProperty)->ToInt();
}

void TextBlock::SetTextAlignment(suic::TextAlignment val)
{
    SetValue(TextAlignmentProperty, suic::Integer::GetPosInt(val));
}

suic::Float TextBlock::GetLineHeight()
{
    return GetValue(LineHeightProperty)->ToFloat();
}

void TextBlock::SetLineHeight(suic::Float val)
{
    SetValue(LineHeightProperty, new suic::OFloat(val));
}

suic::Float TextBlock::GetLineSpacing()
{
    return GetValue(LineSpacingProperty)->ToFloat();
}

void TextBlock::SetLineSpacing(suic::Float val)
{
    SetValue(LineSpacingProperty, new suic::OFloat(val));
}

suic::TextEffectCollection* TextBlock::GetTextEffects()
{
    return dynamic_cast<suic::TextEffectCollection*>(GetValue(TextEffectsProperty));
}

void TextBlock::SetTextEffects(suic::TextEffectCollection* val)
{
    SetValue(TextEffectsProperty, val);
}

suic::TextDecorationCollection* TextBlock::GetTextDecorations()
{
    return dynamic_cast<suic::TextDecorationCollection*>(GetValue(TextDecorationsProperty));
}

void TextBlock::SetTextDecorations(suic::TextDecorationCollection* val)
{
    SetValue(TextDecorationsProperty, val);
}

bool TextBlock::HasComplexContent() const
{
    return (_inlines != NULL && _inlines->GetCount() > 0);
}

InlineCollection* TextBlock::GetInlines()
{
    if (_inlines == NULL)
    {
        _inlines = new InlineCollection(this);
    }

    return _inlines;
}

void TextBlock::AddText(suic::String text)
{
    GetInlines()->AddText(text);
}

void TextBlock::AddChild(suic::Object* child)
{
    GetInlines()->AddChild(child);
}

void TextBlock::RemoveChild(Object* child)
{
    GetInlines()->RemoveChild(child);
}

int TextBlock::GetVisualChildrenCount()
{
    return _complexChild.GetCount();
}

suic::Element* TextBlock::GetVisualChild(int index)
{
    return _complexChild[index];
}

void TextBlock::AddComplexElement(suic::Element* elem)
{
    elem->ref();
    _complexChild.Add(elem);
}

void TextBlock::ClearSeqLines()
{
    for (int i = 0; i < _seqLines.GetCount(); ++i)
    {
        delete _seqLines[i];
    }
    _seqLines.Clear();
}

void TextBlock::ClearComplexElement()
{
    for (int i = 0; i < _complexChild.GetCount(); ++i)
    {
        _complexChild[i]->unref();
    }
    _complexChild.Clear();
}

int TextBlock::GetLineCount() const
{
    return _seqLines.GetCount();
}

LineTrim* TextBlock::GetLine(int index) const
{
    return _seqLines[index];
}

void TextBlock::AddLineTrim(LineTrim* pLine)
{
    _seqLines.Add(pLine);
}

bool TextBlock::HitTestCore(suic::HitTestCtx* hitResult, bool onlySelf)
{
    int iBottom = 0;
    int iLineSpace = (int)GetLineSpacing();
    Point offset = GetCanvasOffset();

    for (int i = 0; i < _seqLines.GetCount(); ++i)
    {
        LineTrim* pLine = _seqLines[i];

        for (int j = 0; j < pLine->GetCount(); ++j)
        {
            suic::Rect rect;
            LinePart* linePart = pLine->GetLinePart(j);

            if (linePart->lineObj != NULL)
            {
                rect.left = linePart->offset + offset.x;
                rect.bottom = iBottom + pLine->GetLineHeight() + offset.y;
                rect.top = rect.bottom - linePart->height;
                rect.right = rect.left + linePart->width;

                if (HitTest(hitResult, rect))
                {
                    Element* parent = linePart->lineObj->GetUIParent();
                    while (parent)
                    {
                        if (parent->GetRTTIType() == Hyperlink::RTTIType())
                        {
                            hitResult->hitRes = parent;
                            hitResult->hitFilter = HitTestFilterAction::Stop;
                            return true;
                        }
                        parent = parent->GetUIParent();
                    }
                    
                    return false;
                }
            }
        }

        iBottom += pLine->GetLineHeight() + iLineSpace;
    }

    return false;
}

void TextBlock::OnRender(suic::Drawing* drawing)
{
    int iBottom = 0;
    int iRight = GetActualWidth();
    int iLineSpace = (int)GetLineSpacing();
    LineDrawing lineDraw;
    suic::TextAlignment textAlign = GetTextAlignment();
    suic::Brush* brush = GetBackground();

    if (NULL != brush)
    {
        suic::fRect rect(0, 0, iRight, GetActualHeight());
        drawing->DrawRect(DrawCtx::DefDraw, brush, NULL, &rect);
    }

    if (_seqLines.GetCount() > 0)
    {
        lineDraw.drawing = drawing;
        lineDraw.textBlock = this;
        lineDraw.right = iRight;

        for (int i = 0; i < _seqLines.GetCount(); ++i)
        {
            int iOffset = 0;
            LineTrim* pLine = _seqLines[i];

            switch (textAlign)
            {
            case suic::TextAlignment::tLeft:
                break;

            case suic::TextAlignment::tRight:
                iOffset = GetActualWidth() - pLine->GetLineWidth();
                break;

            case suic::TextAlignment::tCenter:
                iOffset = (Float) (GetActualWidth() - pLine->GetLineWidth()) / 2.0f;
                break;
            }

            for (int j = 0; j < pLine->GetCount(); ++j)
            {
                LinePart* linePart = pLine->GetLinePart(j);

                lineDraw.start = linePart->start;
                lineDraw.count = linePart->count;

                lineDraw.rect.left = linePart->offset + iOffset;
                lineDraw.rect.bottom = iBottom + pLine->GetLineHeight();
                lineDraw.rect.top = lineDraw.rect.bottom - linePart->height;
                lineDraw.rect.right = lineDraw.rect.left + linePart->width;

                if (linePart->lineObj != NULL)
                {
                    linePart->lineObj->Render(lineDraw);
                }
            }

            iBottom += pLine->GetLineHeight() + iLineSpace;
        }
    }
}

void TextBlock::MeasureInline(Inline* pInline, LineMeasure& lInfo, bool bLast)
{
    int iOffset = lInfo.trimWidth - lInfo.currWidth;
    bool bFinish = false;

    if (pInline->GetRTTIType() == LineBreak::RTTIType())
    {
        lInfo.maxWidth = max(lInfo.maxWidth, lInfo.curMaxWidth);
        lInfo.curMaxWidth = 0;

        lInfo.currHeight += lInfo.lineTrm->GetLineHeight();
        lInfo.lineTrm = new LineTrim();

        pInline->Measure(lInfo);
        AddLineTrim(lInfo.lineTrm);

        lInfo.currWidth = lInfo.trimWidth;
    }
    else if (lInfo.currWidth > 0)
    {
        pInline->Measure(lInfo);
    }
}

void TextBlock::MeasureInlineChildren(Inline* pInline, LineMeasure& lInfo)
{
    if (!lInfo.lastItem)
    {
        int iCount = pInline->GetCount();

        for (int i = 0; i < iCount; ++i)
        {
            Inline* lineObj = pInline->GetInline(i);

            if (lineObj->GetCount() == 0)
            {
                MeasureInline(lineObj, lInfo, i == iCount - 1);
            }
            else
            {
                MeasureInlineChildren(lineObj, lInfo);
            }
        }
    }
}

suic::Size TextBlock::OnMeasure(const suic::Size& constraint)
{
    //
    // 作为一般的Text对待
    //
    if (GetInlines()->GetCount() == 0)
    {
        return suic::Size();
    }

    suic::Size measureSize;
    int iCount = 0;
    LineMeasure lInfo;
    InlineCollection* lineColls = NULL;
    int iLineSpace = (int)GetLineSpacing();

    lInfo.trimWidth = constraint.cx;
    lInfo.txtWrapping = GetTextWrapping();
    lInfo.txtTrimming = GetTextTrimming();

    lInfo.currWidth = lInfo.trimWidth;
    lInfo.trimHeight = constraint.cy;

    lInfo.lineTrm = new LineTrim();
    lInfo.textObj = this;

    lineColls = GetInlines();
    iCount = lineColls->GetCount();

    ClearSeqLines();
    _seqLines.Add(lInfo.lineTrm);
    ClearComplexElement();

    for (int i = 0; i < iCount; ++i)
    {
        Inline* pInline = lineColls->GetChild(i);

        if (pInline->GetCount() == 0)
        {
            MeasureInline(pInline, lInfo, i == iCount - 1);
        }
        else
        {
            MeasureInlineChildren(pInline, lInfo);
        }
    }

    lInfo.maxWidth = max(lInfo.maxWidth, lInfo.curMaxWidth);
    lInfo.maxWidth = ceil(lInfo.maxWidth);

    if (lInfo.txtWrapping == suic::TextWrapping::tNoWrap)
    {
        lInfo.maxWidth = min(lInfo.maxWidth, constraint.cx);
    }

    measureSize.cx = lInfo.maxWidth;

    for (int j = 0; j < _seqLines.GetCount(); ++j)
    {
        measureSize.cy += _seqLines[j]->GetLineHeight() + iLineSpace;
    }

    return measureSize;
}

void TextBlock::OnArrange(const suic::Size& arrangeSize)
{
    int iBottom = 0;
    int iLineSpace = (int)GetLineSpacing();

    for (int i = 0; i < _seqLines.GetCount(); ++i)
    {
        LineTrim* pLine = _seqLines[i];

        if (pLine->HasUIContainer())
        {
            for (int j = 0; j < pLine->GetCount(); ++j)
            {
                suic::Rect finalRect;
                LinePart* linePart = pLine->GetLinePart(j);
                InlineUIContainer* inlineUI = suic::RTTICast<InlineUIContainer>(linePart->lineObj);

                if (NULL != inlineUI && inlineUI->GetChild() != NULL)
                {
                    finalRect.left = linePart->offset;
                    finalRect.bottom = iBottom + pLine->GetLineHeight();
                    finalRect.top = finalRect.bottom - linePart->height;
                    finalRect.right = finalRect.left + linePart->width;

                    inlineUI->Arrange(finalRect);
                }
            }
        }

        iBottom += pLine->GetLineHeight() + iLineSpace;
    }
}

}
