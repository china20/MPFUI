// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����GridViewHeaderColumn.h
// ��  �ܣ��б���ͼͷ��
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2010-11-02
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

    static bool StaticInit();

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
