// 华勤科技版权所有 2008-2022
// 
// 文件名：TextView.h
// 功  能：定义文本控件基类接口。
// 
// 作  者：MPF开发组
// 时  间：2010-07-02
// 
// ============================================================================

#ifndef _UITEXTVIEW_H_
#define _UITEXTVIEW_H_

#include <System/Windows/FrameworkElement.h>

namespace suic
{

class SUICORE_API TextChangedEventArg : public RoutedEventArg
{
public:

    TextChangedEventArg();
    TextChangedEventArg(String newText);

    String GetText() const;

private:

    String _text;
};

class SUICORE_API IKeboardInputSink
{
public:

    virtual ~IKeboardInputSink() {}

    virtual void OnGotFocus(RoutedEventArg* e) = 0;
    virtual void OnLostFocus(RoutedEventArg* e) = 0;

    virtual void OnTextInput(KeyboardEventArg* e) = 0;
    virtual void OnKeyDown(KeyboardEventArg* e) = 0;
    virtual void OnKeyUp(KeyboardEventArg* e) = 0;
};

}

#endif
