// 华勤科技版权所有 2008-2022
// 
// 文件名：RichTextEdit.h
// 功  能：居于RichTextBox实现标准的富文本控件
// 
// 作  者：MPF开发组
// 时  间：2010-08-31
// 
// ============================================================================

#ifndef _UIRICHTEXTBOX_H_
#define _UIRICHTEXTBOX_H_

#include <Extend/Editor/RichTextDef.h>
#include <Extend/Editor/TextBoxBase.h>

namespace suic
{

/// <summary>
///  RichTextBox，实现标准的RichTextBox。
/// </summary>
class SUIWGX_API RichTextBox : public suic::TextBoxBase
{
public:

    static void StaticInit();

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
