// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����ListView.h
// ��  �ܣ��б���ͼ��i������
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2010-11-02
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
