// ======================================================================
//
// Copyright (c) 2008-2022 汪荣(视点UI), Inc. All rights reserved.
//
// MPF界面库遵循指定开源协议，团体或公司商用请根据协议购买授权，
// 任何个人、团体或公司不能居于此库衍生任何商业性质的库或代码。
//
// ======================================================================

/////////////////////////////////////////////////////////////////////////
// MenuItem.cpp

#include <Framework/Controls/Menu.h>

namespace suic
{

class TrackingMenuPopupHook : public Object
{
public:

    TrackingMenuPopupHook();

    static TrackingMenuPopupHook* Ins();

    void TrackContextMenu(Popup* popup, Element* menu, int x, int y);
    void TrackMenuPopup(Popup* popup, Element* item, Element* menu);
    void TrackSubMenuPopup(Popup* popup, Element* menu);

    void SetTrackingMenu(Popup* popup, Element* menu);

    bool HandleMouseDown(Handle hwnd, bool& bOverTree);
    bool HandleMouseUp(Handle hwnd, bool& bOverTree);
    bool HandleMouseMove(MessageParam* mp, bool& bOverTree);
    bool IsMouseOverOwer();

    MenuItem* GetMenuItem(Element* elem);

    void OnRootPopupClosing(Object* source, EventArg* e);
    bool OnFilterMessage(Object* sender, MessageParam* mp, bool& interrupt);

    void ClosePopup(Element* elem);
    void CloseTopPopup();

    void HideAllPopup();
    void CloseAllPopup(bool bAsync);

    Element* HitTestPopup(Handle hwnd, bool& bOver, bool& bOverTree);
    bool IsMouseCapture() const;
    void SetMouseCapture(bool value);

    void AddPopupMenuItem(Popup* popup);

    Element* GetTrackingMenu()
    {
        return _trackingMenu;
    }

    Popup* GetRootPopup() const
    {
        return _popup;
    }

    Element* GetTrackingMenuItem()
    {
        return _trackingMenuItem;
    }

    MenuItem* GetFocusMenuItem()
    {
        return _focusMenuItem;
    }

    bool IsInTracking()
    {
        return (NULL != _trackingMenu);
    }

protected:

    Popup* _popup;
    XArray _popups;

    // 弹出的根菜单窗口
    Element* _trackingMenu;
    Element* _trackingMenuItem;
    MenuItem* _focusMenuItem;
    bool _mouseCapture;
};

#define GetMenuPopup() TrackingMenuPopupHook::Ins()

}
