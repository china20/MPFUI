// 华勤科技版权所有 2008-2022
// 
// 文件名：ListBox.h
// 功  能：列表框。
// 
// 作  者：MPF开发组
// 时  间：2010-11-02
// 
// ============================================================================

#ifndef _UILISTBOX_H_
#define _UILISTBOX_H_

#include <Extend/suiwgxconfig.h>
#include <Framework/Controls/Selector.h>
#include <Extend/Controls/ListBoxItem.h>

namespace suic
{

class SUIWGX_API ListBox : public Selector
{
public:

    RTTIOfClass(ListBox)

    static DpProperty* SelectionModeProperty;

    static void OnSelectionModePropChanged(suic::DpObject* d, suic::DpPropChangedEventArg* e);

    static bool StaticInit();

    ListBox();
    ~ListBox();

    SelectionMode GetSelectionMode();
    void SetSelectionMode(SelectionMode val);

    void SelectAll();
    void UnselectAll();

    void ScrollToStart();
    void ScrollToEnd();
    void ScrollIntoView(Object* item);

public:

    bool IsItemItsOwnContainer(Object* item);

    void OnKeyDown(KeyboardEventArg* e);
    void OnMouseLeftButtonDown(MouseButtonEventArg* e);

protected:

    void MakeSingleSelection(ListBoxItem* item);
    void MakeToggleSelection(ListBoxItem* item);
    void MakeAnchorSelection(ListBoxItem* actionItem, bool clearCurrent);

    Object* GetAnchorItem();
    void SetAnchorItem(Object* item);
    
protected:
    
    void NotifyListItemMouseDragged(ListBoxItem* listItem);
    void NotifyListItemClicked(ListBoxItem* item, MouseButton mouseButton);
    void HandleSelectedItem(Object* container, MouseButton mouseButton);

    void UpdateAnchorAndActionItem(ListBoxItem* listItem);

    void OnClearItem(DpObject* elem, ItemEntry* item);
    void PrepareContainerForItemOverride(DpObject* elem, ItemEntry* item);

    Element* GetContainerForItemOverride();

    virtual void SelectAllImpl();
    virtual void UnselectAllImpl();

    friend class ListBoxItem;

private:

    WeakRef _anchorItem;
    WeakRef _lastActionItem;
};

}

#endif
