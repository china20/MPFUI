// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����PasswordBox.h
// ��  �ܣ�ʵ������ؼ���
// 
// ��  �ߣ�Sharpui������
// ʱ  �䣺2010-07-02
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