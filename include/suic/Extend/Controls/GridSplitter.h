//Created by Wang.xp
//Created Time :2015/09/17

#ifndef _GridSplitter_H_
#define _GridSplitter_H_

#include <Extend/suiwgxconfig.h>
#include <Framework\Controls\Grid.h>
#include <Framework\Controls\Thumb.h>
#include <Framework\Controls\Decorator.h>

namespace suic
{

// 摘要:
//     指定由 GridSplitter 控件调整其大小的行或列。
enum GridResizeBehavior
{
    // 摘要:
    //     将根据FrameworkElement.HorizontalAlignment 和FrameworkElement.VerticalAlignment
    //     属性的值重新分配空间。
    grbBasedOnAlignment = 0,
    //
    // 摘要:
    //     对于水平Controls.GridSplitter，将在为Controls.GridSplitter
    //     指定的行与该行下面的一行之间重新分配空间。 对于垂直Controls.GridSplitter，将在为Controls.GridSplitter
    //     指定的列与该列右侧的一列之间重新分配空间。
    grbCurrentAndNext = 1,
    //
    // 摘要:
    //     对于水平Controls.GridSplitter，将在为Controls.GridSplitter
    //     指定的行与该行上面的一行之间重新分配空间。 对于垂直Controls.GridSplitter，将在为Controls.GridSplitter
    //     指定的列与该列左侧的一列之间重新分配空间。
    grbPreviousAndCurrent = 2,
    //
    // 摘要:
    //     对于水平Controls.GridSplitter，将在为Controls.GridSplitter
    //     指定的行上面的各行与该行下面的各行之间重新分配空间。 对于垂直Controls.GridSplitter，将在为
    //    Controls.GridSplitter 指定的列左侧的各列与该列右侧的各列之间重新分配空间。
    grbPreviousAndNext = 3,
};

// 摘要:
//    指定GridSplitter 控件是在行之间还是在列之间重新分配空间。
enum GridResizeDirection
{
    // 摘要:
    //     根据 SGridSplitter 的FrameworkElement.HorizontalAlignment、FrameworkElement.VerticalAlignment、System.Windows.FrameworkElement.ActualWidth
    //     和FrameworkElement.ActualHeight 属性的值重新分配空间。
    grdAuto = 0,
    //
    // 摘要:
    //     在列之间重新分配空间。
    grdColumns = 1,
    //
    // 摘要:
    //     在行之间重新分配空间。
    grdRows = 2,
};

class GridSplitter;

class PreviewAdorner : public suic::Adorner
{
public:
    
    PreviewAdorner(GridSplitter* gridSplitter, Style* previewStyle);
    ~PreviewAdorner();

    Float GetOffsetX();
    void SetOffsetX(Float val);

    Float GetOffsetY();
    void SetOffsetY(Float val);

protected:

    void OnArrange(const suic::Size& finalSize);
    Element* GetVisualChild(int index);
    int GetVisualChildrenCount();

private:

    suic::Decorator* _decorator;
};

class SUIWGX_API GridSplitter : public suic::Thumb
{
public:

    enum SplitBehavior
    {
        Split,
        Resize1,
        Resize2,
        ResizeAll
    };

    typedef struct tagResizeData
    {
        bool ShowsPreview;
        
        double OffsetX;
        double OffsetY;
        double MinChange;
        double MaxChange;

        Grid* Grid;
        PreviewAdorner* Adorner;
        AdornerLayer* AdornerLay;

        DefinitionBase* Definition1;
        DefinitionBase* Definition2;

        suic::GridSize* OriginalDefinition1Length;
        suic::GridSize* OriginalDefinition2Length;

        GridResizeDirection ResizeDirection;
        GridResizeBehavior ResizeBehavior;
        GridSplitter::SplitBehavior SplitBehavior;

        int SplitterIndex;
        int Definition1Index;
        int Definition2Index;
        
        double OriginalDefinition1ActualLength;
        double OriginalDefinition2ActualLength;
        double SplitterLength;

    }ResizeData;

public:

    GridSplitter();
    ~GridSplitter();

    RTTIOfClass(GridSplitter);

    static void OnDragStartedTrunk(suic::DpObject* sender,suic::RoutedEventArg* e);
    static void OnDragDeltaTrunk(suic::DpObject* sender, suic::RoutedEventArg* e);
    static void OnDragCompletedTrunk(suic::DpObject* sender, suic::RoutedEventArg* e);

    static bool IsStar(DefinitionBase* definition);
    static bool IsAuto(DefinitionBase* definition);
    static void SetDefinitionLength(DefinitionBase* definition, GridSize* length);
    static DefinitionBase* GetGridDefinition(Grid* grid, int index, GridResizeDirection direction);

    static suic::DpProperty* DragIncrementProperty;
    static suic::DpProperty* PreviewStyleProperty;
    static suic::DpProperty* KeyboardIncrementProperty;
    static suic::DpProperty* ResizeBehaviorProperty;
    static suic::DpProperty* ResizeDirectionProperty;
    static suic::DpProperty* ShowsPreviewProperty;

    static bool StaticInit();

public:

    void SetDragIncrement(suic::Float val);
    suic::Float GetDragIncrement();

    suic::Style* GetPreviewStyle();
    void SetPreviewStyle(suic::Style* val);

    void SetKeyboardIncrement(suic::Float val);
    suic::Float GetKeyboardIncrement();

    void SetResizeBehavior(GridResizeBehavior behavior);
    GridResizeBehavior GetResizeBehavior();

    void SetResizeDirection(GridResizeDirection direction);
    GridResizeDirection GetResizeDirection();

    void SetShowsPreview(bool val);
    bool GetShowsPreview();
    void UpdatePreviewPos(int x, int y);

protected:

    void OnDragStarted(suic::RoutedEventArg* e);
    void OnDragDelta(suic::DragDeltaEventArg* e);
    void OnDragCompleted(suic::DragCompletedEventArg* e);

    suic::Grid* GetParentGrid();

private:

    GridResizeDirection GetEffectiveResizeDirection();
    GridResizeBehavior GetEffectiveResizeBehavior(GridResizeDirection direction);
    bool SetupDefinitionsToResize();
    void InitializeData();
    bool IsFirstTamplateParentGrid();

    double GetActualLength(DefinitionBase* definition);
    
    void SetLengths(double definition1Pixels, double definition2Pixels);
    bool MoveSplitter(double horizontalChange, double verticalChange);
    void CancelResize();
    void UpdateGridElement();

    void StartPreview();
    void RemovePreview();

private:

    ResizeData* _resizeData;
    suic::Grid* _parentGrid;
};

inline suic::Float GridSplitter::GetDragIncrement()
{
    return GetValue(DragIncrementProperty) ->ToFloat();
}

inline suic::Float GridSplitter::GetKeyboardIncrement()
{
    return GetValue(KeyboardIncrementProperty)->ToFloat();
}

inline GridResizeBehavior GridSplitter::GetResizeBehavior()
{
    return (GridResizeBehavior)(GetValue(ResizeBehaviorProperty)->ToInt());
}

inline GridResizeDirection GridSplitter::GetResizeDirection()
{
    return (GridResizeDirection)(GetValue(ResizeDirectionProperty)->ToInt());
}

inline	bool GridSplitter::GetShowsPreview()
{
    return GetValue(ShowsPreviewProperty)->ToBool();
}

typedef suic::shared<GridSplitter> GridSplitterPtr;

}

#endif
