// 华勤科技版权所有 2008-2022
// 
// 文件名：CheckBox.h
// 功  能：按钮基类
// 
// 作  者：MPF开发组
// 时  间：2010-08-31
// 
// =============================================================

#ifndef _UICHECKBOX_H_
#define _UICHECKBOX_H_

#include <Framework/Controls/ToggleButton.h>

namespace suic
{

/// <summary>
/// CheckBox。
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
