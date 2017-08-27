// 华勤科技版权所有 2010-2011
// 
// 文件名：GridDefWindow.h
// 功  能：Grid的行列编辑界面。
// 
// 作  者：汪荣
// 时  间：2013-01-02
// 
// ============================================================================

#ifndef _GRIDDEFWINDOW_H_
#define _GRIDDEFWINDOW_H_

#include <Main/FilterNode.h>
#include <Core/TypeControls.h>
#include <Core/ValueEditor.h>
#include <Main/XamlRootItem.h>

using namespace suic;

class GridTypeData : public suic::NotifyPropChanged
{
public:

    BeginMember(GridTypeData, suic::NotifyPropChanged)
        MemberGetString(Size)
        MemberGetString(MinSize)
        MemberGetString(MaxSize)
    EndMember()

    GridTypeData()
    {
        _value = 1;
        _sizeType = suic::GridSizeType::Star;
        _minSize = -1;
        _maxSize = -1;
    }

    void SetGridSize(suic::Float val, suic::GridSizeType type)
    {
        _value = val;
        _sizeType = type;
    }

    suic::Float GetGridSizeValue()
    {
        return _value;
    }

    suic::GridSizeType GetGridSizeType()
    {
        return _sizeType;
    }

    suic::String GetSize()
    {
        switch (_sizeType)
        {
            case suic::GridSizeType::Auto:
                return suic::String(_U("Auto"));

            case suic::GridSizeType::Pixel:
                return suic::String().Format(_U("%d"), (int)_value);

            default:
                return suic::String().Format(_U("%.2f*"), _value);
        }
    }

    int GetMinValue()
    {
        return _minSize;
    }

    suic::String GetMinSize()
    {
        if (_minSize < 0)
        {
            return suic::String(_U("Auto"));
        }
        return suic::String().Format(_U("%d"), _minSize);
    }

    void SetMinSize(int val)
    {
        _minSize = val;
    }

    int GetMaxValue()
    {
        return _maxSize;
    }

    suic::String GetMaxSize()
    {
        if (_maxSize < 0)
        {
            return suic::String(_U("Auto"));
        }
        return suic::String().Format(_U("%d"), _maxSize);
    }

    void SetMaxSize(int val)
    {
        _maxSize = val;
    }

private:

    int _minSize;
    int _maxSize;

    suic::Float _value;
    suic::GridSizeType _sizeType;
};

class GridDefWindow : public suic::Window
{
public:
    
    // 事件映射
    BeginRoutedEvent(GridDefWindow, suic::Window)
        MemberRouted(OnAddGridRow)
        MemberRouted(OnRemoveGridRow)
        MemberRouted(OnAddGridColumn)
        MemberRouted(OnRemoveGridColumn)
        MemberEvent(OnRowListViewItemChanged,suic::Element,suic::SelectionChangedEventArg)
        MemberEvent(OnColumnListViewItemChanged,suic::Element,suic::SelectionChangedEventArg)
    EndRoutedEvent()

    GridDefWindow(ButtonSetterEditor* btnEditor);
    ~GridDefWindow();

protected:

    void OnColumnListViewItemChanged(suic::Element* sender, suic::SelectionChangedEventArg* e);
    void OnRowListViewItemChanged(suic::Element* sender, suic::SelectionChangedEventArg* e);

    void OnAddGridColumn(suic::DpObject* sender, suic::RoutedEventArg* e);
    void OnRemoveGridColumn(suic::DpObject* sender, suic::RoutedEventArg* e);

    void OnAddGridRow(suic::DpObject* sender, suic::RoutedEventArg* e);
    void OnRemoveGridRow(suic::DpObject* sender, suic::RoutedEventArg* e);

    void OnColumnWidth(suic::DpObject* sender, suic::RoutedEventArg* e);
    void OnColumnMinWidth(suic::DpObject* sender, suic::RoutedEventArg* e);
    void OnColumnMaxWidth(suic::DpObject* sender, suic::RoutedEventArg* e);

    void OnRowHeight(suic::DpObject* sender, suic::RoutedEventArg* e);
    void OnRowMinHeight(suic::DpObject* sender, suic::RoutedEventArg* e);
    void OnRowMaxHeight(suic::DpObject* sender, suic::RoutedEventArg* e);

    int GetRowSel();
    int GetColumnSel();

    void NotifyValueChanged();

protected:

    void OnInitialized(EventArg* e);
    void OnLoaded(suic::LoadedEventArg* e);

    void InitGridData();
    void InitGridDetails(SetterCollectionNode* pNodeColl, GridTypeData* gridData, const String& name);

private:

    DesignGrid* _gridElem;
    ButtonSetterEditor* _btnEditor;

    GridStarControl* _rowHei;
    GridWidthControl* _rowMinHei;
    GridWidthControl* _rowMaxHei;

    GridStarControl* _columnWid;
    GridWidthControl* _columnMinWid;
    GridWidthControl* _columnMaxWid;

    suic::ListView* _rowList;
    suic::ListView* _columnList;
};

#endif
