// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����CaretElement.h
// ��  �ܣ��������߼���
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2012-07-02
// 
// ============================================================================

#ifndef _UICARETELEMENT_H_
#define _UICARETELEMENT_H_

#include <System/Windows/FrameworkElement.h>

namespace suic {

class SUICORE_API CaretElement : public FrameworkElement
{
public:

    static DpProperty* BackgroundProperty;

    CaretElement();
    virtual ~CaretElement();

    static bool StaticInit();

    RTTIOfClass(CaretElement)

    void Show();
    void Hide();

    void SetPos(Point point);
    void SetSize(Size size);

    Brush* GetBackground();
    void SetBackground(Brush* val);

    bool IsBlinking();

protected:

    void OnInitialized(EventArg* e);
    void OnRender(Drawing * drawing);
};

}

#endif
