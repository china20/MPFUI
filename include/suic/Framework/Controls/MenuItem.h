// 华勤科技版权所有 2008-2022
// 
// 文件名：MenuItem.h
// 功  能：实现菜单项逻辑。
// 
// 作  者：MPF开发组
// 时  间：2011-07-02
// 
// ============================================================================

#ifndef _UIMENUITEM_H_
#define _UIMENUITEM_H_

#include <Framework/Controls/Popup.h>
#include <Framework/Controls/Selector.h>
#include <Framework/Controls/HeaderedItemsControl.h>

namespace suic
{

/// <summary>
///  MenuItem，实现标准的菜单项。
/// </summary>
class MenuItem;
typedef shared<MenuItem> MenuItemPtr;

enum MenuItemRole
{ 
    TopLevelItem,
    TopLevelHeader, 
    SubmenuItem, 
    SubmenuHeader,
};

struct SUICORE_API MenuItemRoleBox
{
    static Integer* TopLevelItemBox;
    static Integer* TopLevelHeaderBox;
    static Integer* SubmenuItemBox;
    static Integer* SubmenuHeaderBox;
};

class SUICORE_API MenuItem : public HeaderedItemsControl
{
public:

    static RoutedEvent* ClickEvent;
    static RoutedEvent* CheckedEvent;
    static RoutedEvent* UncheckedEvent;

    static RoutedEvent* SubmenuOpenedEvent;
    static RoutedEvent* SubmenuClosedEvent;

    static DpProperty* IconProperty;
    static DpProperty* RoleProperty;
    static DpProperty* IsSubmenuOpenProperty;
    static DpProperty* IsCheckedProperty;
    static DpProperty* IsCheckableProperty;
    static DpProperty* IsPressedProperty;
    static DpProperty* IsHighlightedProperty;

    static DpProperty* IsSeparatorProperty;

    MenuItem();
    virtual ~MenuItem();

    void Dispose();

    static void StaticInit();

    static void OnIsCheckedPropChanged(DpObject* d, DpPropChangedEventArg* e);
    static void OnIsSeparatorPropChanged(DpObject* d, DpPropChangedEventArg* e);
    static void OnIsCheckablePropChanged(DpObject* target, DpPropChangedEventArg* e);
    static void OnIsSelectedPropChanged(DpObject* d, DpPropChangedEventArg* e);

    RTTIOfClass(MenuItem)

    void CloseSubmenu();
    MenuItemRole GetRole();

    ImageSource* GetIcon();
    void SetIcon(ImageSource* val);

    bool IsSubmenuOpen();
    bool IsSubmenu();

    bool IsSelected();
    void SetIsSelected(bool val);

    bool IsCheckable();
    void SetIsCheckable(bool val);

    bool IsSeparator();
    bool IsHighlighted();

    bool IsChecked();
    void SetIsChecked(bool val);

    ICommandSource* GetCommandSource();

    Element* GetContainerForItemOverride();
    bool IsItemItsOwnContainer(Object* item);

public:

    void OnPopupClosed(Object* source, EventArg* e);
    void OnPopupOpened(Object* source, EventArg* e);

public:

    virtual void OnClick();

    virtual void OnSubmenuOpened(RoutedEventArg* e);
    virtual void OnSubmenuClosed(RoutedEventArg* e);

    virtual void OnChecked(RoutedEventArg* e);
    virtual void OnUnchecked(RoutedEventArg* e);

protected:

    void OnApplyTemplate();
    void OnInitialized(EventArg* e);

    void CloseChildSubmenu();

    void HandleMenuItemEnter(bool fromKey);
    void HandleMenuItemClick();
    void HandleLeftButtonDown();

    void SetIsHighlighted(bool val);

    static void SetIsHighlighted(DpObject* o, bool v);

    void OnItemsChanged(NotifyCollChangedEventArg* e);
    void OnVisualParentChanged(Element* oldParent);

    void OnKeyDown(KeyboardEventArg* e);

    void OnMouseEnter(MouseButtonEventArg* e);
    void OnMouseLeave(MouseButtonEventArg* e);
    void OnMouseLeftButtonDown(MouseButtonEventArg* e);
    void OnMouseLeftButtonUp(MouseButtonEventArg* e);

    void PrepareHeaderedItemsControl(ItemEntry* item, ItemsControl* parentItemsControl);

private:

    void UpdateRole();
    Element* GetTargetElement();
    Selector* GetParentSelector();
    void SetIsSeparator(bool val);

protected:

    bool _fromKey;
    bool _isSeparator;
    PopupPtr _submenuPopup;
    ICommandSource* _commandSource;

    friend class MenuBase;
};

inline ImageSource* MenuItem::GetIcon()
{
    return RTTICast<ImageSource>(GetValue(IconProperty));
}

inline void MenuItem::SetIcon(ImageSource* val)
{
    SetValue(IconProperty, val);
}

inline MenuItemRole MenuItem::GetRole()
{
    return (MenuItemRole)GetValue(RoleProperty)->ToInt();
}

inline bool MenuItem::IsCheckable()
{
    return GetValue(IsCheckableProperty)->ToBool();
}

inline void MenuItem::SetIsCheckable(bool val)
{
    SetValue(IsCheckableProperty, BOOLTOBOOLEAN(val));
}

inline bool MenuItem::IsSeparator()
{
    return _isSeparator;
}

inline bool MenuItem::IsHighlighted()
{
    return GetValue(IsHighlightedProperty)->ToBool();
}

inline void MenuItem::SetIsSeparator(bool val)
{
    SetValue(IsSeparatorProperty, BOOLTOBOOLEAN(val));
}

inline bool MenuItem::IsChecked()
{
    return GetValue(IsCheckedProperty)->ToBool();
}

inline void MenuItem::SetIsChecked(bool val)
{
    SetValue(IsCheckedProperty, BOOLTOBOOLEAN(val));
}

};

#endif
