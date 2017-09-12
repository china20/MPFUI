// 华勤科技版权所有 2008-2022
// 
// 文件名：GridView.h
// 功  能：列表控件视图。
// 
// 作  者：MPF开发组
// 时  间：2010-11-02
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

    static void StaticInit();

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
