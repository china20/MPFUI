// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����TextBlock.h
// ��  �ܣ�ʵ�ֱ�׼���ļ���ؼ���
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2010-07-02
// 
// ============================================================================

#ifndef _UITEXTBLOCKWGX_H_
#define _UITEXTBLOCKWGX_H_

#include <System/Tools/Array.h>
#include <Framework/Controls/TextElement.h>

namespace suic
{

class LineTrim;
class TextBlock;

struct LineMeasure
{
    // ��ǰ������ƫ��
    //int offset;
    suic::Float trimWidth;
    suic::Float currWidth;

    suic::Float trimHeight;
    suic::Float currHeight;

    suic::Float maxWidth;
    suic::Float curMaxWidth;

    bool lastItem;

    suic::TextWrapping txtWrapping;
    suic::TextTrimming txtTrimming;

    // ����󷵻�ֵ
    LineTrim* lineTrm;
    TextBlock* textObj;
    
    LineMeasure();
    void Clear();
};

struct LineDrawing
{
    int start;
    int count;
    int right;
    suic::fRect rect;
    suic::Drawing* drawing;
    suic::TextBlock* textBlock;

    LineDrawing();
};

class SUICORE_API Inline : public suic::TextElement
                         , public suic::IAddChild
{
public:

    RTTIOfClass(Inline)

    static suic::DpProperty* BaselineAlignmentProperty;
    static suic::DpProperty* FlowDirectionProperty;
    static suic::DpProperty* TextDecorationsProperty;

    static void OnTextDecorationsPropChanged(suic::DpObject* d, suic::DpPropChangedEventArg* e);

    static bool StaticInit();

    Inline();
    ~Inline();

    suic::TextDecorationCollection* GetTextDecorations();
    void SetTextDecorations(suic::TextDecorationCollection* val);

    virtual int GetCount();
    virtual Inline* GetInline(int index);

    virtual void AddText(suic::String text);
    virtual void AddChild(suic::Object* child);
    virtual void RemoveChild(Object* child);

    virtual void Measure(LineMeasure& info);
    virtual void Render(LineDrawing& drawing);

protected:

    suic::String GetText() { return suic::String(); }
    void SetText(suic::String text) {}

protected:

    suic::TextDecorationCollection* _textDecoration;
};

/// <Summary>
///  ����һ���ı���
/// </Summary>
class SUICORE_API Run : public Inline
{
public:

    static suic::DpProperty* TextProperty;

    static void OnTextPropChanged(suic::DpObject* d, suic::DpPropChangedEventArg* e);

    static bool StaticInit();

    RTTIOfClass(Run)

    Run();
    Run(suic::String text);

    virtual ~Run();

    suic::String GetText();
    void SetText(suic::String text);

    void Measure(LineMeasure& info);
    void Render(LineDrawing& ldraw);

protected:

    void AddText(suic::String text);
    void AddChild(suic::Object* child);

private:

    suic::String _text;
};

class SUICORE_API InlineCollection
{
public:

    InlineCollection(suic::Element* p);
    ~InlineCollection();

    suic::DpObject* GetParent() const;

    int GetCount() const;
    Inline* GetChild(int index);

    void AddText(suic::String text);
    void AddChild(suic::Object* child);

    void RemoveAt(int index);
    void RemoveChild(suic::Object* child);

    void Clear();

private:

    void CheckInnerText();

private:

    suic::Array<Inline*> _inlines;
    suic::Element* _parent;
};

inline int InlineCollection::GetCount() const
{
    return _inlines.GetCount(); 
}

inline Inline* InlineCollection::GetChild(int index)
{
    return _inlines[index];
}

// ����������ɫ���ı���
class SUICORE_API Span : public Inline
{
public:

    RTTIOfClass(Span)

    Span();
    virtual ~Span();

    int GetCount();
    Inline* GetInline(int index);

    void AddText(suic::String text);
    void AddChild(suic::Object* child);
    void RemoveChild(Object* child);

    InlineCollection* GetInlines();

private:

    InlineCollection* _inlines;
};

// ����
class SUICORE_API LineBreak : public Inline
{
public:

    RTTIOfClass(LineBreak)

    virtual ~LineBreak();
    void Measure(LineMeasure& info);
};

// ����
class SUICORE_API Bold : public Span
{
public:

    static bool StaticInit();

    RTTIOfClass(Bold)
    
    Bold();
    virtual ~Bold();
};

// б��
class SUICORE_API Italic : public Span
{
public:

    RTTIOfClass(Italic)

    virtual ~Italic() {}
};

// �����»�������
class SUICORE_API Underline : public Span
{
public:

    RTTIOfClass(Underline)

    virtual ~Underline() {}
};

// ������
class SUICORE_API Hyperlink : public Span
{
public:

    static suic::RoutedEvent* ClickEvent;
    static suic::RoutedEvent* RequestNavigateEvent;

    static suic::DpProperty* NavigateUriProperty;
    static suic::DpProperty* IsHyperlinkPressedProperty;

    static bool StaticInit();

    RTTIOfClass(Hyperlink)

    Hyperlink();
    virtual ~Hyperlink();

    bool IsHyperlinkPressed();

    suic::String GetNavigateUri();
    void SetNavigateUri(suic::String uri);

    void AddClick(suic::RoutedEventHandler* handler);
    void RemoveClick(suic::RoutedEventHandler* handler);

protected:

    void OnClick();

