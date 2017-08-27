// 华勤科技版权所有 2008-2022
// 
// 文件名：GridViewRowPresenter.h
// 功  能：列表视图行渲染。
// 
// 作  者：MPF开发组
// 时  间：2010-11-02
// 
// ============================================================================

#ifndef _UIGRIDVIEWROWPRESENTER_H_
#define _UIGRIDVIEWROWPRESENTER_H_

#include <Framework/Controls/GridViewRowPresenterBase.h>

namespace suic
{

class SUICORE_API GridViewRowPresenter : public GridViewRowPresenterBase
{
public:

    static DpProperty* ContentProperty;

    static bool StaticInit();
    static void OnContentPropChanged(DpObject* d, DpPropChangedEventArg* e);

    RTTIOfClass(GridViewRowPresenter)

    GridViewRowPresenter();
    ~GridViewRowPresenter();

    Object* GetContent();
    void SetContent(Object* val);

public:

    void OnPreApplyTemplate();

    Size OnMeasure(const Size& constraintSize);
    void OnArrange(const Size& arrangeSize);

private:

    void UpdateCells();
    void CreateCellElement(ElementColl* children, GridViewColumn* column);

private:

    Object* _content;
};

inline Object* GridViewRowPresenter::GetContent()
{
    return _content;
}

inline void GridViewRowPresenter::SetContent(Object* val)
{
    SetValue(ContentProperty, val);
}

}

#endif
