// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����Rectangle.h
// ��  �ܣ����οؼ���
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2011-07-02
// 
// ============================================================================


#ifndef _UIRECTANGLE_H_
#define _UIRECTANGLE_H_

#include <Framework/Controls/Shape.h>

namespace suic
{

class SUICORE_API Rectangle : public Shape
{
public:

    static DpProperty* RadiusXProperty;
    static DpProperty* RadiusYProperty;

    static bool StaticInit();

    Rectangle();
    virtual ~Rectangle();

    RTTIOfClass(Rectangle)

    int GetRadiusX();
    int GetRadiusY();

    void SetRadiusX(int val);
    void SetRadiusY(int val);

protected:

    Size OnMeasure(const Size& size);
    void OnRenderSizeChanged(SizeChangedInfo& e);

    void OnRender(Drawing* drawing);
};

inline int Rectangle::GetRadiusX()
{
    return GetValue(RadiusXProperty)->ToInt();
}

inline int Rectangle::GetRadiusY()
{
    return GetValue(RadiusYProperty)->ToInt();
}

inline void Rectangle::SetRadiusX(int val)
{
    SetValue(RadiusXProperty, new Integer(val));
}

inline void Rectangle::SetRadiusY(int val)
{
    SetValue(RadiusYProperty, new Integer(val));
}

}

#endif
