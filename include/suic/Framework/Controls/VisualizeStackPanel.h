// 华勤科技版权所有 2008-2022
// 
// 文件名：VisualizeStackPanel.h
// 功  能：虚拟列表。
// 
// 作  者：MPF开发组
// 时  间：2011-07-02
// 
// ============================================================================


#ifndef _UIVISUALIZINGSTACKPANEL_H_
#define _UIVISUALIZINGSTACKPANEL_H_

#include <Framework/Controls/VisualizePanel.h>

namespace suic
{

class ItemsControl;

class SUICORE_API IProvideStackingSize
{
public:

    virtual ~IProvideStackingSize() {}
    virtual Size EstimatedContainerSize(bool isHorizontal) = 0;
    virtual int GetHeaderSize(bool isHorizontal) = 0;
};

/// <summary>
/// VirtualizingStackPanel
/// </summary>
class SUICORE_API VirtualizingStackPanel : public VirtualizingPanel
{
public:

    static DpProperty* OrientationProperty;

    static void StaticInit();
    static void OnOrientationPropChanged(DpObject* d, DpPropChangedEventArg* e);

    RTTIOfClass(VirtualizingStackPanel)

    VirtualizingStackPanel();
    virtual ~VirtualizingStackPanel();
    
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

    void ClearAllContainers(ItemsControl* itemsControl);

    int GetVisibleStart() const;

public:

    int GetVisibleChildrenCount();
    void InvalidateChildrenResourceReference();

    Size OnMeasure(const Size& constraintSize);
    void OnArrange(const Size& arrangesize);
    
protected:

    virtual void OnItemsChangedInternal(Object* sender, ItemsChangedEventArg* e);
    
    Size MeasureCommon(const Size& constraint);
    void HandleMoreContainer();
    void AdjustViewportOffset(MeasureData& measureData, ItemsControl* itemsControl, bool isHori);

    void ResetChildMeasureData(MeasureData* childMeasureData, const MeasureData& measureData, Size layoutSize, Size stackSize, bool bHori);
    int CalcFirstVisibleItemOffset(ItemsControl* itemsOwner, MeasureData& measureData, bool bHori, int& firstItemOffset, Size& stackOffset);

protected:

    int _visibleCount;
    int _visibleStart;
    int _visibleOffset;
};

inline int VirtualizingStackPanel::GetVisibleChildrenCount()
{
    return _visibleCount;
}

inline int VirtualizingStackPanel::GetVisibleStart() const
{
    return _visibleStart;
}

inline Orientation VirtualizingStackPanel::GetOrientation()
{
    return (Orientation)GetValue(OrientationProperty)->ToInt();
}

inline int VirtualizingStackPanel::GetLogicalOrientation()
{
    return GetOrientation();
}

inline void VirtualizingStackPanel::SetOrientation(Orientation val)
{
    SetValue(OrientationProperty, OrientationBox::From(val));
}

}

#endif
