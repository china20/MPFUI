// ======================================================================
//
// Copyright (c) 2008-2022 汪荣(视点UI), Inc. All rights reserved.
//
// MPF界面库遵循指定开源协议，团体或公司商用请根据协议购买授权，
// 任何个人、团体或公司不能居于此库衍生任何商业性质的库或代码。
//
// ======================================================================

/////////////////////////////////////////////////////////////////////////
// Grid.cpp

#include <Framework/Controls/Grid.h>
#include <System/Graphics/SolidcolorBrush.h>
#include <System/Tools/HashMap.h>
#include <System/Tools/Math.h>
#include <System/Tools/ResourceParser.h>

namespace suic
{

//////////////////////////////////////////////////////////////
// Grid
//

ImplementRTTIOfClass(Grid, Panel)

suic::DpProperty* Grid::ColumnProperty;
suic::DpProperty* Grid::RowProperty;

suic::DpProperty* Grid::ColumnSpanProperty;
suic::DpProperty* Grid::RowSpanProperty;
suic::DpProperty* Grid::ShowGridLinesProperty;

static bool CompareNullRefs(Object* x, Object* y, int& result)
{
    result = 2;
    if (x == NULL)
    {
        if (y == NULL)
        {
            result = 0;
        }
        else
        {
            result = -1;
        }
    }
    else if (y == NULL)
    {
        result = 1;
    }
    return (result != 2);
}

static int CompareFloat(Float val1, Float val2)
{
    int num = 0;

    if (FloatUtil::LessThan(val1, val2))
    {
        num = -1;
    }
    else if (FloatUtil::GreaterThan(val1, val2))
    {
        num = 1;
    }

    return num;
}

class StarDistributionOrderIndexComparer
{
public:
    
    GridDefinitions* definitions;

    StarDistributionOrderIndexComparer(GridDefinitions* definitions)
    {
        this->definitions = definitions;
    }

    int Compare(Float x, Float y, Object* flag)
    {
        int num = 0;
        DefinitionBase* defb1 = (*definitions)[x];
        DefinitionBase* defb2 = (*definitions)[y];

        if (!CompareNullRefs(defb1, defb2, num))
        {
            num = CompareFloat(defb1->GetSizeCache(), defb2->GetSizeCache());
        }
        return num;
    }
};

class StarDistributionOrderComparer
{
public:

    StarDistributionOrderComparer()
    {
    }

    int Compare(DefinitionBase* defb1, DefinitionBase* defb2, Object* flag)
    {
        int num = 0;

        if (!CompareNullRefs(defb1, defb2, num))
        {
            num = CompareFloat(defb1->GetSizeCache(), defb2->GetSizeCache());
        }
        return num;
    }
};

class SpanMaxDistributionOrderComparer 
{
public:

    SpanMaxDistributionOrderComparer ()
    {
    }

    int Compare(DefinitionBase* defb1, DefinitionBase* defb2, Object* flag)
    {
        int num = 0;

        if (CompareNullRefs(defb1, defb2, num))
        {
            return num;
        }

        if (defb1->IsAuto())
        {
            if (defb2->IsAuto())
            {
                return CompareFloat(defb1->GetSizeCache(), defb2->GetSizeCache());
            }
            else
            {
                return 1;
            }
        }
        if (defb2->IsAuto())
        {
            return -1;
        }
        else
        {
            return CompareFloat(defb1->GetSizeCache(), defb2->GetSizeCache());
        }
    }
};

class DistributionOrderIndexComparer
{
public:

    GridDefinitions* definitions;

    DistributionOrderIndexComparer(GridDefinitions* definitions)
    {
        this->definitions = definitions;
    }

    int Compare(int x, int y, Object* flag)
    {
        int num = 0;
        DefinitionBase* defb1 = (*definitions)[x];
        DefinitionBase* defb2 = (*definitions)[y];

        if (!CompareNullRefs(defb1, defb2, num))
        {
            double num2 = defb1->GetSizeCache() - defb1->GetMinSizeForArrange();
            double num3 = defb2->GetSizeCache() - defb2->GetMinSizeForArrange();

            if (num2 < num3)
            {
                num = -1;
            }
            else if (num2 > num3)
            {
                num = 1;
            }
        }
        return num;
    }
};

class SpanPreferredDistributionOrderComparer
{
public:

    SpanPreferredDistributionOrderComparer()
    {

    }

