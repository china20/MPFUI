/////////////////////////////////////////////////////
// GridDefWindow.cpp

#include "stdafx.h"

#include <Editor/GridDefWindow.h>


GridDefWindow::GridDefWindow(ButtonSetterEditor* btnEditor)
{
    _btnEditor = btnEditor;
    _gridElem = RTTICast<DesignGrid>(_btnEditor->GetEditTarget());

    _rowHei = NULL;
    _rowMinHei = NULL;
    _rowMaxHei = NULL;

    _columnWid = NULL;
    _columnMinWid = NULL;
    _columnMaxWid = NULL;

    _rowList = NULL;
    _columnList = NULL;
}   

GridDefWindow::~GridDefWindow()
{
}

void GridDefWindow::OnInitialized(EventArg* e)
{
    _rowHei = FindElem<GridStarControl>(_U("rowHeight"));
    _rowMinHei = FindElem<GridWidthControl>(_U("rowMinHeight"));
    _rowMaxHei = FindElem<GridWidthControl>(_U("rowMaxHeight"));

    _rowHei->AddGridValueChanged(new suic::RoutedEventHandler(this, &GridDefWindow::OnRowHeight));
    _rowMinHei->AddBaseValueChanged(new suic::RoutedEventHandler(this, &GridDefWindow::OnRowMinHeight));
    _rowMaxHei->AddBaseValueChanged(new suic::RoutedEventHandler(this, &GridDefWindow::OnRowMaxHeight));

    _columnWid = FindElem<GridStarControl>(_U("columnWidth"));
    _columnMinWid = FindElem<GridWidthControl>(_U("columnMinWidth"));
    _columnMaxWid = FindElem<GridWidthControl>(_U("columnMaxWidth"));

    _columnWid->AddGridValueChanged(new suic::RoutedEventHandler(this, &GridDefWindow::OnColumnWidth));
    _columnMinWid->AddBaseValueChanged(new suic::RoutedEventHandler(this, &GridDefWindow::OnColumnMinWidth));
    _columnMaxWid->AddBaseValueChanged(new suic::RoutedEventHandler(this, &GridDefWindow::OnColumnMaxWidth));

    _rowList = FindElem<suic::ListView>(_U("rowList"));
    _columnList = FindElem<suic::ListView>(_U("columnList"));

    InitGridData();

    //_rowList->AddSelectionChanged(new suic::SelectionChangedEventHandler(this, &GridDefWindow::OnRowListViewItemChanged));
    //_columnList->AddSelectionChanged(new suic::SelectionChangedEventHandler(this, &GridDefWindow::OnColumnListViewItemChanged));
}

void GridDefWindow::InitGridData()
{
    if (_gridElem != NULL)
    {
        for (int i = 0; i < _gridElem->GetRowCount(); ++i)
        {
            SetterCollectionNode* pNodeColl = _gridElem->GetRow(i);
            GridTypeData* gridData = new GridTypeData();
            InitGridDetails(pNodeColl, gridData, _U("Height"));
            _rowList->AddChild(gridData);
        }

        if (_rowList->GetCount() > 0)
        {
            _rowList->SetSelectedIndex(0);
        }

        for (int j = 0; j < _gridElem->GetColumnCount(); ++j)
        {
            SetterCollectionNode* pNodeColl = _gridElem->GetColumn(j);
            GridTypeData* gridData = new GridTypeData();
            InitGridDetails(pNodeColl, gridData, _U("Width"));
            _columnList->AddChild(gridData);
        }

        if (_columnList->GetCount() > 0)
        {
            _columnList->SetSelectedIndex(0);
        }
    }
}

void GridDefWindow::InitGridDetails(SetterCollectionNode* pNodeColl, GridTypeData* gridData, const String& name)
{
    SetterNode* pNode = pNodeColl->FindSetter(name);
    if (NULL != pNode)
    {
        GridSize* gSize = RTTICast<GridSize>(pNode->GetValue());
        if (NULL != gSize)
        {
            gridData->SetGridSize(gSize->GetValue(), (suic::GridSizeType)gSize->GetGridSizeType());
        }
    }

    pNode = pNodeColl->FindSetter(suic::String().Format(_U("Min%s"), name.c_str()));
    if (NULL != pNode)
    {
        gridData->SetMinSize(pNode->GetValue()->ToInt());
    }

    pNode = pNodeColl->FindSetter(suic::String().Format(_U("Max%s"), name.c_str()));
    if (NULL != pNode)
    {
        gridData->SetMaxSize(pNode->GetValue()->ToInt());
    }
}

