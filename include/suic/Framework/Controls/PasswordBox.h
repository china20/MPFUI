// 华勤科技版权所有 2010-2011
// 
// 文件名：PasswordBox.h
// 功  能：实现密码控件。
// 
// 作  者：Sharpui开发组
// 时  间：2010-07-02
// 
// ============================================================================

#ifndef _UIPASSWORDBOX_H_
#define _UIPASSWORDBOX_H_

#include <Framework/Controls/Control.h>
#include <Framework/Controls/TextElement.h>

namespace suic
{

class SUICORE_API PasswordBox : public Control
{
public:

    static DpProperty* TextProperty;
    static DpProperty* PasswordCharProperty;

    static bool StaticInit();

    RTTIOfClass(PasswordBox)

    PasswordBox();
    virtual ~PasswordBox();

    String GetPassword();

protected:

    void OnApplyTemplate();

    void OnGotFocus(RoutedEventArg& e);
    void OnKeyDown(KeyboardEventArg& e);

private:

    TextElement* _textEditor;
};

};

#endif