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

// ժҪ:
//     ָ���� GridSplitter �ؼ��������С���л��С�
enum GridResizeBehavior
{
    // ժҪ:
    //     ������FrameworkElement.HorizontalAlignment ��FrameworkElement.VerticalAlignment
    //     ���Ե�ֵ���·���ռ䡣
    grbBasedOnAlignment = 0,
    //
    // ժҪ:
    //     ����ˮƽControls.GridSplitter������ΪControls.GridSplitter
    //     ָ����������������һ��֮�����·���ռ䡣 ���ڴ�ֱControls.GridSplitter������ΪControls.GridSplitter
    //     ָ������������Ҳ��һ��֮�����·���ռ䡣
    grbCurrentAndNext = 1,
    //
    // ժҪ:
    //     ����ˮƽControls.GridSplitter������ΪControls.GridSplitter
    //     ָ����������������һ��֮�����·���ռ䡣 ���ڴ�ֱControls.GridSplitter������ΪControls.GridSplitter
    //     ָ���������������һ��֮�����·���ռ䡣
    grbPreviousAndCurrent = 2,
    //
    // ժҪ:
    //     ����ˮƽControls.GridSplitter������ΪControls.GridSplitter
    //     ָ����������ĸ������������ĸ���֮�����·���ռ䡣 ���ڴ�ֱControls.GridSplitter������Ϊ
    //    Controls.GridSplitter ָ���������ĸ���������Ҳ�ĸ���֮�����·���ռ䡣
    grbPreviousAndNext = 3,
};

// ժҪ:
//    ָ��GridSplitter �ؼ�������֮�仹������֮�����·���ռ䡣
enum GridResizeDirection
{
    // ժҪ:
    //     ���� SGridSplitter ��FrameworkElement.HorizontalAlignment��FrameworkElement.VerticalAlignment��System.Windows.FrameworkElement.ActualWidth
    //     ��FrameworkElement.ActualHeight ���Ե�ֵ���·���ռ䡣
    grdAuto = 0,
    //
    // ժҪ:
    //     ����֮�����·���ռ䡣
    grdColumns = 1,
    //
    // ժҪ:
    //     ����֮�����·���ռ䡣
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
