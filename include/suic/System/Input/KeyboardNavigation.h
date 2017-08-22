// 华勤科技版权所有 2010-2011
// 
// 文件名：KeyboardNavigation.h
// 功  能：实现键盘的导航。
// 
// 作  者：MPF开发组
// 时  间：2010-07-02
// 
// ============================================================================

#ifndef _UIKEYBOARDNAVIGATION_H_
#define _UIKEYBOARDNAVIGATION_H_

#include <System/Windows/Element.h>
#include <Framework/Controls/Decorator.h>

namespace suic
{

class FocusVisualAdorner : public Adorner
{
private:
    
    Element* _child;

public:

    RTTIOfClass(FocusVisualAdorner)

    FocusVisualAdorner();
    FocusVisualAdorner(Element* elem, Style* style);
    ~FocusVisualAdorner();

    int GetVisualChildrenCount();
    Element* GetVisualChild(int index);

protected:

    Size OnMeasure(const Size& constraint);
    void OnArrange(const Size& arrangeSize);
};

class SUICORE_API KeyboardNavigation
{
protected:

    KeyboardNavigation();
    
public:

    static DpProperty* TabIndexProperty;
    static DpProperty* IsTabStopProperty;
    static DpProperty* AcceptsReturnProperty;
    static DpProperty* TabNavigationProperty;
    static DpProperty* TabOnceActiveElementProperty;
    static DpProperty* ControlTabNavigationProperty;
    static DpProperty* ControlTabOnceActiveElementProperty;
    static DpProperty* DirectionalNavigationProperty;

    static bool StaticInit();

    static bool IsTabStop(Element* elem);
    static int GetTabIndex(Element* elem);
    static bool IsAcceptsReturn(Element* elem);
    static void UpdateFocusedElement(Element* elem);

    static void SetTabOnceActiveElement(Element* elem, DpObject* val);
    static void SetDirectionalNavigation(Element* element, KNavMode mode);
    static void SetControlTabOnceActiveElement(Element* elem, DpObject* val);

    static bool GetAlwaysShowFocusVisual();
    static void SetAlwaysShowFocusVisual(bool val);

    static void ShowFocusVisual();
    static KeyboardNavigation* Current();

    RTTIOfAbstractClass(KeyboardNavigation)

    void HideFocusVisual();
    FrameworkElement* PredictFocusedElement(Element* elem, FocusNavDirection dir);
 
    virtual bool ProcessInput(Element* rootElement, MessageParam* e);

protected:

    virtual void UpdateActiveElement(Element* activeElement, DpProperty* dp);
    virtual void UpdateActiveElement(Element* container, Element* activeElement, DpProperty* dp);

    virtual void UpdateActiveElement(Element* elem);
    virtual void NotifyFocusChanged(Object* sender, KeyboardFocusEventArg* e);

    virtual void NotifyFocusEnterMainFocusScope(Object* sender, EventArg* e);
    virtual void ShowFocusVisual(Element* elem);
    virtual bool Navigate(Element* elem, FocusNavDirection req, ModifierKey modKeys, Element* firstElem);

    friend class FrameworkElement;

protected:

    DpProperty* _navigationProperty;
    FocusVisualAdorner* _focusVisualAdornerCache;

    static KeyboardNavigation* _keyNav;
    static bool _alwaysShowFocusVisual;
};

}

#endif
