// 华勤科技版权所有 2008-2022
// 
// 文件名：DockPanel.h
// 功  能：实现布局逻辑。
// 
// 作  者：MPF开发组
// 时  间：2011-07-02
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

    // 停靠类型（Integer类型，Left、Top、Right、Bottom）
    static DpProperty* DockProperty;
    // 最后一个元素是否填充完剩余区域（Boolean类型）
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
