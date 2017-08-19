// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����UniformGrid.h
// ��  �ܣ��ȿ���
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2011-07-02
// 
// ============================================================================


#ifndef _UIUNIFORMGRID_H_
#define _UIUNIFORMGRID_H_

#include <Framework/Controls/Panel.h>

namespace suic
{

/// <summary>
/// �������񲼾ֹ���֧�ֶ��Ӵ�����Զ����֡�
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
