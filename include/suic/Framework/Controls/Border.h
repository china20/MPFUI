// 华勤科技版权所有 2008-2022
// 
// 文件名：Border.h
// 功  能：定义核心库边框类，封装元素的边框和背景绘制逻辑。
// 
// 作  者：MPF开发组
// 时  间：2010-05-02
// 
// ============================================================================

#ifndef _UIBORDER_H_
#define _UIBORDER_H_

#include <Framework/Controls/Decorator.h>

namespace suic
{

/// <summary>
/// 边框控件
/// </summary>
class SUICORE_API Border : public Decorator
{
public:

    static DpProperty* CornerRadiusProperty;
    static DpProperty* BackgroundProperty;
    static DpProperty* PaddingProperty;
    static DpProperty* BorderBrushProperty;
    static DpProperty* BorderThicknessProperty;

    static bool StaticInit();

    static void OnBackgroundPropChanged(DpObject* d, DpPropChangedEventArg* e);
    static void OnForegroundPropChanged(DpObject* d, DpPropChangedEventArg* e);
    static void OnPaddingPropChanged(DpObject* d, DpPropChangedEventArg* e);
    static void OnBorderBrushPropChanged(DpObject* d, DpPropChangedEventArg* e);
    static void OnBorderThicknessPropChanged(DpObject* d, DpPropChangedEventArg* e);

    Border();
    virtual ~Border();

    RTTIOfClass(Border)

    fRect GetCornerRadius();
    void SetCornerRadius(fRect val);

    Rect GetPadding();
    void SetPadding(Rect val);

    Brush* GetBackground();
    void SetBackground(Brush* val);

    Brush* GetBorderBrush();
    void SetBorderBrush(Brush* val);

    Rect GetBorderThickness();
    void SetBorderThickness(Rect border);

protected:

    void ResetCachedBitmap();
    void UpdateCachedBitmap(Drawing* drawing);

    void RenderChildren(Drawing* drawing);
    bool EnterRenderChildren(Drawing* drawing);
    void ExitRenderChildren(Drawing* drawing);

    bool HitTestCore(HitTestCtx* hitResult, bool onlySelf);

    Size OnMeasure(const Size& availableSize);
    void OnArrange(const Size& arrangeSize);

    void OnRenderSizeChanged(SizeChangedInfo& e);

    void OnRender(Drawing * drawing);

protected:

    Rect _thickness;
    Rect _padding;
    
    Brush* _background;
    Brush* _foreground;
    Brush* _borderBrush;

    Bitmap* _cachedBitmap;
};

}

#endif
