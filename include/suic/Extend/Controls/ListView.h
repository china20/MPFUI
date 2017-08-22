// 华勤科技版权所有 2010-2011
// 
// 文件名：ListView.h
// 功  能：列表视图空i吉安。
// 
// 作  者：MPF开发组
// 时  间：2010-11-02
// 
// ============================================================================

#ifndef _UILISTVIEW_H_
#define _UILISTVIEW_H_

#include <Extend/suiwgxconfig.h>
#include <Extend/Controls/ListBox.h>
#include <Framework/Controls/ViewBase.h>

namespace suic
{

class SUIWGX_API ListView : public ListBox
{
public:

    RTTIOfClass(ListView)

    static DpProperty* ViewProperty;

    static void OnViewPropChanged(DpObject* d, DpPropChangedEventArg* e);
    static bool StaticInit();

    ListView();
    ~ListView();

    ViewBase* GetView();
    void SetView(ViewBase* view);

public:

    bool IsItemItsOwnContainer(Object* item);

protected:

    Element* GetContainerForItemOverride();
    void PrepareContainerForItemOverride(DpObject* element, ItemEntry* item);
};

}

#endif
