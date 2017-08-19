// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����GradientBrush.h
// ��  �ܣ����彥��ˢ�ӻ��ࡣ
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2010-07-02
// 
// ============================================================================

#ifndef _UIGRADIENTBRUSH_H_
#define _UIGRADIENTBRUSH_H_

#include <System/Graphics/Brush.h>

namespace suic
{

class SUICORE_API GradientStops : public Object
{
public:

    void AddStops(Float p, Uint32 c)
    {
        colors.Add(c);
        ratios.Add(p);
    }

    int GetCount() const
    {
        return colors.GetCount();
    }

    void Clear()
    {
        colors.Clear();
        ratios.Clear();
    }

    Array<Uint32> colors;
    Array<Float> ratios;
};

class SUICORE_API GradientBrush : public Brush
{
public:

    RTTIOfAbstractClass(GradientBrush)

    GradientBrush();
    virtual ~GradientBrush();

    GradientStops* GetGradientStops();
    void SetGradientStops(GradientStops* stops);

    void AddGradientStop(Color clr, Float radio);

    bool Parse(IXamlNode* pNode);

protected:

    GradientStops* _stops;
};

inline GradientStops* GradientBrush::GetGradientStops()
{
    if (_stops == NULL)
    {
        _stops = new GradientStops();
        _stops->ref();
    }
    return _stops;
}

}

#endif
