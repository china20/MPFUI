
#ifndef _UITEXTRENDERHOST_H_
#define _UITEXTRENDERHOST_H_

#include <Extend/RichText/RichTextDef.h>

#include <System/Windows/TextInput.h>
#include <System/Windows/ScrollInfo.h>
#include <System/Windows/FrameworkElement.h>
#include <Framework/Controls/CaretElement.h>
#include <Framework/Controls/ScrollViewer.h>

#include <Extend/RichText/TextBoxBase.h>
#include <Extend/RichText/PasswordBox.h>

using namespace suic;
class TextRenderHost;

namespace sui
{

class TextControl : public suic::FrameworkElement
{
public:

    RTTIOfClass(TextControl)

    TextControl();
    ~TextControl();

    TextRenderHost* GetTextRender() const;
    suic::IScrollInfo* GetScrollInfo();
    suic::ScrollViewer* GetScrollViewer() const;
    suic::CaretElement* GetCaret() const;
    ITextCallback* GetTextCallback() const;

    suic::String GetText();
    void SetText(suic::String text);

    bool GetReadOnly();
    void SetReadOnly(bool val);

    bool GetEnabled();
    bool GotFocus();
    void RefleshVisual();

    void SetAcceptsTab(bool val);

public:

    void UpdateScrollPos(bool bHori, int pos, bool bRedraw);
    void UpdateScrollRange(bool bHori, int minPos, int maxPos, bool bRedraw);

public:

    virtual void ShowCaret(bool bShow);
    virtual void InitCaret(int w, int h);
    virtual void ResetCaret(suic::Point point);

protected:

    int GetVisualChildrenCount();
    suic::Element* GetVisualChild(int index);

    void InvalidateScrollInfo();
    bool CallMessage(suic::Uint32 message, suic::Uint32 wp, suic::Uint32 lp, suic::Uint64* lr);

    suic::Size OnMeasure(const suic::Size& constraint);
    suic::Size OnArrange(const suic::Size& arrangeSize);

    void OnRender(suic::Drawing * drawing);
    void OnRenderSizeChanged(suic::SizeChangedInfo& e);

    void OnSetCursor(suic::CursorEventArg* e);
    
    void OnTextInput(suic::KeyboardEventArg* e);
    void OnKeyDown(suic::KeyboardEventArg* e);
    void OnKeyUp(suic::KeyboardEventArg* e);

    bool OnEvent(suic::Object* sender, suic::MessageParam* mp);

    void OnLoaded(suic::LoadedEventArg* e);
    void OnUnloaded(suic::LoadedEventArg* e);

    void OnGotFocus(suic::RoutedEventArg* e);
    void OnLostFocus(suic::RoutedEventArg* e);

    void OnMouseEnter(suic::MouseButtonEventArg* e);
    void OnMouseMove(suic::MouseButtonEventArg* e);
    void OnMouseLeave(suic::MouseButtonEventArg* e);
    void OnMouseLeftButtonDown(suic::MouseButtonEventArg* e);
    void OnMouseLeftButtonDbclk(suic::MouseButtonEventArg* e);
    void OnMouseLeftButtonUp(suic::MouseButtonEventArg* e);

    friend class sui::TextBoxBase;
    friend class sui::PasswordBox;

private:

    ITextCallback* _owner;
    suic::CaretElement* _caret;
    suic::ScrollInfo* _scrollInfo;
    TextRenderHost* _renderHost;
};

inline suic::IScrollInfo* TextControl::GetScrollInfo()
{
    if (NULL == _scrollInfo)
    {
        _scrollInfo = new suic::ScrollInfo();
    }
    return _scrollInfo;
}

inline suic::ScrollViewer* TextControl::GetScrollViewer() const
{
    if (NULL == _scrollInfo)
    {
        return NULL;
    }
    else
    {
        return _scrollInfo->GetScrollOwner();
    }
}

inline suic::CaretElement* TextControl::GetCaret() const
{
    return _caret;
}

}

#endif
