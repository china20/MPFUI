// 华勤科技版权所有 2010-2011
// 
// 文件名：Menu.h
// 功  能：实现菜单逻辑。
// 
// 作  者：MPF开发组
// 时  间：2011-07-02
// 
// ============================================================================

#ifndef _UIMENU_H_
#define _UIMENU_H_

#include <Framework/Controls/Selector.h>
#include <Framework/Controls/MenuItem.h>

namespace suic
{

/// <Summary>
/// Menu，实现标准的列表框。
/// <Summary/>

class MenuBase;
typedef shared<MenuBase> MenuBasePtr;

class SUICORE_API MenuBase : public Selector
{
public:

    static bool StaticInit();

    MenuBase();
    virtual ~MenuBase();

    RTTIOfClass(MenuBase)

    void Dispose();

    MenuBase* GetOwner() const;
    void SetOwner(MenuBase* owner);

public:

    Element* GetContainerForItemOverride();
    bool IsItemItsOwnContainer(Object* item);

    bool BlockRouteEvent() const;

    void OnApplyTemplate();
    void OnInitialized(EventArg* e);

    void OnRender(Drawing * drawing);

    void OnTextInput(KeyboardEventArg* e);
    void OnKeyDown(KeyboardEventArg* e);

    void OnMouseEnter(MouseButtonEventArg* e);
    void OnMouseMove(MouseButtonEventArg* e);
    void OnMouseLeave(MouseButtonEventArg* e);
    void OnMouseLeftButtonUp(MouseButtonEventArg* e);

protected:

    MenuBase* _ownerMenu;
    MenuItemPtr _currentSelection;
};

class SUICORE_API Menu : public MenuBase
{
public:

    static bool StaticInit();

    Menu();
    virtual ~Menu();

    RTTIOfClass(Menu)
};

typedef suic::shared<Menu> MenuPtr;

class SUICORE_API ContextMenu : public MenuBase
{
public:

    static DpProperty* IsOpenProperty;
    static DpProperty* PlacementProperty;
    static DpProperty* PlacementRectangleProperty;
    static DpProperty* PlacementTargetProperty;
    static DpProperty* VerticalOffsetProperty;
    static DpProperty* HorizontalOffsetProperty;

    static bool StaticInit();
    static void CloseContextMenu();

    static void OnIsOpenPropChanged(DpObject* d, DpPropChangedEventArg* e);

    ContextMenu();
    virtual ~ContextMenu();

    RTTIOfClass(ContextMenu)

    bool IsOpen();
    void SetIsOpen(bool val);

    PlacementMode GetPlacement();
    void SetPlacement(PlacementMode val);

    Rect GetPlacementRectangle();
    void GetPlacementRectangle(Rect val);

    Element* GetPlacementTarget();
    void SetPlacementTarget(Element* target);

    int GetVerticalOffset();
    void SetVerticalOffset(int val);

    int GetHorizontalOffset();
    void SetHorizontalOffset(int val);

    Popup* GetParentPopup();

    void TrackContextMenu();

    void OnPopupOpened(Object* sender, EventArg* e);
    void OnPopupClosed(Object* sender, EventArg* e);

private:

    void HookupParentPopup();

private:

    Popup* _parentPopup;
};

typedef suic::shared<ContextMenu> ContextMenuPtr;

}

#endif