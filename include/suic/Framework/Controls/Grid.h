// 华勤科技版权所有 2010-2011
// 
// 文件名：Grid.h
// 功  能：实现表格逻辑。
// 
// 作  者：MPF开发组
// 时  间：2011-07-02
// 
// ============================================================================


#ifndef _UIGRID_H_
#define _UIGRID_H_

#include <Framework/Controls/Panel.h>
#include <Framework/Controls/RowDefinition.h>
#include <Framework/Controls/ColumnDefinition.h>

#include <System/Windows/FrameworkElementFactory.h>

namespace suic
{

struct SUICORE_API CellCache 
{
    // 单元格所在行索引
    int RowIndex;
    // 单元格所在列索引
    int ColumnIndex;
    // 合并的行数
    int RowSpan;
    // 合并的列数
    int ColumnSpan;
    // 单元格(column)类型
    GridSizeType SizeTypeU;
    // 单元格(row)类型
    GridSizeType SizeTypeV;

    // 相同类型下一索引
    int Next;

    bool IsStarU() const
    {
        return (((Byte)(SizeTypeU & GridSizeType::Star)) != 0);
    }

    bool IsAutoU() const
    {
        return (((Byte)(SizeTypeU & GridSizeType::Auto)) != 0);
    }

    bool IsStarV() const
    {
        return (((Byte)(SizeTypeV & GridSizeType::Star)) != 0);
    }

    bool IsAutoV() const
    {
        return (((Byte)(SizeTypeV & GridSizeType::Auto)) != 0);
    }
};

class ExtendedData
{
public:

    // 行列不带*
    int CellGroup1;
    // 列不带*
    int CellGroup2;
    // 行不带*
    int CellGroup3;
    // 行列都带*
    int CellGroup4;

    ArrayType<CellCache> CellCaches;

    ExtendedData();
    ~ExtendedData();

    RowDefinitions* GetRowDefs() const
    {
        return _rows;
    }

    ColumnDefinitions* GetColumnDefs() const
    {
        return _columns;
    }

    DefinitionBase* GetColumn(int index) const
    {
        return _columns->GetItem(index);
    }

    DefinitionBase* GetRow(int index) const
    {
        return _rows->GetItem(index);
    }

private:

    RowDefinitions* _rows;
    ColumnDefinitions* _columns;
};

/// <summary>
/// Grid
/// </summary>
class SUICORE_API Grid : public Panel
{
public:

    // 列索引，附加属性
    static suic::DpProperty* ColumnProperty;
    // 行索引，附加属性
    static suic::DpProperty* RowProperty;
    // 合并列数
    static suic::DpProperty* ColumnSpanProperty;
    // 合并行数
    static suic::DpProperty* RowSpanProperty;
    // 是否显示网格线
    static suic::DpProperty* ShowGridLinesProperty;

    Grid();
    virtual ~Grid();

    RTTIOfClass(Grid)

    static void SetColumn(Element* elem, int val);
    static void SetRow(Element* elem, int val);
    static void SetColumnSpan(Element* elem, int val);
    static void SetRowSpan(Element* elem, int val);

    static int GetColumn(Element* elem);
    static int GetRow(Element* elem);
    static int GetColumnSpan(Element* elem);
    static int GetRowSpan(Element* elem);

    static bool StaticInit();

    ColumnDefinitions* GetColumnDefs();
    RowDefinitions* GetRowDefs();

    void AddColumnDef(ColumnDefinition* val);
    void AddRowDef(RowDefinition* val);

    bool IsShowGridLines();
    void SetShowGridLines(bool val);

    bool IsCellsCacheDirty() const;
    void SetCellsCacheDirty(bool val);

protected:

    void RenderChildren(Drawing* drawing);

    Size OnMeasure(const Size& constrait);
    void OnArrange(const Size& arrangeSize);
    
    bool OnReadingChild(IXamlNode* node);

    void ReadGridRowDefs(IXamlNode* pNode);
    void ReadGridColumnDefs(IXamlNode* pNode);

protected:

    ExtendedData* GetData() const;

    CellCache& GetCellCache(int index);

    RowDefinition* GetRowDef(int index);
    ColumnDefinition* GetColumnDef(int index);

    void EnsureCacheCells();
    void CalcFinalColumnSize();
    void CalcFinalRowSize();

    Float CalculateDesiredSize(GridDefinitions* definitions);
    GridSizeType GetSizeTypeForRange(GridDefinitions* definitions, int start, int count);
    void MeasureCell(int cell, bool forceInfinityV);
    void MeasureCellsGroup(int cellsHead, Size referenceSize, bool ignoreDesiredSizeU, bool forceInfinityV);
    Float GetMeasureSizeForRange(GridDefinitions* definitions, int start, int count);
    Float GetFinalSizeForRange(GridDefinitions* definitions, int start, int count);

    void ValidateDefsLayout(GridDefinitions* definitions, bool treatStarAsAuto);
    void ResolveStar(GridDefinitions* definitions, Float availableSize);
    void EnsureMinSizeInDefRange(GridDefinitions* definitions, int start, int count, Float requestedSize, Float percentRefSize);
    void SetFinalSize(GridDefinitions* definitions, Float finalSize, bool columns);

    bool HasFlags(int name) const;
    void SetFlags(int name, bool val);

protected:

    Int32 _flags;
    ExtendedData* _data;
};

inline ExtendedData* Grid::GetData() const
{
    return _data;
}

inline bool Grid::HasFlags(int name) const
{
    return ((_flags & name) == name);
}

inline void Grid::SetFlags(int name, bool val)
{
    if (val)
    {
        _flags |= name;
    }
    else
    {
        _flags &= ~name;
    }
}

};

#endif
