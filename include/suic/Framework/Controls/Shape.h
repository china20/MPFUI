// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����Shape.h
// ��  �ܣ���״���ࡣ
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2011-07-02
// 
// ============================================================================

#ifndef _UISHAPE_H_
#define _UISHAPE_H_

#include <System/Windows/FrameworkElement.h>

namespace suic
{

class SUICORE_API Shape : public FrameworkElement
{
public:

    // ��״Ч��
    static DpProperty* EffectProperty;
    // ��״����ģʽ
    static DpProperty* StretchProperty;
    // ��״��仭ˢ(Brush����)
    static DpProperty* FillProperty;
    // ��״��߻�ˢ(Brush����)
    static DpProperty* StrokeProperty;
    // ��״�߿�(Integer����)
    static DpProperty* StrokeThicknessProperty;

    static DpProperty* StrokeDashCapProperty;
    static DpProperty* StrokeDashArrayProperty;
    static DpProperty* StrokeDashOffsetProperty;
    static DpProperty* StrokeEndLineCapProperty;
    static DpProperty* StrokeStartLineCapProperty;
    static DpProperty* StrokeLineJoinProperty;
    static DpProperty* StrokeMiterLimitProperty;

    static bool StaticInit();

    Shape();
    virtual ~Shape();

    RTTIOfAbstractClass(Shape);

    int GetStretch();
    void SetStretch(int val);

    Brush* GetFill();
    void SetFill(Brush* val);

    Brush* GetStroke();
    void SetStroke(Brush* val);

    Float GetStrokeThickness();
    void SetStrokeThickness(Float val);

    PenLineCap GetStrokeDashCap();
    FloatAutoArray* GetStrokeDashArray();
    Float GetStrokeDashOffset();
    PenLineCap GetStrokeEndLineCap();
    PenLineJoin GetStrokeLineJoin();
    Float GetStrokeMiterLimit();
    PenLineCap GetStrokeStartLineCap();

protected:

    suic::Size OnMeasure(const suic::Size& size);
    void OnArrange(const suic::Size& arrangeSize);

    void InitPen(Pen* pen, Brush* strokeBrush, int iStrokeThickness);

protected:

    Geometry* _renderedGeometry;
};

inline int Shape::GetStretch()
{
    return GetValue(StretchProperty)->ToInt();
}

inline void Shape::SetStretch(int val)
{
    VerifyAccess(__FILELINE__);
    SetValue(StretchProperty, new Integer(val));
}

inline Brush* Shape::GetFill()
{
    return RTTICast<Brush>(GetValue(FillProperty));
}

inline void Shape::SetFill(Brush* val)
{
    VerifyAccess(__FILELINE__);
    SetValue(FillProperty, val);
}

inline Brush* Shape::GetStroke()
{
    return RTTICast<Brush>(GetValue(StrokeProperty));
}

inline void Shape::SetStroke(Brush* val)
{
    VerifyAccess(__FILELINE__);
    SetValue(StrokeProperty, val);
}

inline Float Shape::GetStrokeThickness()
{
    return GetValue(StrokeThicknessProperty)->ToFloat();
}

inline void Shape::SetStrokeThickness(Float val)
{
    VerifyAccess(__FILELINE__);
    SetValue(StrokeThicknessProperty, new OFloat(val));
}

}

#endif
