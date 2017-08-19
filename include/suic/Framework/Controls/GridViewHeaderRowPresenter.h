// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����GridViewHeaderRowPresenter.h
// ��  �ܣ��б���ͼͷ����Ⱦ��
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2010-11-02
// 
// ============================================================================

#ifndef _UIGRIDVIEWHEADERROWPRESENTER_H_
#define _UIGRIDVIEWHEADERROWPRESENTER_H_

#include <Framework/Controls/ScrollViewer.h>
#include <Framework/Controls/GridViewRowPresenterBase.h>

namespace suic
{

class SUICORE_API GridViewHeaderRowPresenter : public GridViewRowPresenterBase
{
public:

    static DpProperty* ColumnHeaderToolTipProperty;
    static DpProperty* ColumnHeaderTemplateProperty;
    static DpProperty* ColumnHeaderContainerStyleProperty;

    RTTIOfClass(GridViewHeaderRowPresenter)

    static bool StaticInit();

    GridViewHeaderRowPresenter();
    ~GridViewHeaderRowPresenter();

    Style* GetColumnHeaderContainerStyle();
    void SetColumnHeaderContainerStyle(Style* val);

    ControlTemplate* GetColumnHeaderTemplate();
    void SetColumnHeaderTemplate(ControlTemplate* val);

    void OnMainScrollChanged(Element* sender, ScrollChangedEventArg* e);
    void OnHeaderScrollChanged(Element* sender, ScrollChangedEventArg* e);

public:

    void OnPreApplyTemplate();
    Size OnMeasure(const Size& constraintSize);
    void OnArrange(const Size& arrangeSize);

private:

    void AttatchToScrollView();

private:

    Byte _scollIsPending;
    ScrollViewer* _svMain;
    ScrollViewer* _svHeader;
};

}

#endif
