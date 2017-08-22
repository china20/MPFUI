
#ifndef _UITEXTCONTAINER_H_
#define _UITEXTCONTAINER_H_

#include <Extend/Editor/RichTextDef.h>

#include <System/Windows/TextInput.h>
#include <System/Windows/ScrollInfo.h>
#include <System/Windows/FrameworkElement.h>
#include <Framework/Controls/CaretElement.h>
#include <Framework/Controls/ScrollViewer.h>

namespace suic
{

class SUIWGX_API TextContainer : public suic::FrameworkElement
{
public:

    RTTIOfClass(TextContainer)

    TextContainer();
    ~TextContainer();

    ITextRender* GetTextRender() const;
    suic::IScrollInfo* GetScrollInfo();
    suic::ScrollViewer* GetScrollViewer() const;
    suic::CaretElement* GetCaretElement() const;

    Embbed* FindEmbbed(int cp);
    Embbed* HitTestEmbbed(suic::Point point);

    suic::String GetText();
    void SetText(suic::String text);

    bool GetReadOnly();
    void SetReadOnly(bool val);

    bool GetEnabled();
    bool GotFocus();

    void RefleshVisual();
    void UpdateView();

    void SetAcceptsTab(bool val);

public:

    void UpdateScrollPos(bool bHori, int pos, bool bRedraw);
    void UpdateScrollRange(bool bHori, int minPos, int maxPos, bool bRedraw);

public:

    virtual void ShowCaret(bool bShow);
    virtual void InitCaret(int w, int h);
    virtual void ResetCaret(suic::Point point);

protected:

    void SetTextCallback(suic::ITextCallback* textCb);
    void SetLineLeftOffset(int index, int leftOffset);

    int GetVisualChildrenCount();
    suic::Element* GetVisualChild(int index);

    void UpdateViewRect();
    void InvalidateScrollInfo(bool bRaised);
    bool DoMessage(suic::Uint32 message, suic::Uint32 wp, suic::Uint32 lp, suic::Uint64* lr);

    suic::Size OnMeasure(const suic::Size& constraint);
    void OnArrange(const suic::Size& arrangeSize);

    void OnRender(suic::Drawing * drawing);
    void OnRenderSizeChanged(suic::SizeChangedInfo& e);

    void OnSetCursor(suic::CursorEventArg* e);

    void OnTextInput(suic::KeyboardEventArg* e);
    void OnImeEvent(suic::MessageEventArg* e);
    void OnKeyDown(suic::KeyboardEventArg* e);
    void OnKeyUp(suic::KeyboardEventArg* e);

    void OnLoaded(suic::LoadedEventArg* e);
    void OnUnloaded(suic::LoadedEventArg* e);

    void OnLostFocus(suic::RoutedEventArg* e);
    void OnLostKeyboardFocus(suic::KeyboardFocusEventArg* e);

    void OnMouseEnter(suic::MouseButtonEventArg* e);
    void OnMouseMove(suic::MouseButtonEventArg* e);
    void OnMouseLeave(suic::MouseButtonEventArg* e);
    void OnMouseLeftButtonDown(suic::MouseButtonEventArg* e);
    void OnMouseLeftButtonDbclk(suic::MouseButtonEventArg* e);
    void OnMouseLeftButtonUp(suic::MouseButtonEventArg* e);

private:

    ITextRender* _textRender;
    suic::Element* _hostElem;
    suic::CaretElement* _caret;
    suic::ScrollInfo* _scrollInfo;

    friend class TextBoxBase;
    friend class PasswordBox;
};

inline suic::IScrollInfo* TextContainer::GetScrollInfo()
{
    if (NULL == _scrollInfo)
    {
        _scrollInfo = new suic::ScrollInfo();
    }
    return _scrollInfo;
}

inline suic::ScrollViewer* TextContainer::GetScrollViewer() const
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

inline suic::CaretElement* TextContainer::GetCaretElement() const
{
    return _caret;
}

}

#endif
