// 华勤科技版权所有 2008-2022
// 
// 文件名：StackPanel.h
// 功  能：栈式面板
// 
// 作  者：MPF开发组
// 时  间：2010-08-31
// 
// =============================================================

#ifndef _UISTACKPANEL_H_
#define _UISTACKPANEL_H_

#include <Framework/Controls/Panel.h>
#include <System/Windows/ScrollInfo.h>

namespace suic
{

/// <summary>
/// StackPanel。
/// </summary>
class SUICORE_API StackPanel : public Panel
{
public:

    static DpProperty* OrientationProperty;

    static void OnOrientationPropChanged(DpObject* d, DpPropChangedEventArg* e);
    static void StaticInit();

    StackPanel();
    virtual ~StackPanel();

    RTTIOfClass(StackPanel)

    /// <summary>
    ///  获取界面元素的方向
    /// </summary>
    /// <remarks>
    ///  界面元素的方向分为水平和垂直（Horizontal和Vertical）。
    /// </remarks>
    /// <returns>Horizontal: 水平; Vertical: 垂直</returns> 
    Orientation GetOrientation();
    void SetOrientation(Orientation val);

    int GetLogicalOrientation();
    int GetVisibleIndex() const;
    int GetVisibleCount() const;
    int GetVisibleOffset() const;

    int GetVisibleChildrenCount();
    Element* GetVisibleChild(int index);
    void NotifyItemChanged();

    int ComputeOffsetFromItem(suic::Object* item, eItemDirection id, int& offset, int& itemSize);

protected:

    IScrollInfo* GetScrollInfo();
    ScrollData* GetScrollData();
    bool IsScrolling();

protected:

    void OnInitialized(EventArg* e);
    void OnItemsChangedInternal(Object* sender, ItemsChangedEventArg* e);

    Size OnMeasure(const Size& constraintSize);
    void OnArrange(const Size& arrangesize);

private:

    void OnScrollChange();
    void EnsureScrollingData(Size viewport, Size extent, Point offset);

private:

    int _visibleIndex;
    int _visibleOffset;
    int _visibleCount;
    ScrollInfo* _scrollInfo;
};

inline Orientation StackPanel::GetOrientation()
{
    return (Orientation)GetValue(OrientationProperty)->ToInt();
}

inline int StackPanel::GetVisibleIndex() const
{
    return _visibleIndex;
}

inline int StackPanel::GetVisibleCount() const
{
    return _visibleCount;
}

inline int StackPanel::GetVisibleOffset() const
{
    return _visibleOffset;
}

inline void StackPanel::SetOrientation(Orientation val)
{
    SetValue(OrientationProperty, OrientationBox::From(val));
}

inline int StackPanel::GetLogicalOrientation()
{
    return GetOrientation();
}

typedef shared<StackPanel> StackPanelPtr;

}

#endif