void GridDefWindow::OnLoaded(suic::LoadedEventArg* e)
{
}

void GridDefWindow::OnColumnListViewItemChanged(suic::Element* sender, suic::SelectionChangedEventArg* e)
{
    if (e->GetAddedItems()->GetCount() > 0)
    {
        GridTypeData* pItem = DynamicCast<GridTypeData>(e->GetAddedItems()->GetItem(0));
        _columnWid->UpdateValue(pItem->GetGridSizeValue(), (suic::GridSizeType)pItem->GetGridSizeType());
        _columnMinWid->InitFloat(pItem->GetMinValue());
        _columnMaxWid->InitFloat(pItem->GetMaxValue());
    }
    e->SetHandled(true);
}

void GridDefWindow::OnRowListViewItemChanged(suic::Element* sender, suic::SelectionChangedEventArg* e)
{
    if (e->GetAddedItems()->GetCount() > 0)
    {
        GridTypeData* pItem = DynamicCast<GridTypeData>(e->GetAddedItems()->GetItem(0));
        _rowHei->UpdateValue(pItem->GetGridSizeValue(), (suic::GridSizeType)pItem->GetGridSizeType());
        _rowMinHei->InitFloat(pItem->GetMinValue());
        _rowMaxHei->InitFloat(pItem->GetMaxValue());
    }
    e->SetHandled(true);
}

void GridDefWindow::NotifyValueChanged()
{
    if (NULL != _btnEditor)
    {
        _btnEditor->RefleshSetterToDesignUI(true);
        if (NULL != _gridElem->GetUIElement())
        {
            _gridElem->GetUIElement()->UpdateLayout();
        }
    }
}

void GridDefWindow::OnAddGridColumn(suic::DpObject* sender, suic::RoutedEventArg* e)
{
    SetterNode* setterNode = NULL;
    GridTypeData* gridData = new GridTypeData();
    int index = _columnList->GetCount();

    _columnList->AddChild(gridData);
    _gridElem->AddColumn();
    _gridElem->SetColumnWidth(index, gridData->GetGridSizeType(), gridData->GetGridSizeValue());
    _gridElem->SetColumnMinWidth(index, gridData->GetMinValue());
    _gridElem->SetColumnMaxWidth(index, gridData->GetMaxValue());

    _columnList->SetSelectedIndex(index);

    NotifyValueChanged();
    e->SetHandled(true);
}

void GridDefWindow::OnRemoveGridColumn(suic::DpObject* sender, suic::RoutedEventArg* e)
{
    int iSel = GetColumnSel();
    if (iSel >= 0)
    {
        _columnList->GetItemsSource()->RemoveItemAt(iSel);
        _gridElem->RemoveColumnAt(iSel);

        if (_columnList->GetCount() > 0)
        {
            _columnList->SetSelectedIndex(0);
        }
    }
    NotifyValueChanged();
    e->SetHandled(true);
}

void GridDefWindow::OnAddGridRow(suic::DpObject* sender, suic::RoutedEventArg* e)
{
    SetterNode* setterNode = NULL;
    GridTypeData* gridData = new GridTypeData();
    int index = _rowList->GetCount();

    _rowList->AddChild(gridData);
    _gridElem->AddRow();

    _gridElem->SetRowHeight(index, gridData->GetGridSizeType(), gridData->GetGridSizeValue());
    _gridElem->SetRowMinHeight(index, gridData->GetMinValue());
    _gridElem->SetRowMaxHeight(index, gridData->GetMaxValue());

    _rowList->SetSelectedIndex(index);
    NotifyValueChanged();
    e->SetHandled(true);
}

void GridDefWindow::OnRemoveGridRow(suic::DpObject* sender, suic::RoutedEventArg* e)
{
    int iSel = GetRowSel();
    if (iSel >= 0)
    {
        _rowList->GetItemsSource()->RemoveItemAt(iSel);
        _gridElem->RemoveRowAt(iSel);
        if (_rowList->GetCount() > 0)
        {
            _rowList->SetSelectedIndex(0);
        }
    }
    NotifyValueChanged();
    e->SetHandled(true);
}

