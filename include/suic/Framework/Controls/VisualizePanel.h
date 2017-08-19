// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����VisualizePanel.h
// ��  �ܣ�������岼�ֻ��ࡣ
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2011-07-02
// 
// ============================================================================


#ifndef _UIVISUALIZEPANEL_H_
#define _UIVISUALIZEPANEL_H_

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

    void OnItemsChangedInternal(Object* sender, ItemsChangedEventArg* e);
};

}

#endif
