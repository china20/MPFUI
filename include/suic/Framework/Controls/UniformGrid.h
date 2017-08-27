// 华勤科技版权所有 2008-2022
// 
// 文件名：UniformGrid.h
// 功  能：等宽表格。
// 
// 作  者：MPF开发组
// 时  间：2011-07-02
// 
// ============================================================================


#ifndef _UIUNIFORMGRID_H_
#define _UIUNIFORMGRID_H_

#include <Framework/Controls/Panel.h>

namespace suic
{

/// <summary>
/// 均等网格布局管理，支持对子窗体的自动布局。
/// </summary>
class SUICORE_API UniformGrid : public Panel
{
public:

    static DpProperty* ColumnsProperty;
    static DpProperty* RowsProperty;

    static bool StaticInit();

    UniformGrid();
    ~UniformGrid();

    RTTIOfClass(UniformGrid)

    void SetColumns(int val);
    void SetRows(int val);

    int GetColumns();
    int GetRows();

    void SetFirstColumn(int val);
    int GetFirstColumn();

protected:

    Size OnMeasure(const Size& constraint) ;
    void OnArrange(const Size& arrangeSize);

protected:

    void UpdateComputedValues();

protected:

    int _columns;
    int _rows;

    int _firstColumn;
};

}

#endif
