// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����GridView.h
// ��  �ܣ��б�ؼ���ͼ��
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2010-11-02
// 
// ============================================================================

#ifndef _UIGRIDVIEW_H_
#define _UIGRIDVIEW_H_

#include <Framework/Controls/ViewBase.h>
#include <Framework/Controls/GridViewColumnCollection.h>

namespace suic
{

class SUICORE_API GridView : public ViewBase, public IAddChild
{
public:

    RTTIOfClass(GridView)

    static DpProperty* ColumnsProperty;
    static DpProperty* ColumnHeaderToolTipProperty;
    static DpProperty* ColumnHeaderTemplateProperty;
    static DpProperty* ColumnHeaderContainerStyleProperty;

    static bool StaticInit();

    GridView();
    ~GridView();

    void AddChild(Object* obj);
    void AddText(String val);
    void RemoveChild(Object* child);

    Element* GetOwner() const;
    GridViewColumnCollection* GetColumns();

public:

    void LinkViewOwner(DpObject* owner);
    void UnlinkViewOwner(DpObject* owner);
    void PrepareItem(DpObject* item);

private:

    Element* _owner;
    GridViewColumnCollection* _columns;
};

inline Element* GridView::GetOwner() const
{
    return _owner;
}

}

#endif