    void OnMouseLeftButtonUp(suic::MouseButtonEventArg* e);
    void OnMouseLeftButtonDown(suic::MouseButtonEventArg* e);

private:

    void SetIsHyperlinkPressed(bool pressed);
};

// �ؼ�
class SUICORE_API InlineUIContainer : public Inline
{
public:

    RTTIOfClass(InlineUIContainer)

    InlineUIContainer();
    virtual ~InlineUIContainer();

    void AddChild(suic::Object* child);
    void RemoveChild(Object* child);

    suic::Element* GetChild() const;
    void SetChild(suic::Element* child);

    int GetVisualChildrenCount();
    suic::Element* GetVisualChild(int index);

    void Measure(LineMeasure& info);
    void Render(LineDrawing& drawing);

private:

    int _index;
    suic::Element* _child;
};

struct LinePart
{
    // ��ʼ�ַ�λ��
    int start;
    int count;

    // ��ʼ����λ��
    suic::Float offset;
    suic::Float width;
    suic::Float height;

    Inline* lineObj;

    LinePart();
};

class LineTrim
{
public:

    LineTrim();
    ~LineTrim();

    int GetCount() const;
    LinePart* GetLinePart(int index);

    void AddLinePart(LinePart* linePart);

    void Clear();

    void SetLineHeight(int val);
    int GetLineHeight() const;

    int GetLineWidth() const;

    void SetHasUIContainer(bool val);
    bool HasUIContainer() const;

private:

    // ���и߶�
    int _lineHeight;
    int _lineWidth;
    bool _hasUIContainer;
    suic::Array<LinePart*> _lineParts;
};

inline int LineTrim::GetLineWidth() const
{
    return _lineWidth;
}

class SUICORE_API TextBlock : public suic::FrameworkElement
                            , public suic::IAddChild
{
public:

    static suic::DpProperty* BackgroundProperty;
    static suic::DpProperty* ForegroundProperty;

    static suic::DpProperty* FontFamilyProperty;
    static suic::DpProperty* FontSizeProperty;
    static suic::DpProperty* FontStretchProperty;
    static suic::DpProperty* FontStyleProperty;
    static suic::DpProperty* FontWeightProperty;

    static suic::DpProperty* TextProperty;
    static suic::DpProperty* PaddingProperty;
    static suic::DpProperty* LineHeightProperty;
    static suic::DpProperty* LineSpacingProperty;

    static suic::DpProperty* TextEffectsProperty;
    static suic::DpProperty* TextDecorationsProperty;

    static suic::DpProperty* TextTrimmingProperty;
    static suic::DpProperty* TextWrappingProperty;
    static suic::DpProperty* TextAlignmentProperty;

    static void OnTextPropChanged(suic::DpObject* d, suic::DpPropChangedEventArg* e);

    static bool StaticInit();

    RTTIOfClass(TextBlock)

    TextBlock();
    ~TextBlock();

    suic::String GetText();
    void SetText(suic::String text);

    suic::String GetFontFamily();
    void SetFontFamily(suic::String val);

    int GetFontSize();
    void SetFontSize(int val);

    int GetFontWeight();
    void SetFontWeight(int val);

    int GetFontStyle();
    void SetFontStyle(int val);

    int GetFontStretch();
    void SetFontStretch(int val);

    suic::Brush* GetForeground();
    void SetForeground(suic::Brush* val);

    suic::Brush* GetBackground();
    void SetBackground(suic::Brush* val);

    suic::TextTrimming GetTextTrimming();
    void SetTextTrimming(suic::TextTrimming val);

    suic::Float GetLineHeight();
    void SetLineHeight(suic::Float val);

    suic::Float GetLineSpacing();
    void SetLineSpacing(suic::Float val);

    suic::TextWrapping GetTextWrapping();
    void SetTextWrapping(suic::TextWrapping val);

    suic::TextAlignment GetTextAlignment();
    void SetTextAlignment(suic::TextAlignment val);

    suic::TextEffectCollection* GetTextEffects();
    void SetTextEffects(suic::TextEffectCollection* val);

    suic::TextDecorationCollection* GetTextDecorations();
    void SetTextDecorations(suic::TextDecorationCollection* val);

    int GetLineCount() const;
    LineTrim* GetLine(int index) const;

    void AddLineTrim(LineTrim* pLine);

    bool HasComplexContent() const;
    InlineCollection* GetInlines();

    void AddText(suic::String text);
    void AddChild(suic::Object* child);
    void RemoveChild(Object* child);

    int GetVisualChildrenCount();
    Element* GetVisualChild(int index);

    void AddComplexElement(suic::Element* elem);

    void Clear();

protected:

    void ClearSeqLines();
    void ClearComplexElement();
    bool HitTestCore(suic::HitTestCtx* hitResult, bool onlySelf);

    void OnRender(suic::Drawing* drawing);

    suic::Size OnMeasure(const suic::Size& constraint);
    void OnArrange(const suic::Size& arrangeSize);

    void MeasureInline(Inline* pInline, LineMeasure& lInfo, bool bLast);
    void MeasureInlineChildren(Inline* pInline, LineMeasure& lInfo);

private:

    InlineCollection* _inlines;

    // ������������Ϣ
    suic::Array<LineTrim*> _seqLines;
    suic::Array<suic::Element*> _complexChild;

    // �����ϴβ������ƴ�С
    suic::Size _referenceSize;
};

typedef suic::shared<TextBlock> TextBlockPtr;

}

#endif
