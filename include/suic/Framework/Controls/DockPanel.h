// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����DockPanel.h
// ��  �ܣ�ʵ�ֲ����߼���
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2011-07-02
// 
// ============================================================================

#ifndef _UIDOCKPANEL_H_
#define _UIDOCKPANEL_H_

#include <Framework/Controls/Panel.h>

namespace suic
{

class SUICORE_API DockPanel : public Panel
{
public:

    // ͣ�����ͣ�Integer���ͣ�Left��Top��Right��Bottom��
    static DpProperty* DockProperty;
    // ���һ��Ԫ���Ƿ������ʣ������Boolean���ͣ�
    static DpProperty* LastChildFillProperty;

    DockPanel();

    static bool StaticInit();
    static void OnLastChildFillPropChanged(DpObject* d, DpPropChangedEventArg* e);
    static int GetDock(Element* child);

    RTTIOfClass(DockPanel)

    virtual ~DockPanel();

    bool IsLastChildFill() const;
    void SetIsLastChildFill(bool val);

protected:

    Size OnMeasure(const Size& constraint);
    void OnArrange(const Size& arrangeSize);

protected:

    bool _isLastChildFill;

};

inline bool DockPanel::IsLastChildFill() const
{
    return _isLastChildFill;
}

inline void DockPanel::SetIsLastChildFill(bool val)
{
    SetValue(LastChildFillProperty, BOOLTOBOOLEAN(val));
}

}

#endif
