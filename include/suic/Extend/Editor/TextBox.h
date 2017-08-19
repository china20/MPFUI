// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����TextBox.h
// ��  �ܣ�ʵ���ı��༭��
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2010-08-31
// 
// ============================================================================

#ifndef _UITEXTEDIT_H_
#define _UITEXTEDIT_H_

#include <Extend/Editor/TextBoxBase.h>

namespace suic
{

/// <summary>
///  TextBox��ʵ�ֱ�׼���ı��༭��
/// </summary>
class SUIWGX_API TextBox : public suic::TextBoxBase
{
public:

    RTTIOfClass(TextBox)

    static suic::DpProperty* CharacterCasingProperty;
    static suic::DpProperty* MaxLengthProperty;
    static suic::DpProperty* MaxLinesProperty;
    static suic::DpProperty* MinLinesProperty;
    static suic::DpProperty* TextAlignmentProperty;
    static suic::DpProperty* TextDecorationsProperty;

    static void OnMaxLengthPropChanged(suic::DpObject* d, suic::DpPropChangedEventArg* e);
    static void OnMaxLinesPropChanged(suic::DpObject* d, suic::DpPropChangedEventArg* e);
    static void OnMinLinesPropChanged(suic::DpObject* d, suic::DpPropChangedEventArg* e);
    static void OnTextAlignmentPropChanged(suic::DpObject* d, suic::DpPropChangedEventArg* e);

    static bool StaticInit();

    TextBox();
    virtual ~TextBox();

    int GetMaxLines();
    void SetMaxLines(int val);

    int GetMinLines();
    void SetMinLines(int val);

    suic::TextAlignment GetTextAlignment();
    void SetTextAlignment(suic::TextAlignment val);

protected:

    void InitTextHost();
    void OnTextWrappingChanged(suic::DpPropChangedEventArg* e);
};

typedef suic::shared<TextBox> TextBoxPtr;

}

#endif
