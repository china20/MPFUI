// 华勤科技版权所有 2008-2022
// 
// 文件名：GridViewRowPresenterBase.h
// 功  能：列表视图行渲染。
// 
// 作  者：MPF开发组
// 时  间：2010-11-02
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

    static void StaticInit();

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

    // 是否需要刷新显示信息
    bool _needReflesh;
    // 可视子元素
    ElementColl* _uiElems;
    GridViewColumnCollection* _columns;
};

}

#endif
