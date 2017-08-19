// 华勤科技版权所有 2010-2011
// 
// 文件名：ListBoxItem.h
// 功  能：列表框项。
// 
// 作  者：MPF开发组
// 时  间：2010-11-02
// 
// ============================================================================

#ifndef _UILISTBOXITEM_H_
#define _UILISTBOXITEM_H_

#include <Extend/suiwgxconfig.h>
#include <Framework/Controls/Content.h>

namespace suic
{

class Selector;
class ListBox;

class SUIWGX_API ListBoxItem : public ContentControl
{
public:

    enum SelectedFlag
    {
        sfSelected = 1,
        sfFromMouse = 2,
    };
 
    static DpProperty* IsSelectedProperty;
    
    static RoutedEvent* SelectedEvent;
    static RoutedEvent* UnselectedEvent;

    static void OnIsSelectedChanged(DpObject* d, DpPropChangedEventArg* e);

    ListBoxItem();
    ~ListBoxItem();

    RTTIOfClass(ListBoxItem)

    ListBox* GetParentListBox();
    Selector* GetParentSelector();

    bool GetIsSelected();
    void SetIsSelected(bool val);

protected:

    void OnPreApplyTemplate();
    void OnInitialized(EventArg* e);

    void OnMouseEnter(MouseButtonEventArg* e);
    void OnMouseLeave(MouseButtonEventArg* e);
    void OnMouseLeftButtonDown(MouseButtonEventArg* e);
    void OnMouseRightButtonDown(MouseButtonEventArg* e);
    void OnVisualParentChanged(DpObject* oldParent);

    virtual void OnSelected(RoutedEventArg* e);
    virtual void OnUnselected(RoutedEventArg* e);

    bool ContainsSelectedFlag(Byte k);
    void SetSelectedFlag(Byte k, bool v);

    static bool StaticInit();

private:

    void HandleIsSelectedChanged(bool newValue, RoutedEventArg* e);
    void HandleMouseButtonDown(MouseButton mouseButton);

protected:

    Byte _selectedFlag;
    friend class ListBox;
};

inline bool ListBoxItem::ContainsSelectedFlag(Byte k)
{
    return (k == (_selectedFlag & k));
}

inline void ListBoxItem::SetSelectedFlag(Byte k, bool v)
{
    if (v)
    {
        _selectedFlag |= k;
    }
    else
    {
        _selectedFlag &= ~k;
    }
}

}

#endif