    int Compare(DefinitionBase* defb1, DefinitionBase* defb2, Object* flag)
    {
        int num = 0;

        if (CompareNullRefs(defb1, defb2, num))
        {
            return num;
        }
        if (defb1->IsAuto())
        {
            if (defb2->IsAuto())
            {
                return CompareFloat(defb1->GetMinSize(), defb2->GetMinSize());
            }
            return -1;
        }
        if (defb2->IsAuto())
        {
            return 1;
        }

        return CompareFloat(defb1->GetPreferredSize(), defb2->GetPreferredSize());
    }
};

enum Flags
{
    ArrangeOverrideInProgress = 0x80000,
    HasGroup3CellsInAutoRows = 0x20000,
    HasStarCellsU = 0x8000,
    HasStarCellsV = 0x10000,
    ListenToNotifications = 0x1000,
    MeasureOverrideInProgress = 0x40000,
    ShowGridLinesPropertyValue = 0x100,
    SizeToContentU = 0x2000,
    SizeToContentV = 0x4000,
    ValidCellsStructure = 4,
    ValidDefinitionsUStructure = 1,
    ValidDefinitionsVStructure = 2,
};

ExtendedData::ExtendedData()
{
    _rows = new RowDefinitions();
    _columns = new ColumnDefinitions();
}

ExtendedData::~ExtendedData()
{
    delete _rows;
    delete _columns;
}

class SpanKey
{
private:
    
    int _count;
    int _start;
    bool _u;

public:

    SpanKey()
        : _start(0)
        , _count(0)
        , _u(false)
    {

    }

    SpanKey(int start, int count, bool u)
    {
       _start = start;
       _count = count;
        _u = u;
    }

    SpanKey(const SpanKey& Other)
    {
        _start = Other._start;
        _count = Other._count;
        _u = Other._u;
    }

    bool Equals(const SpanKey& obj)
    {
        SpanKey key = obj;
        return ((((key._start == _start)) && (key._count == _count)) && (key._u == _u));
    }

    bool operator==(const SpanKey& obj)
    {
        return Equals(obj);
    }

    int GetHashCode() const
    {
        int num = _start ^ (_count << 2);
        if (_u)
        {
            return (num & 0x7ffffff);
        }
        return (num | 0x8000000);
    }

    int Count()
    {
        return _count;
    }

    int Start()
    {
        return _start;
    }

