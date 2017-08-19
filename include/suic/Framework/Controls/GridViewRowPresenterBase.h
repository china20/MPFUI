// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����GridViewRowPresenterBase.h
// ��  �ܣ��б���ͼ����Ⱦ��
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2010-11-02
// 
// ============================================================================

#ifndef _UIGRIDVIEWROWPRESENTERBASE_H_
#define _UIGRIDVIEWROWPRESENTERBASE_H_

#include <System/Windows/FrameworkElement.h>
#include <System/Tools/ElementCollection.h>
#include <Framework/Controls/GridViewColumnCollection.h>

namespace suic
{

class SUICORE_API GridViewRowPresenterBase : public FrameworkElement
{
public:

    static DpProperty* ColumnsProperty;

    static bool StaticInit();

    static void OnColumnsPropChanged(DpObject* d, DpPropChangedEventArg* e);

    RTTIOfClass(GridViewRowPresenterBase)

    GridViewRowPresenterBase();
    ~GridViewRowPresenterBase();

    GridViewColumnCollection* GetColumns();

public:

    int GetVisualChildrenCount();
    Element* GetVisualChild(int index);

    Element* GetLogicalChild(int index);
    int GetLogicalChildrenCount();

protected:

    ElementColl* GetInternalChildren();
    Element* GetInternalChild(int index);

protected:

    // �Ƿ���Ҫˢ����ʾ��Ϣ
    bool _needReflesh;
    // ������Ԫ��
    ElementColl* _uiElems;
    GridViewColumnCollection* _columns;
};

}

#endif
