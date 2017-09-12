// 华勤科技版权所有 2008-2022
// 
// 文件名：ComboBoxItem.h
// 功  能：实现组合框项逻辑。
// 
// 作  者：MPF开发组
// 时  间：2011-07-02
// 
// ============================================================================


#ifndef _UICOMBOBOXITEM_H_
#define _UICOMBOBOXITEM_H_

#include <Extend/suiwgxconfig.h>
#include <Extend/Controls/ListBoxItem.h>

namespace suic
{

/// <summary>
/// 组合框子项，作为ListBox的项加入
/// </summary>
class SUIWGX_API ComboBoxItem : public ListBoxItem
{
public:

    ComboBoxItem();
    virtual ~ComboBoxItem();

    RTTIOfClass(ComboBoxItem)

protected:

    static void StaticInit();

    void OnMouseMove(MouseButtonEventArg* e);
    void OnMouseLeftButtonUp(MouseButtonEventArg* e);

    friend class ComboBox;
};

}

#endif
