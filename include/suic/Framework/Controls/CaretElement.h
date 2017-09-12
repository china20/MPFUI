// 华勤科技版权所有 2008-2022
// 
// 文件名：CaretElement.h
// 功  能：处理光标逻辑。
// 
// 作  者：MPF开发组
// 时  间：2012-07-02
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

    static void StaticInit();

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
