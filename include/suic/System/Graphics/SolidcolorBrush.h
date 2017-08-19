// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����SolidColorBrush.h
// ��  �ܣ�ʵ����ɫˢ�ӡ�
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2010-07-02
// 
// ============================================================================

#ifndef _UISOLIDCOLORBRUSH_H_
#define _UISOLIDCOLORBRUSH_H_

#include <System/Graphics/Brush.h>

namespace suic
{

class SUICORE_API SolidColorBrush : public Brush
{
public:

    static SolidColorBrush* Red;
    static SolidColorBrush* Green;
    static SolidColorBrush* Blue;
    static SolidColorBrush* Black;
    static SolidColorBrush* White;

    static bool StaticInit();

    RTTIOfClass(SolidColorBrush)

    SolidColorBrush();
    SolidColorBrush(Color color);
    virtual ~SolidColorBrush();

    SolidColorBrush* Clone() const;

    bool Parse(const String& xml);
    bool Parse(IXamlNode* pNode);

    Color GetColor() const;
    void SetColor(Color color);

    Color ToColor();
    String ToString();

protected:

    Color _color;
};

inline Color SolidColorBrush::ToColor()
{
    return _color;
}

};

#endif
