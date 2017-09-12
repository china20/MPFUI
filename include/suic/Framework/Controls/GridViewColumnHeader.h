// 华勤科技版权所有 2008-2022
// 
// 文件名：GridViewHeaderColumn.h
// 功  能：列表视图头。
// 
// 作  者：MPF开发组
// 时  间：2010-11-02
// 
// ============================================================================

#ifndef _UIGRIDVIEWHEADERCOLUMN_H_
#define _UIGRIDVIEWHEADERCOLUMN_H_

#include <Framework/Controls/Thumb.h>
#include <Framework/Controls/ButtonBase.h>
#include <Framework/Controls/GridViewColumn.h>

namespace suic
{

class SUICORE_API GridViewColumnHeader : public ButtonBase
{
public:

    RTTIOfClass(GridViewColumnHeader)

    static DpProperty* ColumnProperty;

    static void StaticInit();

    GridViewColumnHeader();
    ~GridViewColumnHeader();

    GridViewColumn* GetColumn();
    void SetColumn(GridViewColumn* column);

protected:

    void OnApplyTemplate();

    void OnThumbDragCompleted(DpObject* sender, RoutedEventArg* e);
    void OnThumbDragDelta(DpObject* sender, RoutedEventArg* e);
    void OnThumbDragStarted(DpObject* sender, RoutedEventArg* e);
    void OnDoubleClickThumbDrag(Element* sneder, MouseButtonEventArg* e);

private:

    Element* _owner;
    Thumb* _headerGripper;

    static int _trackValue;

    friend class GridViewHeaderRowPresenter;
};

inline GridViewColumn* GridViewColumnHeader::GetColumn()
{
    return RTTICast<GridViewColumn>(GetValue(ColumnProperty));
}

inline void GridViewColumnHeader::SetColumn(GridViewColumn* column)
{
    SetValue(ColumnProperty, column);
}

}

#endif
