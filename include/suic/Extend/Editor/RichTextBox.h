// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����RichTextEdit.h
// ��  �ܣ�����RichTextBoxʵ�ֱ�׼�ĸ��ı��ؼ�
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2010-08-31
// 
// ============================================================================

#ifndef _UIRICHTEXTBOX_H_
#define _UIRICHTEXTBOX_H_

#include <Extend/Editor/RichTextDef.h>
#include <Extend/Editor/TextBoxBase.h>

namespace suic
{

/// <summary>
///  RichTextBox��ʵ�ֱ�׼��RichTextBox��
/// </summary>
class SUIWGX_API RichTextBox : public suic::TextBoxBase
{
public:

    static bool StaticInit();

    RTTIOfClass(RichTextBox)

    RichTextBox();
    virtual ~RichTextBox();

    ITextDoc* GetTextDoc();
    TextSel* GetSelection();

    bool InsertEmbbed(Embbed* embbed);
    bool InsertEmbbed(suic::Int32 cp, Embbed* embbed);

    bool CharIndexIsObject(int charIndex);

protected:

    void InitTextHost();
    void NotifyTextChanged();
    void OnTextWrappingChanged(suic::DpPropChangedEventArg* e);

private:

    ITextDoc* _textDoc;
};

}

#endif