    bool U()
    {
        return _u;
    }
};

Grid::Grid()
    : _flags(0)
    , _data(NULL)
{
}

Grid::~Grid()
{
    if (_data)
    {
        delete _data;
    }
}

void Grid::StaticInit()
{
    if (ColumnProperty == NULL)
    {
        Panel::StaticInit();

        ColumnProperty = DpProperty::RegisterAttached(_T("Column"), RTTIType(), 
            Integer::RTTIType(), DpPropMemory::GetPropMeta(Integer::GetZeroInt(), PropMetadataOptions::AffectsParentArrange));
        RowProperty = DpProperty::RegisterAttached(_T("Row"), RTTIType(), Integer::RTTIType(), 
            DpPropMemory::GetPropMeta(Integer::GetZeroInt(), PropMetadataOptions::AffectsParentArrange));

        ColumnSpanProperty = DpProperty::RegisterAttached(_T("ColumnSpan"), RTTIType(), Integer::RTTIType(), 
            DpPropMemory::GetPropMeta(new Integer(1), PropMetadataOptions::AffectsParentArrange));
        RowSpanProperty = DpProperty::RegisterAttached(_T("RowSpan"), RTTIType(), Integer::RTTIType(), 
            DpPropMemory::GetPropMeta(new Integer(1), PropMetadataOptions::AffectsParentArrange));
        ShowGridLinesProperty = DpProperty::Register(_T("ShowGridLines"), RTTIType(), Boolean::RTTIType(), 
            DpPropMemory::GetPropMeta(Boolean::False, PropMetadataOptions::AffectsRender));
    }
}

void Grid::ValidateDefsLayout(GridDefinitions* defs, bool treatStarAsAuto)
{
    for (int i = 0; i < defs->GetCount(); i++)
    {
        DefinitionBase* defb = defs->GetItem(i);
        Float userMinSize = defb->GetUserMinSize();
        Float userMaxSize = defb->GetUserMaxSize();
        Float positiveInfinity = 0.0f;

        // 恢复上次计算的中间值
        defb->Reset();

        switch (defb->GetSizeType())
        {
        case GridSizeType::Auto:
            positiveInfinity = Numeric::MeasureInt;
            break;

        case GridSizeType::Pixel:
            positiveInfinity = defb->GetUserSize()->GetValue();
            userMinSize = max(userMinSize, min(positiveInfinity, userMaxSize));
            break;

        case GridSizeType::Star:
            positiveInfinity = Numeric::MeasureInt;
            break;

        default:
            ;
        }

        defb->SetMinSize(userMinSize);
        defb->SetMeasureSize(max(userMinSize, min(positiveInfinity, userMaxSize)));
    }
}

void Grid::RenderChildren(Drawing* drawing)
{
    Panel::RenderChildren(drawing);

    if (IsShowGridLines())
    {
        suic::Pen pen;
        int iOffset = 0;
        int iHei = GetActualHeight();
        int iWid = GetActualWidth();

        pen.SetAntiAlias(false);
        pen.SetThickness(1);

        for (int i = 0; i < GetRowDefs()->GetCount(); ++i)
        {
            iOffset = GetRowDefs()->GetItem(i)->GetOffset();
            // 绘制线
            drawing->DrawLine(DrawCtx::DefDraw, &pen, fPoint(0, iOffset), fPoint(iWid, iOffset));
        }

        for (int j = 0; j < GetColumnDefs()->GetCount(); ++j)
        {
            iOffset = GetColumnDefs()->GetItem(j)->GetOffset();
            drawing->DrawLine(DrawCtx::DefDraw, &pen, fPoint(iOffset, 0), fPoint(iOffset, iHei));
        }

        drawing->DrawLine(DrawCtx::DefDraw, &pen, fPoint(0, 0), fPoint(iWid, 0));
        drawing->DrawLine(DrawCtx::DefDraw, &pen, fPoint(iWid - pen.GetThickness(), 0), fPoint(iWid, iHei));
        drawing->DrawLine(DrawCtx::DefDraw, &pen, fPoint(iWid, iHei - pen.GetThickness()), fPoint(0, iHei - pen.GetThickness()));
        drawing->DrawLine(DrawCtx::DefDraw, &pen, fPoint(0, iHei), fPoint(0, 0));
    }
}

Size Grid::OnMeasure(const Size& constraint)
{
    Size desiredSize;

    if (NULL == _data || 
        (_data->GetColumnDefs()->GetCount() == 0 && 
        _data->GetRowDefs()->GetCount() == 0))
    {
        ElementColl* children = GetChildren();

        for (int i = 0, count = children->GetCount(); i < count; ++i) 
        { 
            suic::Element* child = children->GetAt(i);

            if (child != NULL)
            {
                SetMeasureDataOnChild(child, constraint);
                child->Measure(constraint);
                desiredSize.cx = max(desiredSize.cx, child->GetDesiredSize().Width()); 
                desiredSize.cy = max(desiredSize.cy, child->GetDesiredSize().Height());
            } 
        }
    }
    else
    {
        if (0 == _data->GetColumnDefs()->GetCount())
        {
            AddColumnDef(new ColumnDefinition());
        }
        else if (0 == _data->GetRowDefs()->GetCount())
        {
            AddRowDef(new RowDefinition());
        }

        ValidateDefsLayout(GetRowDefs(), false);
        ValidateDefsLayout(GetColumnDefs(), false);

        EnsureCacheCells();

        // 测量高和宽都固定的元素
        MeasureCellsGroup(_data->CellGroup1, constraint, false, false);

        if (!HasFlags(HasGroup3CellsInAutoRows))
        {
            if (HasFlags(HasStarCellsV))
            {
                ResolveStar(GetRowDefs(), constraint.Height());
            }

            MeasureCellsGroup(_data->CellGroup2, constraint, false, false);

            if (HasFlags(HasStarCellsU))
            {
                ResolveStar(GetColumnDefs(), constraint.Width());
            }

            MeasureCellsGroup(_data->CellGroup3, constraint, false, false);
        }
        else if (_data->CellGroup2 > _data->CellCaches.Length())
        {
            if (HasFlags(HasStarCellsU))
            {
                ResolveStar(GetColumnDefs(), constraint.Width());
            }

            MeasureCellsGroup(_data->CellGroup3, constraint, false, false);

            if (HasFlags(HasStarCellsV))
            {
                ResolveStar(GetRowDefs(), constraint.Height());
            }
        }
        else
        {
            MeasureCellsGroup(_data->CellGroup2, constraint, false, true);

            if (HasFlags(HasStarCellsU))
            {
                ResolveStar(GetColumnDefs(), constraint.Width());
            }

            MeasureCellsGroup(_data->CellGroup3, constraint, false, false);

            if (HasFlags(HasStarCellsV))
            {
                ResolveStar(GetRowDefs(), constraint.Height());
            }

            MeasureCellsGroup(_data->CellGroup2, constraint, true, false);
        }

        MeasureCellsGroup(_data->CellGroup4, constraint, false, false);
        desiredSize = Size(CalculateDesiredSize(GetColumnDefs()), CalculateDesiredSize(GetRowDefs()));
    }

    return desiredSize;
}

void Grid::OnArrange(const Size& arrangeSize)
{
    if (NULL == _data || 
        (_data->GetColumnDefs()->GetCount() == 0 && 
        _data->GetRowDefs()->GetCount() == 0))
    {
        ElementColl* children = GetChildren(); 
        Rect finalRect(0, 0, arrangeSize.cx, arrangeSize.cy);

        for(int i = 0, count = children->GetCount(); i < count; i++)
        { 
            suic::Element* child = children->GetAt(i);
            if (child != NULL) 
            {
                child->Arrange(finalRect);
            }
        } 
    } 
    else
    {
        // 先确认缓存项是否有效
        //EnsureCacheCells();

        SetFinalSize(_data->GetColumnDefs(), arrangeSize.Width(), true);
        SetFinalSize(_data->GetRowDefs(), arrangeSize.Height(), false);

        ElementColl* children = GetChildren(); 

        // 开始布局
        for (int i = 0; i < _data->CellCaches.Length(); i++)
        {
            Element* element2 = children->GetAt(i);

            if (element2 != NULL)
            {
                int columnIndex = _data->CellCaches[i].ColumnIndex;
                int rowIndex = _data->CellCaches[i].RowIndex;
                int columnSpan = _data->CellCaches[i].ColumnSpan;
                int rowSpan = _data->CellCaches[i].RowSpan;

                Rect finalRect((columnIndex == 0) ? 0.0 : _data->GetColumn(columnIndex)->GetOffset()
                    , (rowIndex == 0) ? 0.0 : _data->GetRow(rowIndex)->GetOffset()
                    , GetFinalSizeForRange(GetColumnDefs(), columnIndex, columnSpan)
                    , GetFinalSizeForRange(GetRowDefs(), rowIndex, rowSpan));

                element2->Arrange(finalRect);
            }
        }
    }
}

bool Grid::OnReadingChild(IXamlNode* node)
{
    bool bFind = true;
    String name = node->GetName();

    if (name.Equals(_U("Grid.RowDefinitions")))
    {
        ReadGridRowDefs(node);
    }   
    else if (name.Equals(_U("Grid.ColumnDefinitions")))
    {
        ReadGridColumnDefs(node);
    }
    else
    {
        bFind = false;
    }

    return bFind;
}

void Grid::ReadGridRowDefs(IXamlNode* pNode)
{
    // 读取子节点
    pNode->Reset();

    while (pNode->HasNext())
    {
        IXamlNode* child = pNode->Current();
        String name = child->GetName();

        if (name.Equals(_U("RowDefinition")))
        {
            RowDefinition* rowDef = new RowDefinition();
            IXamlAttris* attri = child->GetAttris();
            attri->Reset();
            GetRowDefs()->AddItem(rowDef);

            while (attri->HasNext())
            {
                String name = attri->GetName();
                String val = attri->GetValue();

                val.Trim();

                if (val.Empty())
                {
                    continue;
                }

                DpProperty* dp = DpProperty::Lookup(name, rowDef->GetRTTIType());

                if (NULL != dp)
                {
                    ResourceItem resItem;
                    ResourceParserOp::ReadResource(NULL, dp, val, resItem);

                    if (resItem.res.get() != DpProperty::UnsetValue())
                    {
                        rowDef->SetValue(dp, resItem.res.get());
                    }
                }
            }
        }
        else
        {
            throw InvalidValueException(_U("Node of grid is error!"), __FILELINE__);
        }
    }
}

void Grid::ReadGridColumnDefs(IXamlNode* pNode)
{
    // 读取子节点
    pNode->Reset();

    while (pNode->HasNext())
    {
        IXamlNode* child = pNode->Current();
        String name = child->GetName();

        if (name.Equals(_U("ColumnDefinition")))
        {
            ColumnDefinition* colDef = new ColumnDefinition();
            IXamlAttris* attri = child->GetAttris();
            attri->Reset();
            GetColumnDefs()->AddItem(colDef);

            while (attri->HasNext())
            {
                String name = attri->GetName();
                String val = attri->GetValue();

                val.Trim();

                if (val.Empty())
                {
                    continue;
                }

                DpProperty* dp = DpProperty::Lookup(name, colDef->GetRTTIType());

                if (NULL != dp)
                {
                    ResourceItem resItem;
                    ResourceParserOp::ReadResource(NULL, dp, val, resItem);

                    if (resItem.res.get() != DpProperty::UnsetValue())
                    {
                        colDef->SetValue(dp, resItem.res.get());
                    }
                }
            }
        }
        else
        {
            throw InvalidValueException(_U("Node of grid is error!"), __FILELINE__);
        }
    }
}

void Grid::SetColumn(suic::Element* elem, int val)
{
    elem->SetValue(ColumnProperty, new suic::Integer(val));
}

void Grid::SetRow(suic::Element* elem, int val)
{
    elem->SetValue(RowProperty, new suic::Integer(val));
}

void Grid::SetColumnSpan(suic::Element* elem, int val)
{
    elem->SetValue(ColumnSpanProperty, new suic::Integer(val));
}

void Grid::SetRowSpan(suic::Element* elem, int val)
{
    elem->SetValue(RowSpanProperty, new suic::Integer(val));
}

int Grid::GetColumn(suic::Element* elem)
{
    return elem->GetValue(ColumnProperty)->ToInt();
}

int Grid::GetRow(suic::Element* elem)
{
    return elem->GetValue(RowProperty)->ToInt();
}

int Grid::GetColumnSpan(suic::Element* elem)
{
    return elem->GetValue(ColumnSpanProperty)->ToInt();
}

int Grid::GetRowSpan(suic::Element* elem)
{
    return elem->GetValue(RowSpanProperty)->ToInt();
}

ColumnDefinitions* Grid::GetColumnDefs()
{
    if (_data == NULL)
    {
        _data = new ExtendedData();
        _data->GetColumnDefs()->SetOwner(this);
    }
    return _data->GetColumnDefs();
}

RowDefinitions* Grid::GetRowDefs()
{
    if (_data == NULL)
    {
        _data = new ExtendedData();
        _data->GetRowDefs()->SetOwner(this);
    }
    return _data->GetRowDefs();
}

CellCache& Grid::GetCellCache(int index)
{
    return _data->CellCaches[index];
}

ColumnDefinition* Grid::GetColumnDef(int index)
{
    return RTTIEqualCast<ColumnDefinition>(_data->GetColumn(index));
}

RowDefinition* Grid::GetRowDef(int index)
{
    return RTTIEqualCast<RowDefinition>(_data->GetRow(index));
}

void Grid::AddColumnDef(ColumnDefinition* val)
{
    GetColumnDefs()->AddItem(val);
}

void Grid::AddRowDef(RowDefinition* val)
{
    GetRowDefs()->AddItem(val);
}

bool Grid::IsShowGridLines()
{
    return GetValue(ShowGridLinesProperty)->ToBool();
}

void Grid::SetShowGridLines(bool val)
{
    SetValue(ShowGridLinesProperty, BOOLTOBOOLEAN(val));
}

bool Grid::IsCellsCacheDirty() const
{
    return true;
}

void Grid::SetCellsCacheDirty(bool val)
{

}

Float Grid::CalculateDesiredSize(GridDefinitions* defs)
{
    Float num = 0;

    for (int i = 0; i < defs->GetCount(); i++)
    {
        num += defs->GetItem(i)->GetMinSize();
    }

    return num;
}

void Grid::EnsureCacheCells()
{
    if (IsCellsCacheDirty())
    {
        SetCellsCacheDirty(false);

        _data->CellCaches.Clear();

        _data->CellGroup1 = 0x7fffffff;
        _data->CellGroup2 = 0x7fffffff;
        _data->CellGroup3 = 0x7fffffff;
        _data->CellGroup4 = 0x7fffffff;

        bool flag = false;
        bool flag2 = false;
        bool flag3 = false;

        int colCount = GetColumnDefs()->GetCount();
        int rowCount = GetRowDefs()->GetCount();

        ElementColl* children = GetChildren(); 
        _data->CellCaches.Reset(children->GetCount(), true);

        for (int i = 0; i < children->GetCount(); ++i)
        {
            suic::Element* elem(children->GetAt(i));

            if (elem)
            {
                CellCache& cache = _data->CellCaches[i];

                cache.ColumnIndex = UIMIN(GetColumn(elem), colCount - 1);
                cache.RowIndex = UIMIN(GetRow(elem), rowCount - 1);
                cache.ColumnSpan = UIMIN(GetColumnSpan(elem), colCount - cache.ColumnIndex);
                cache.RowSpan = UIMIN(GetRowSpan(elem), rowCount - cache.RowIndex);
                
                cache.SizeTypeU = GetSizeTypeForRange(GetColumnDefs(), cache.ColumnIndex, cache.ColumnSpan);
                cache.SizeTypeV = GetSizeTypeForRange(GetRowDefs(), cache.RowIndex, cache.RowSpan);

                flag |= cache.IsStarU();
                flag2 |= cache.IsStarV();

                // 行不带*
                if (!cache.IsStarV())
                {
                    // 列不是Auto类型
                    if (!cache.IsStarU())
                    {
                        cache.Next = _data->CellGroup1;
                        _data->CellGroup1 = i;
                    }
                    else
                    {
                        cache.Next = _data->CellGroup3;
                        _data->CellGroup3 = i;
                        flag3 |= cache.IsAutoV();
                    }
                }
                else if (cache.IsAutoU() && !cache.IsStarU())
                {
                    cache.Next = _data->CellGroup2;
                    _data->CellGroup2 = i;
                }
                else
                {
                    cache.Next = _data->CellGroup4;
                    _data->CellGroup4 = i;
                }
            }
        }

        SetFlags(HasStarCellsU, flag);
        SetFlags(HasStarCellsV, flag2);
        SetFlags(HasGroup3CellsInAutoRows, flag3);
    }
}

static void RegisterSpan(HashMap<SpanKey, Float>& store, int start, int count, bool u, Float value)
{
    SpanKey key(start, count, u);
    HashMap<SpanKey, Float>::iterator iter = store.find(key);

    if (iter == store.end())
    {
        store.insert(key, value);
    }
    else if (iter->second < value)
    {
        iter->second = value;
    }
}

void Grid::MeasureCellsGroup(int cellsHead, Size referenceSize, bool ignoreDesiredSizeU, bool forceInfinityV)
{
    if (cellsHead < _data->CellCaches.Length())
    {
        ElementColl* children = GetChildren(); 
        bool flag = forceInfinityV;
        int cell = cellsHead;
        HashMap<SpanKey, Float> store;

        do
        {
            Element* child = children->GetAt(cell);

            MeasureCell(cell, forceInfinityV);

            if (!ignoreDesiredSizeU)
            {
                Float iDesiredWid = child->GetDesiredSize().Width();

                if (_data->CellCaches[cell].ColumnSpan == 1)
                {
                    Float iMinWid = min(iDesiredWid, GetColumnDef(GetCellCache(cell).ColumnIndex)->GetMaxWidth());
                    GetColumnDef(GetCellCache(cell).ColumnIndex)->UpdateMinSize(iMinWid);
                }
                else
                {
                    RegisterSpan(store, GetCellCache(cell).ColumnIndex, GetCellCache(cell).ColumnSpan, true, iDesiredWid);
                }
            }

            if (!flag)
            {
                Float iDesiredHei = child->GetDesiredSize().Height();

                if (_data->CellCaches[cell].RowSpan == 1)
                {
                    Float iMinHei = min(iDesiredHei, GetRowDef(GetCellCache(cell).RowIndex)->GetMaxHeight());
                    GetRowDef(GetCellCache(cell).RowIndex)->UpdateMinSize(iMinHei);
                }
                else
                {
                    RegisterSpan(store, GetCellCache(cell).RowIndex, GetCellCache(cell).RowSpan, false, iDesiredHei);
                }
            }

            cell = _data->CellCaches[cell].Next;
        }
        while (cell < _data->CellCaches.Length());

        HashMap<SpanKey, Float>::iterator iter = store.begin();

        while (iter != store.end())
        {
            SpanKey key = iter->first;
            Float requestedSize = iter->second;

            if (key.U())
            {
                EnsureMinSizeInDefRange(GetColumnDefs(), key.Start(), key.Count(), requestedSize, referenceSize.Width());
            }
            else
            {
                EnsureMinSizeInDefRange(GetRowDefs(), key.Start(), key.Count(), requestedSize, referenceSize.Height());
            }
            
            ++iter;
        }
    }
}

void Grid::MeasureCell(int cell, bool forceInfinityV)
{
    Float posInfinity = 0;
    Float widContraint = 0;

    if (_data->CellCaches[cell].IsAutoU() && !_data->CellCaches[cell].IsStarU())
    {
        posInfinity = Numeric::MeasureInt;
    }
    else
    {
        posInfinity = GetMeasureSizeForRange(GetColumnDefs(), _data->CellCaches[cell].ColumnIndex, _data->CellCaches[cell].ColumnSpan);
    }

    if (forceInfinityV)
    {
        widContraint = Numeric::MeasureInt;
    }
    else if (_data->CellCaches[cell].IsAutoV() && !_data->CellCaches[cell].IsStarV())
    {
        widContraint = Numeric::MeasureInt;
    }
    else
    {
        widContraint = GetMeasureSizeForRange(GetRowDefs(), _data->CellCaches[cell].RowIndex, _data->CellCaches[cell].RowSpan);
    }

    Element* child = GetChildren()->GetAt(cell);

    if (child != NULL)
    {
        Size childConstraint((int)posInfinity, (int)widContraint);

        SetMeasureDataOnChild(child, childConstraint);
        child->Measure(childConstraint);
    }
}

Float Grid::GetMeasureSizeForRange(GridDefinitions* defs, int start, int count)
{
    Float num = 0.0f;
    int index = (start + count) - 1;

    do
    {
        DefinitionBase* dbase = defs->GetItem(index);
        num += (dbase->GetSizeType() == GridSizeType::Auto) ? dbase->GetMinSize() : dbase->GetMeasureSize();
    }
    while (--index >= start);

    return num;
}

GridSizeType Grid::GetSizeTypeForRange(GridDefinitions* defs, int start, int count)
{
    GridSizeType none = GridSizeType::None;
    int index = (start + count) - 1;

    do
    {
        DefinitionBase* defb = defs->GetItem(index);
        none = (GridSizeType)((Byte)(none | defb->GetSizeType()));
    }
    while (--index >= start);

    return none;
}

void Grid::EnsureMinSizeInDefRange(GridDefinitions* defs, int start, int count, Float requestedSize, float percentRefSize)
{
    if (requestedSize > 0 && count > 0)
    {
        ArrayType<DefinitionBase*, false> temp;
        
        int num = start + count;
        int num2 = 0;

        Float num3 = 0.0;
        Float num4 = 0.0;
        Float num5 = 0.0;
        Float num6 = 0.0;

        temp.Reset(count, true);

        for (int i = start; i < num; i++)
        {
            DefinitionBase* defb = defs->GetItem(i);

            Float minSize = defb->GetMinSize();
            Float preferredSize = defb->GetPreferredSize();
            Float num10 = max(defb->GetUserMaxSize(), minSize);

            // 最小值之和
            num3 += minSize;
            // 实际需要值之和
            num4 += preferredSize;
            // 最大值之和
            num5 += num10;
            defb->SetSizeCache(num10);

            // 记录下最大值
            if (num6 < num10)
            {
                num6 = num10;
            }

            // 需要自动计算的列或行
            if (defb->GetSizeType() == GridSizeType::Auto)
            {
                num2++;
            }

            temp[i - start] = defb;
        }

        // 给出的值必须要大于最小值之和 
        if (FloatUtil::GreaterThan(requestedSize, num3))
        {
            // 给出值小于实际值之和
            if (FloatUtil::LessThanOrClose(requestedSize, num4))
            {
                int index = 0;
                Float num11 = requestedSize;

                SpanPreferredDistributionOrderComparer comparer;
                temp.Sort<SpanPreferredDistributionOrderComparer>(0, temp.Length(), &comparer, 0);

                // 减掉需要自动计算列或行大小
                while (index < num2)
                {
                    num11 -= temp[index]->GetMinSize();
                    index++;
                }

                while (index < count)
                {
                    Float num13 = min(num11 / ((Float)(count - index)), temp[index]->GetPreferredSize());
                    if (num13 > temp[index]->GetMinSize())
                    {
                        temp[index]->UpdateMinSize(num13);
                    }
                    num11 -= num13;
                    index++;
                }
            }
            // 给出值小于所有列或行最大值
            else if (requestedSize <= num5)
            {
                int num15 = 0;
                int iAuto = 0;
                Float num14 = requestedSize - num4;

                SpanMaxDistributionOrderComparer comparer;
                temp.Sort<SpanMaxDistributionOrderComparer>(0, temp.Length(), &comparer, 0);

                while (num15 < (count - num2))
                {
                    Float num16 = temp[num15]->GetPreferredSize();
                    Float num17 = num16 + (num14 / ((Float)((count - num2) - num15)));
                    temp[num15]->UpdateMinSize(min(num17, temp[num15]->GetSizeCache()));
                    num14 -= temp[num15]->GetMinSize() - num16;
                    num15++;
                }

                while (num15 < count)
                {
                    Float num18 = temp[num15]->GetMinSize();
                    Float num19 = num18 + (num14 / ((Float)(count - num15)));
                    temp[num15]->UpdateMinSize(min(num19, temp[num15]->GetSizeCache()));
                    num14 -= temp[num15]->GetMinSize() - num18;
                    num15++;
                }
            }
            else
            {
                Float num20 = requestedSize / ((Float)count);

                if (num20 < num6)
                {
                    Float num21 = (num6 * count) - num5;
                    Float num22 = requestedSize - num5;

                    for (int j = 0; j < count; j++)
                    {
                        Float num24 = ((num6 - temp[j]->GetSizeCache()) * num22) / num21;
                        temp[j]->UpdateMinSize(temp[j]->GetSizeCache() + num24);
                    }
                }
                else
                {
                    for (int k = 0; k < count; k++)
                    {
                        temp[k]->UpdateMinSize(num20);
                    }
                }
            }
        }
    }
}

void Grid::CalcFinalColumnSize()
{
}

void Grid::CalcFinalRowSize()
{
}

void Grid::ResolveStar(GridDefinitions* defs, float availableSize)
{
    Array<DefinitionBase*, false> temp;

    int count = defs->GetCount();
    int length = 0;
    Float num2 = 0.0f;

    for (int i = 0; i < count; i++)
    {
        DefinitionBase* defb = defs->GetItem(i);
        Float num4 = 0.0f;

        switch (defb->GetSizeType())
        {
        case GridSizeType::Pixel:
            {
                num2 += defb->GetMeasureSize();
                continue;
            }
        case GridSizeType::Auto:
            {
                num2 += defb->GetMinSize();
                continue;
            }
        case (GridSizeType::Auto | GridSizeType::Pixel):
            {
                continue;
            }
        case GridSizeType::Star:
            {
                temp.Add(defb);
                length++;

                num4 = defb->GetUserSize()->GetValue();

                if (num4 != 0)
                {
                    break;
                }
                defb->SetMeasureSize(0.0f);
                defb->SetSizeCache(0.0f);
                continue;
            }
        default:
            {
                continue;
            }
        }
        
        num4 = min(num4, Numeric::MeasureInt);
        defb->SetMeasureSize(num4);
        Float num5 = min(max(defb->GetMinSize(), defb->GetUserMaxSize()), Numeric::MeasureInt);
        defb->SetSizeCache(num5 / num4);
    }

    if (length > 0)
    {
        static StarDistributionOrderComparer comparer;
        temp.Sort<StarDistributionOrderComparer>(0, length, &comparer, 0);

        Float num6 = 0.0;
        int index = length - 1;
        do
        {
            num6 += temp[index]->GetMeasureSize();
            temp[index]->SetSizeCache(num6);
        } while (--index >= 0);

        index = 0;
        
        do
        {
            Float minSize = 0.0f;
            Float measureSize = temp[index]->GetMeasureSize();

            if (measureSize == 0)
            {
                minSize = temp[index]->GetMinSize();
            }
            else
            {
                Float num10 = max((Float)(availableSize - num2), (Float)0.0f) * (measureSize / temp[index]->GetSizeCache());
                minSize = min(num10, temp[index]->GetUserMaxSize());
                minSize = max(temp[index]->GetMinSize(), minSize);
            }
            temp[index]->SetMeasureSize(minSize);
            num2 += minSize;
        }
        while (++index < length);
    }
}

Float Grid::GetFinalSizeForRange(GridDefinitions* defs, int start, int count)
{
    Float num = 0.0;
    int index = (start + count) - 1;

    do
    {
        num += defs->GetItem(index)->GetSizeCache();
    }
    while (--index >= start);

    return num;
}

void Grid::SetFinalSize(GridDefinitions* defs, float finalSize, bool columns)
{
    int length = 0;
    int num2 = defs->GetCount();
    Float num3 = 0.0;
    Float dpiScale = 1.0;
    Array<int, false> definitionIndices;
    definitionIndices.Resize(UIMAX(_data->GetColumnDefs()->GetCount(), _data->GetRowDefs()->GetCount()));

    for (int i = 0; i < defs->GetCount(); i++)
    {
        Float userMaxSize = 0.0f;
        DefinitionBase* defb = defs->GetItem(i);

        if (defb->IsStar())
        {
            Float d = defb->GetUserSize()->GetValue();

            if (FloatUtil::IsZero(d))
            {
                defb->SetMeasureSize(0);
                defb->SetSizeCache(0);
            }
            else
            {
                d = UIMIN(d, Numeric::MeasureInt);
                defb->SetMeasureSize(d);
                Float num7 = UIMIN(UIMAX(defb->GetMinSizeForArrange(), defb->GetUserMaxSize()), Numeric::MeasureInt);
                defb->SetSizeCache(num7 / d);
            }

            definitionIndices[length++] = i;
            continue;
        }

        Float minSizeForArrange = 0.0;

        switch (defb->GetSizeType())
        {
        case GridSizeType::Auto:
            minSizeForArrange = defb->GetMinSizeForArrange();
            break;

        case GridSizeType::Pixel:
            minSizeForArrange = defb->GetUserSize()->GetValue();
            break;
        }

        if (0)
        {
            userMaxSize = minSizeForArrange;
        }
        else
        {
            userMaxSize = defb->GetUserMaxSize();
        }

        defb->SetSizeCache(max(defb->GetMinSizeForArrange(), min(minSizeForArrange, userMaxSize)));
        num3 += defb->GetSizeCache();
        definitionIndices[--num2] = i;
    }

    if (length > 0)
    {
        StarDistributionOrderIndexComparer comparer(defs);
        definitionIndices.Sort<StarDistributionOrderIndexComparer>(0, length, &comparer, 0);
        Float num10 = 0.0;
        int index = length - 1;

        do
        {
            int defIndex = definitionIndices[index];
            num10 += defs->GetItem(defIndex)->GetMeasureSize();
            defs->GetItem(defIndex)->SetSizeCache(num10);
        }
        while (--index >= 0);

        index = 0;

        do
        {
            Float num12 = 0.0f;
            int defIndex = definitionIndices[index];
            DefinitionBase* defBase = defs->GetItem(defIndex);
            int measureSize = defBase->GetMeasureSize();

            if (measureSize == 0)
            {
                num12 = defBase->GetMinSizeForArrange();
            }
            else
            {
                Float num14 = max((Float)(finalSize - num3), (Float)0.0) * ((Float)measureSize / (Float)defBase->GetSizeCache());
                num12 = min(num14, defBase->GetUserMaxSize());
                num12 = max(defBase->GetMinSizeForArrange(), num12);
            }

            defBase->SetSizeCache(num12);
            num3 += defBase->GetSizeCache();
        }
        while (++index < length);
    }

    if ((num3 > finalSize) && num3 != finalSize)
    {
        DistributionOrderIndexComparer comparer2(defs);
        definitionIndices.Sort<DistributionOrderIndexComparer>(0, defs->GetCount(), &comparer2, 0);
        Float num15 = finalSize - num3;

        for (int k = 0; k < defs->GetCount(); k++)
        {
            int num17 = definitionIndices[k];
            DefinitionBase* defTmp = defs->GetItem(num17);
            Float num18 = defTmp->GetSizeCache() + (num15 / ((Float)(defs->GetCount() - k)));
            Float num19 = num18;
            num18 = min(max(num18, defTmp->GetMinSizeForArrange()), defTmp->GetSizeCache());
            num15 -= num18 - defTmp->GetSizeCache();
            defTmp->SetSizeCache(num18);
        }
        num3 = finalSize - num15;
    }
    
    defs->GetItem(0)->SetOffset(0);
    for (int j = 0; j < defs->GetCount(); j++)
    {
        defs->GetItem((j + 1) % defs->GetCount())->SetOffset(defs->GetItem(j)->GetOffset() + defs->GetItem(j)->GetSizeCache());
    }
}

}
