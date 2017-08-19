// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����CheckBox.h
// ��  �ܣ���ť����
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2010-08-31
// 
// =============================================================

#ifndef _UICHECKBOX_H_
#define _UICHECKBOX_H_

#include <Framework/Controls/ToggleButton.h>

namespace suic
{

/// <summary>
/// CheckBox��
/// </summary>
class SUICORE_API CheckBox : public ToggleButton
{
public:

    static bool StaticInit();

    CheckBox();
    virtual ~CheckBox();

    RTTIOfClass(CheckBox)
};

typedef shared<CheckBox> CheckBoxPtr;

}

#endif
