// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����Ellipse.h
// ��  �ܣ�ʵ����Բ�ؼ���
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2011-07-02
// 
// ============================================================================


#ifndef _UIELLIPSE_H_
#define _UIELLIPSE_H_

#include <Framework/Controls/Shape.h>

namespace suic
{

class SUICORE_API Ellipse : public Shape
{
public:

    static bool StaticInit();

    Ellipse();
    virtual ~Ellipse();

    RTTIOfClass(Ellipse)

protected:

    Size OnMeasure(const Size& size);
    void OnRender(Drawing * drawing);
};

}

#endif
