// 华勤科技版权所有 2010-2022
// 
// 文件名：VisualizeWrapPanel.h
// 功  能：实现虚拟的环绕布局面板。
// 
// 作  者：MPF开发组
// 时  间：2017-09-02
// 
// ============================================================================

#ifndef _VISUALIZEWRAPPANEL_H_
#define _VISUALIZEWRAPPANEL_H_

#include <Extend/suiwgxconfig.h>
#include <Framework/Controls/VisualizePanel.h>

namespace suic
{

class VisualizeWrapPanel : public suic::VirtualizingPanel
{
public:

    static suic::DpProperty* OrientationProperty;

    static void StaticInit();

    RTTIOfClass(VisualizeWrapPanel)

    VisualizeWrapPanel();
    ~VisualizeWrapPanel();

    int GetItemWidth();
    void SetItemWidth(int wid);

    int GetItemHeight();
    void SetItemHeight(int hei);

    suic::Orientation GetOrientation();
    void SetOrientation(suic::Orientation val);
    int GetLogicalOrientation();

protected:

    int GetVisibleChildrenCount();
    int GetVisibleStart() const;
    void InvalidateChildrenResourceReference();

    suic::Size OnMeasure(const suic::Size& constraintSize);
    void OnArrange(const suic::Size& arrangesize);

    void OnKeyDown(suic::KeyboardEventArg* e);

    int ComputeOffsetFromItem(suic::Object* item, int& offset, int& itemSize);
    int ComputeOffsetFromIndex(int index, int& offset, int& itemSize);

    void OnItemsChangedInternal(suic::Object* sender, suic::ItemsChangedEventArg* e);

    void ResetChildMeasureData(suic::MeasureData* childMeasureData, const suic::MeasureData& measureData, suic::Size layoutSize);

protected:

    int _itemWid;
    int _itemHei;

    int _visibleCount;
    int _visibleStart;
    int _visibleOffset;
    int _countPerShow;
};

inline int VisualizeWrapPanel::GetVisibleChildrenCount()
{
    return _visibleCount;
}

inline int VisualizeWrapPanel::GetVisibleStart() const
{
    return _visibleStart;
}

}

#endif
