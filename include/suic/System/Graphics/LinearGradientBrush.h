// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����GradientBrush.h
// ��  �ܣ����彥��ˢ�ӻ��ࡣ
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2010-07-02
// 
// ============================================================================

#ifndef _UILINEARGRADIENTBRUSH_H_
#define _UILINEARGRADIENTBRUSH_H_

#include <System/Tools/Array.h>
#include <System/Graphics/GradientBrush.h>

namespace suic
{

struct LinearGradientInfo
{
    Float endX;
    Float endY;
    Float startX;
    Float startY;

    Array<Float> posinfo;
    Array<Uint32> colors;

    LinearGradientInfo();

    void AddGradientStop(Color clr, Float radio);
};

class SUICORE_API LinearGradientBrush : public GradientBrush
{
public:

    RTTIOfClass(LinearGradientBrush)

    LinearGradientBrush();
    virtual ~LinearGradientBrush();

    LinearGradientBrush* Clone() const;

    void SetStartPoint(fPoint start);
    void SetEndPoint(fPoint end);

    fPoint GetStartPoint() const;
    fPoint GetEndPoint() const;

    bool Parse(const String& xml);
    bool Parse(IXamlNode* pNode);

protected:

    fPoint _endPoint;
    fPoint _startPoint;
};

inline fPoint LinearGradientBrush::GetStartPoint() const
{
    return _startPoint;
}

inline fPoint LinearGradientBrush::GetEndPoint() const
{
    return _endPoint;
}

}

#endif
