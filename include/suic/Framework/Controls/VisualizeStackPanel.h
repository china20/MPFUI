// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����VisualizeStackPanel.h
// ��  �ܣ������б�
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2011-07-02
// 
// ============================================================================


#ifndef _UIVISUALIZINGSTACKPANEL_H_
#define _UIVISUALIZINGSTACKPANEL_H_

#include <System/Windows/ScrollInfo.h>
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

    static bool StaticInit();
    static void OnOrientationPropChanged(DpObject* d, DpPropChangedEventArg* e);

    RTTIOfClass(VirtualizingStackPanel)

    VirtualizingStackPanel();
    virtual ~VirtualizingStackPanel();

    IScrollInfo* GetScrollInfo();
    ScrollData* GetScrollData();
    bool IsScrolling();

    /// <summary>
    ///  ��ȡ����Ԫ�صķ���
    /// </summary>
    /// <remarks>
    ///  ����Ԫ�صķ����Ϊˮƽ�ʹ�ֱ��Horizontal��Vertical����
    /// </remarks>
    /// <returns>Horizontal: ˮƽ; Vertical: ��ֱ</returns> 
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

    virtual void OnScrollChange();
    virtual void OnViewportSizeChanged(Size oldViewportSize, Size newViewportSize);
    virtual void OnViewportOffsetChanged(Point oldViewportOffset, Point newViewportOffset);
    virtual void OnItemsChangedInternal(Object* sender, ItemsChangedEventArg* e);

    Size MeasureCommon(const Size& constraint);

    void HandleMoreContainer();
    void ClearRealizedContainer();
    void InsertContainer(int index, Element* conainer);
    Element* RecycleContainer(int index, ItemEntry* item);

    void SetAndVerifyScrollingData(Size viewport, Size prevSize, Size extent, Point offset);
    void AdjustViewportOffset(MeasureData& measureData, ItemsControl* itemsControl, bool isHori);

    void ResetChildMeasureData(MeasureData* childMeasureData, const MeasureData& measureData, Size layoutSize, Size stackSize, bool bHori);
    int CalcFirstVisibleItemOffset(ItemsControl* itemsOwner, MeasureData& measureData, bool bHori, int& firstItemOffset, Size& stackOffset);

protected:

    int _realedCount;
    int _visibleCount;
    int _visibleStart;
    int _visibleOffset;
    int _visibleMeasure;

    ScrollInfo* _scrollInfo;
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