int GridDefWindow::GetRowSel()
{
    suic::ListView* rowList = FindElem<suic::ListView>(_U("rowList"));
    return rowList->GetSelectedIndex();
}

int GridDefWindow::GetColumnSel()
{
    suic::ListView* columnList = FindElem<suic::ListView>(_U("columnList"));
    return columnList->GetSelectedIndex();
}

void GridDefWindow::OnColumnWidth(suic::DpObject* sender, suic::RoutedEventArg* e)
{
    int iSel = GetColumnSel();
    if (iSel >= 0)
    {
        SetterNode* pEditor = _btnEditor->GetSetterNode(true);
        GridTypeData* pItem = DynamicCast<GridTypeData>(_columnList->GetItem(iSel));
        pItem->SetGridSize(_columnWid->GetGridValue(), _columnWid->GetGridType());
        pItem->NotifyChanged(_U(""));
        _gridElem->SetColumnWidth(iSel, _columnWid->GetGridType(), _columnWid->GetGridValue());
        NotifyValueChanged();
    }
}

void GridDefWindow::OnColumnMinWidth(suic::DpObject* sender, suic::RoutedEventArg* e)
{
    int iSel = GetColumnSel();
    if (iSel >= 0)
    {
        SetterNode* pEditor = _btnEditor->GetSetterNode(true);
        GridTypeData* pItem = DynamicCast<GridTypeData>(_columnList->GetItem(iSel));
        pItem->SetMinSize(_columnMinWid->GetFloat());
        pItem->NotifyChanged(_U(""));
        _gridElem->SetColumnMinWidth(iSel, _columnMinWid->GetFloat());
        NotifyValueChanged();
    }
}

void GridDefWindow::OnColumnMaxWidth(suic::DpObject* sender, suic::RoutedEventArg* e)
{
    int iSel = GetColumnSel();
    if (iSel >= 0)
    {
        SetterNode* pEditor = _btnEditor->GetSetterNode(true);
        GridTypeData* pItem = DynamicCast<GridTypeData>(_columnList->GetItem(iSel));
        pItem->SetMaxSize(_columnMaxWid->GetFloat());
        pItem->NotifyChanged(_U(""));
        _gridElem->SetColumnMaxWidth(iSel, _columnMaxWid->GetFloat());
        NotifyValueChanged();
    }
}

void GridDefWindow::OnRowHeight(suic::DpObject* sender, suic::RoutedEventArg* e)
{
    int iSel = GetRowSel();
    if (iSel >= 0)
    {
        SetterNode* pEditor = _btnEditor->GetSetterNode(true);
        GridTypeData* pItem = DynamicCast<GridTypeData>(_rowList->GetItem(iSel));
        pItem->SetGridSize(_rowHei->GetGridValue(), _rowHei->GetGridType());
        pItem->NotifyChanged(_U(""));
        _gridElem->SetRowHeight(iSel, _rowHei->GetGridType(), _rowHei->GetGridValue());
        NotifyValueChanged();
    }
}

void GridDefWindow::OnRowMinHeight(suic::DpObject* sender, suic::RoutedEventArg* e)
{
    int iSel = GetRowSel();
    if (iSel >= 0)
    {
        SetterNode* pEditor = _btnEditor->GetSetterNode(true);
        GridTypeData* pItem = DynamicCast<GridTypeData>(_rowList->GetItem(iSel));
        pItem->SetMinSize(_rowMinHei->GetFloat());
        pItem->NotifyChanged(_U(""));
        _gridElem->SetRowMinHeight(iSel, _rowMinHei->GetFloat());
        NotifyValueChanged();
    }
}

void GridDefWindow::OnRowMaxHeight(suic::DpObject* sender, suic::RoutedEventArg* e)
{
    int iSel = GetRowSel();
    if (iSel >= 0)
    {
        SetterNode* pEditor = _btnEditor->GetSetterNode(true);
        GridTypeData* pItem = DynamicCast<GridTypeData>(_rowList->GetItem(iSel));
        pItem->SetMaxSize(_rowMaxHei->GetFloat());
        pItem->NotifyChanged(_U(""));
        _gridElem->SetRowMaxHeight(iSel, _rowMaxHei->GetFloat());
        NotifyValueChanged();
    }
}
