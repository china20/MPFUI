// 华勤科技版权所有 2008-2022
// 
// 文件名：VisualizePanel.h
// 功  能：虚拟面板布局基类。
// 
// 作  者：MPF开发组
// 时  间：2011-07-02
// 
// ============================================================================


#ifndef _UIVISUALIZEPANEL_H_
#define _UIVISUALIZEPANEL_H_

#include <System/Windows/ScrollInfo.h>
#include <Framework/Controls/Panel.h>

namespace suic
{

/// <summary>
/// VisualizePanel
/// </summary>
class SUICORE_API VirtualizingPanel : public Panel
{
public:

    VirtualizingPanel();
    virtual ~VirtualizingPanel();

    RTTIOfAbstractClass(VirtualizingPanel)

    void GenerateChildren();
    void ClearChildren();

    void BringIndexIntoView(int index);

    VirtualizingPanel* Clone();

    virtual void ClearAllContainers(ItemsControl* itemsControl);

protected:

    virtual void OnScrollChange(IScrollInfo* pScrollInfo);
    virtual void OnItemsChangedInternal(Object* sender, ItemsChangedEventArg* e);
    
};

}

#endif
