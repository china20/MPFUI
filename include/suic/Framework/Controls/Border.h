// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����Border.h
// ��  �ܣ�������Ŀ�߿��࣬��װԪ�صı߿�ͱ��������߼���
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2010-05-02
// 
// ============================================================================

#ifndef _UIBORDER_H_
#define _UIBORDER_H_

#include <Framework/Controls/Decorator.h>

namespace suic
{

/// <summary>
/// �߿�ؼ�
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
