// 华勤科技版权所有 2010-2011
// 
// 文件名：ListViewItem.h
// 功  能：列表视图控件项。
// 
// 作  者：MPF开发组
// 时  间：2010-11-02
// 
// ============================================================================

#ifndef _UILISTVIEWITEM_H_
#define _UILISTVIEWITEM_H_

#include <Extend/suiwgxconfig.h>
#include <Extend/Controls/ListBoxItem.h>

namespace suic
{

class SUIWGX_API ListViewItem : public ListBoxItem
{
public:

    RTTIOfClass(ListViewItem)

    static bool StaticInit();

    ListViewItem();
    ~ListViewItem();
};

}

#endif
