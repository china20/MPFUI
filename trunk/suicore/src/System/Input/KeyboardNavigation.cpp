/////////////////////////////////////////////////////////////////////////
// KeyboardNavigation.cpp

#include <System/Input/KeyboardNavigation.h>
#include <System/Input/FocusManager.h>
#include <System/Input/Keyboard.h>

#include <System/Windows/WeakRefObj.h>
#include <System/Windows/HwndSubclass.h>
#include <System/Windows/InheritStateProperty.h>

#include <System/Tools/Debug.h>
#include <System/Tools/HwndHelper.h>
#include <System/Tools/VisualTreeOp.h>
#include <System/Interop/InternalOper.h>

namespace suic
{

DpProperty* KeyboardNavigation::TabIndexProperty;
DpProperty* KeyboardNavigation::IsTabStopProperty;
DpProperty* KeyboardNavigation::AcceptsReturnProperty;

DpProperty* KeyboardNavigation::TabOnceActiveElementProperty;
DpProperty* KeyboardNavigation::TabNavigationProperty;
DpProperty* KeyboardNavigation::ControlTabNavigationProperty;
DpProperty* KeyboardNavigation::ControlTabOnceActiveElementProperty;
DpProperty* KeyboardNavigation::DirectionalNavigationProperty;

KeyboardNavigation* KeyboardNavigation::_keyNav = NULL;
bool KeyboardNavigation::_alwaysShowFocusVisual = false;

ImplementRTTIOfClass(FocusVisualAdorner, Adorner)
ImplementRTTIOfClass(KeyboardNavigation, Object)

class KNavModeConvert : public IConvertValue
{
public:

    static KNavModeConvert* Convert;

    KNavModeConvert()
    {
    }

    AutoObj KNavModeConvert::ConvertValue(DpProperty* dp, Object* prop)
    {
        String val = prop->ToString();

        if (val.Equals(_T("None")))
        {
            return Integer::GetPosInt(KNavMode::knNone);
        }
        else if (val.Equals(_T("Once")))
        {
            return Integer::GetPosInt(KNavMode::knOnce);
        }
        else if (val.Equals(_T("Cycle")))
        {
            return Integer::GetPosInt(KNavMode::knCycle);
        }
        else if (val.Equals(_T("Contained")))
        {
            return Integer::GetPosInt(KNavMode::knContained);
        }
        else if (val.Equals(_T("Local")))
        {
            return Integer::GetPosInt(KNavMode::knLocal);
        }
        else
        {
            return Integer::GetPosInt(KNavMode::knContinue);     
        }
    }

    String ToString(DpProperty* dp, Object* prop)
    {
        String val;
        int iVal = prop->ToInt();

        switch (iVal)
        {
        case KNavMode::knNone:
            val = _U("None");
            break;

        case KNavMode::knOnce:
            val = _U("Once");
            break;

        case KNavMode::knCycle:
            val = _U("Cycle");
            break;

        case KNavMode::knContained:
            val = _U("Contained");
            break;

        case KNavMode::knLocal:
            val = _U("Local");
            break;

        default:
            val = _U("Continue");
        }

        return val;
    }
};

KNavModeConvert* KNavModeConvert::Convert = new KNavModeConvert();

static bool IsKeyboardMostRecentInputDevice()
{
    return (FocusManager::GetCurrentInputDevice() == KeyboardDeice::Current());
}

bool KeyboardNavigation::GetAlwaysShowFocusVisual()
{
    return _alwaysShowFocusVisual;
}

void KeyboardNavigation::SetAlwaysShowFocusVisual(bool val)
{
    _alwaysShowFocusVisual = val;
}

class KeyboardNavigationImpl : public KeyboardNavigation
{
public:

    KeyboardNavigationImpl() {}

    bool ProcessInput(Element* rootElement, MessageParam* e);

    static Element* GetTabOnceActiveElement(Element* d);
    static Element* GetControlTabOnceActiveElement(Element* d);

protected:

    void UpdateActiveElement(Element* activeElement, DpProperty* dp);
    void UpdateActiveElement(Element* container, Element* activeElement, DpProperty* dp);
    bool Navigate(Element* elem, FocusNavDirection req, ModifierKey modKeys, Element* firstElem);

private:

    bool IsGroup(Element* elem);
    bool IsTabStopOrGroup(Element* elem);
    bool IsElementVisible(Element* elem);

    bool IsFocusScope(Element* elem);
    Element* GetFocusedElement(Element* elem);

    Element* GetFirstChild(Element* elem);
    Element* GetLastChild(Element* elem);
    Element* GetValidParent(Element* elem);
    KNavMode GetKNavMode(Element* elem);

    Element* FindNextHwnd(Element* elem, bool bNext);

    Element* GetActiveElement(Element* d);
    void SetActiveElement(Element* d, Element* value);

    void SetNavigationProperty(ModifierKey modKeys);
    Element* GetGroupParent(Element* elem, bool includeSelf);

    Element* GetPrevTab(Element* elem, Element* container, bool goDownOnly);
    Element* GetPrevTabInGroup(Element* elem, Element* container, KNavMode tabType);
    Element* GetPrevSibling(Element* elem);
    Element* GetLastInTree(Element* container);
    Element* GetPrevInTree(Element* elem, Element* container);
    Element* GetLastTabInGroup(Element* container);
    Element* GetPrevTabWithSameIndex(Element* elem, Element* container);
    Element* GetPrevTabWithPrevIndex(Element* elem, Element* container, KNavMode tabType);

    /// <summary>
    ///  找到Tab需要移到的下一个兄弟元素
    /// </summary>
    /// <param name="elem">当前控件元素</param>
    /// <returns>下一个兄弟控件元素</returns> 
    Element* GetNextSibling(Element* elem);
    Element* GetNextInTree(Element* elem, Element* container);

    Element* GetFirstTabInGroup(Element* container);
    Element* GetNextTabInGroup(Element* elem, Element* container, KNavMode tabType);
    Element* GetNextTab(Element* elem, Element* container, bool onlyDown);

    Element* GetNextTabWithSameIndex(Element* elem, Element* container);
    Element* GetNextTabWithNextIndex(Element* elem, Element* container, KNavMode tabType);

    Element* GetNextInDirection(Element* elem, FocusNavDirection fDir);
};

FocusVisualAdorner::FocusVisualAdorner()
    : Adorner(NULL)
    , _child(NULL)
{

}

FocusVisualAdorner::FocusVisualAdorner(Element* elem, Style* style)
    : Adorner(elem)
{
    Control* control = new Control();

    setAutoDelete(false);

    _child = control;
    _child->ref();

    control->SetStyle(style);
    _child->SetIsHitTestVisible(false);
    _child->Enable(false);

    AddVisualChild(_child);

    setAutoDelete(true);
}

FocusVisualAdorner::~FocusVisualAdorner()
{
    if (_child)
    {
        _child->unref();
    }
}

int FocusVisualAdorner::GetVisualChildrenCount()
{
    return (_child != NULL ? 1 : 0);
}

Element* FocusVisualAdorner::GetVisualChild(int index)
{
    return _child;
}

Size FocusVisualAdorner::OnMeasure(const Size& constraint)
{
    Size renderSize (GetAdornedElement()->ComputeRenderSize());
    //GetVisualChild(0)->Measure(renderSize);
    MeasureChild(GetVisualChild(0), renderSize);
    return renderSize;
}

void FocusVisualAdorner::OnArrange(const Size& arrangeSize)
{
    Adorner::OnArrange(arrangeSize);
    GetVisualChild(0)->Arrange(Rect(Point(), arrangeSize));
}

KeyboardNavigation::KeyboardNavigation()
{
    _navigationProperty = NULL;
    _focusVisualAdornerCache = NULL;
}

void KeyboardNavigation::StaticInit()
{
    if (NULL == IsTabStopProperty)
    {
        IsTabStopProperty = DpProperty::Register(_T("IsTabStop"), RTTIType(), Boolean::RTTIType(), DpPropMemory::GetPropMeta(Boolean::True));
        TabIndexProperty = DpProperty::Register(_T("TabIndex"), RTTIType(), Integer::RTTIType(), DpPropMemory::GetPropMeta(new Integer(0x7fff)));
        AcceptsReturnProperty = DpProperty::RegisterAttached(_T("AcceptsReturn"), RTTIType(), Boolean::RTTIType(), DpPropMemory::GetPropMeta(Boolean::False));

        TabOnceActiveElementProperty = DpProperty::Register(_T("TabOnceActiveElement"), RTTIType(), Object::RTTIType(), DpPropMemory::GetPropMeta(NULL));
        TabNavigationProperty = DpProperty::RegisterAttached(_T("TabNavigation"), RTTIType(), Integer::RTTIType(), DpPropMemory::GetPropMeta(Integer::GetPosInt(KNavMode::knContinue)));
        ControlTabNavigationProperty = DpProperty::RegisterAttached(_T("ControlTabNavigation"), RTTIType(), Integer::RTTIType(), DpPropMemory::GetPropMeta(Integer::GetPosInt(KNavMode::knContinue)));
        ControlTabOnceActiveElementProperty = DpProperty::Register(_T("ControlTabOnceActiveElement"), RTTIType(), Object::RTTIType(), DpPropMemory::GetPropMeta(NULL));
        DirectionalNavigationProperty = DpProperty::RegisterAttached(_T("DirectionalNavigation"), RTTIType(), Integer::RTTIType(), DpPropMemory::GetPropMeta(Integer::GetPosInt(KNavMode::knContinue)));

        TabNavigationProperty->SetConvertValueCb(KNavModeConvert::Convert);
        ControlTabNavigationProperty->SetConvertValueCb(KNavModeConvert::Convert);
        DirectionalNavigationProperty->SetConvertValueCb(KNavModeConvert::Convert);

        BOOL keyboardCues = FALSE;
        //_navigationProperty = TabNavigationProperty;
        SystemParametersInfo(0x100a, 0, &keyboardCues, 0);

        //_alwaysShowFocusVisual = (keyboardCues ? true : false);
    }
}

void KeyboardNavigation::ShowFocusVisual()
{
    Current()->ShowFocusVisual(Keyboard::GetFocusedElement());
}

KeyboardNavigation* KeyboardNavigation::Current()
{
    if (NULL == _keyNav)
    {
        _keyNav = new KeyboardNavigationImpl();
    }

    return _keyNav;
}

void KeyboardNavigation::HideFocusVisual()
{
    if (_focusVisualAdornerCache != NULL)
    {
        AdornerLayer* parent = RTTICast<AdornerLayer>(_focusVisualAdornerCache->GetUIParent());
        
        if (parent != NULL)
        {
            parent->Remove(_focusVisualAdornerCache);
            parent->UpdateLayout();
        }

        Debug::Trace(_U("HideFocusVisual->focusVisualAdornerCache:%ld\n"), (DWORD)(DWORD_PTR)_focusVisualAdornerCache);

        _focusVisualAdornerCache = NULL;
    }
}

FrameworkElement* KeyboardNavigation::PredictFocusedElement(Element* elem, FocusNavDirection dir)
{
    if (elem == NULL)
    {
        return NULL;
    }
    
    _navigationProperty = DirectionalNavigationProperty;
    return NULL;

}

void KeyboardNavigation::ShowFocusVisual(Element* elem)
{
    HideFocusVisual();

    if (GetAlwaysShowFocusVisual() || IsKeyboardMostRecentInputDevice())
    {
        FrameworkElement* fe = CASTTOFE(elem);
        AdornerLayer* adornerLayer = AdornerLayer::GetAdornerLayer(elem);

        if (adornerLayer != NULL && NULL != fe)
        {
            Style* focusStyle = fe->GetFocusVisualStyle();

            if (focusStyle == FrameworkElement::DefaultFocusVisualStyle)
            {
                focusStyle = RTTICast<Style>(fe->FindRes(_U("FocusVisualAdorner")));
            }

            if (focusStyle != NULL)
            {
                adornerLayer->Remove(_focusVisualAdornerCache);
                _focusVisualAdornerCache = new FocusVisualAdorner(elem, focusStyle);
                adornerLayer->Add(_focusVisualAdornerCache);

                elem->InvalidateArrange();

                adornerLayer->UpdateLayout();
            }
        }
    }
}

void KeyboardNavigation::NotifyFocusEnterMainFocusScope(Object* sender, EventArg* e)
{

}

void KeyboardNavigation::NotifyFocusChanged(Object* sender, KeyboardFocusEventArg* e)
{

}

void KeyboardNavigation::UpdateActiveElement(Element* elem)
{
    UpdateActiveElement(elem, TabNavigationProperty);
    UpdateActiveElement(elem, ControlTabNavigationProperty);
}

void KeyboardNavigation::UpdateActiveElement(Element* activeElement, DpProperty* dp)
{
}

void KeyboardNavigation::UpdateActiveElement(Element* container, Element* activeElement, DpProperty* dp)
{
}

void KeyboardNavigationImpl::SetActiveElement(Element* d, Element* value)
{
    if (_navigationProperty == TabNavigationProperty)
    {
        SetTabOnceActiveElement(d, value);
    }
    else
    {
        SetControlTabOnceActiveElement(d, value);
    }
}

void KeyboardNavigationImpl::UpdateActiveElement(Element* activeElement, DpProperty* dp)
{
    _navigationProperty = dp;
    Element* groupParent = GetGroupParent(activeElement, false);
    UpdateActiveElement(groupParent, activeElement, dp);
}

void KeyboardNavigationImpl::UpdateActiveElement(Element* container, Element* activeElement, DpProperty* dp)
{
    _navigationProperty = dp;
    if ((activeElement != container) && (GetKNavMode(container) == KNavMode::knOnce))
    {
        SetActiveElement(container, activeElement);
    }
}

void KeyboardNavigation::UpdateFocusedElement(Element* elem)
{
    Element* focusScope = RTTICast<Element>(FocusManager::GetFocusScope(elem));

    if ((focusScope != NULL) && (focusScope != elem))
    {
        FocusManager::SetFocusedElement(focusScope, elem);
        Element* visualRoot = VisualTreeOp::GetVisualRoot(elem);

        if ((visualRoot != NULL) && (focusScope == visualRoot))
        {
            Current()->NotifyFocusEnterMainFocusScope(visualRoot, &EventArg::Empty);
        }
    }
}

void KeyboardNavigation::SetTabOnceActiveElement(Element* elem, DpObject* val)
{
    elem->SetValue(ControlTabOnceActiveElementProperty, new WeakRefObj(val));
}

void KeyboardNavigation::SetControlTabOnceActiveElement(Element* elem, DpObject* val)
{
    elem->SetValue(ControlTabOnceActiveElementProperty, new WeakRefObj(val));
}

void KeyboardNavigation::SetDirectionalNavigation(Element* elem, KNavMode mode)
{
    if (elem == NULL)
    {
        throw ArgumentNullException("element", __FILELINE__);
    }
    elem->SetValue(DirectionalNavigationProperty, Integer::GetPosInt(mode));
}

bool KeyboardNavigation::ProcessInput(Element* rootElement, MessageParam* e)
{
    return false;
}

bool KeyboardNavigation::Navigate(Element* elem, FocusNavDirection req, ModifierKey modKeys, Element* firstElem)
{
    return false;
}

//---------------------------------------------------------------------------
// KeyboardNavigationImpl

bool KeyboardNavigationImpl::IsFocusScope(Element* elem)
{
    if (!FocusManager::GetIsFocusScope(elem))
    {
        return (GetValidParent(elem) == NULL);
    }
    else
    {
        return true;
    }
}

Element* KeyboardNavigationImpl::GetFocusedElement(Element* elem)
{
    FrameworkElement* fe = CASTTOFE(elem);

    if ((fe != NULL) && !fe->IsKeyboardFocusWithin())
    {
        Element* focusedElem = FocusManager::GetFocusedElement(elem, false);

        if ((focusedElem != NULL) && ((_navigationProperty == ControlTabNavigationProperty) || !IsFocusScope(elem)))
        {
            return focusedElem;
        }
    }
    return NULL;
}

Element* KeyboardNavigationImpl::GetNextInDirection(Element* elem, FocusNavDirection fDir)
{
    Element* navElem = elem;

    if (fDir == FocusNavDirection::fndLeft)
    {
        do
        {
            navElem = GetPrevSibling(navElem);
        }
        while (NULL != navElem && !IsTabStop(navElem));
    }
    else if (fDir == FocusNavDirection::fndRight)
    {
        do
        {
            navElem = GetNextSibling(navElem);
        }
        while (NULL != navElem && !IsTabStop(navElem));
    }

    if (NULL == navElem)
    {
        navElem = elem;
    }

    return navElem;
}

Element* KeyboardNavigationImpl::GetLastInTree(Element* container)
{
    Element* elem = NULL;

    do
    {
        elem = container;
        container = GetLastChild(container);
    }
    while ((container != NULL) && !IsGroup(container));

    if (container != NULL)
    {
        return container;
    }
    else
    {
        return elem;
    }
}

Element* KeyboardNavigationImpl::GetPrevSibling(Element* elem)
{
    Element* parent = elem->GetUIParent();

    if (NULL == parent)
    {
        return FindNextHwnd(elem, false);
    }

    Element* prevElem = NULL;

    for (int i = 0; i < parent->GetVisualChildrenCount(); ++i)
    {
        Element* currElem = parent->GetVisualChild(i);

        if (currElem == elem)
        {
            return prevElem;
        }

        if (currElem && IsElementVisible(currElem))
        {
            prevElem = currElem;
        }
    }

    return prevElem;
}

Element* KeyboardNavigationImpl::GetPrevInTree(Element* elem, Element* container)
{
    if (elem == container)
    {
        return NULL;
    }

    Element* previousSibling = GetPrevSibling(elem);
    if (previousSibling == NULL)
    {
        previousSibling = GetValidParent(elem);
        if (previousSibling && previousSibling->GetUIParent() == NULL)
        {
            previousSibling = GetPrevSibling(previousSibling);
        }
        return previousSibling;
    }

    if (IsGroup(previousSibling))
    {
        return previousSibling;
    }

    return GetLastInTree(previousSibling);
}

Element* KeyboardNavigationImpl::GetLastTabInGroup(Element* container)
{
    int num = 0x7fff;
    Element* lastObj = NULL;
    Element* obj = GetLastInTree(container);

    while ((obj != NULL) && (obj != container))
    {
        if (IsTabStopOrGroup(obj))
        {
            int iTabIndex = GetTabIndex(obj);
            if ((iTabIndex > num) || (lastObj == NULL))
            {
                num = iTabIndex;
                lastObj = obj;
            }
        }

        obj = GetPrevInTree(obj, container);
    }

    return lastObj;
}

Element* KeyboardNavigationImpl::GetPrevTabWithSameIndex(Element* elem, Element* container)
{
    int index = GetTabIndex(elem);
    Element* obj = elem;

    while ((obj = GetPrevInTree(obj, container)) != NULL)
    {
        if (IsTabStopOrGroup(obj) && (GetTabIndex(obj) == index) && obj != container)
        {
            return obj;
        }
    }

    return NULL;
}

Element* KeyboardNavigationImpl::GetPrevTabWithPrevIndex(Element* elem, Element* container, KNavMode tabType)
{
    Element* prevObj = NULL;
    Element* obj = NULL;
    int num = -20000000;
    int num2 = -20000000;
    int iTabIndex = GetTabIndex(elem);
    Element* searchElem = GetLastInTree(container);

    while (searchElem != NULL)
    {
        if (IsTabStopOrGroup(searchElem) && searchElem != container)
        {
            int searchIndex = GetTabIndex(searchElem);

            if (searchIndex > iTabIndex && (searchIndex < num2 || prevObj == NULL))
            {
                num2 = searchIndex;
                prevObj = searchElem;
            }

            if (searchIndex < num || (obj == NULL))
            {
                num = searchIndex;
                obj = searchElem;
            }
        }

        searchElem = GetPrevInTree(searchElem, container);
    }

    if ((tabType == KNavMode::knCycle) && (prevObj == NULL))
    {
        prevObj = obj;
    }

    return prevObj;
}

Element* KeyboardNavigationImpl::GetPrevTabInGroup(Element* elem, Element* container, KNavMode tabType)
{
    if (tabType == KNavMode::knNone)
    {
        return NULL;
    }

    if (elem == NULL)
    {
        return GetLastTabInGroup(container);
    }

    if (tabType == KNavMode::knOnce)
    {
        return NULL;
    }

    if (elem == container)
    {
        return NULL;
    }

    Element* prevTabWithSameIndex = GetPrevTabWithSameIndex(elem, container);

    if (prevTabWithSameIndex != NULL)
    {
        return prevTabWithSameIndex;
    }
    else
    {
        return GetPrevTabWithPrevIndex(elem, container, tabType);
    }
}

Element* KeyboardNavigationImpl::GetPrevTab(Element* elem, Element* container, bool goDownOnly)
{
    if (container == NULL)
    {
        container = GetGroupParent(elem, false);
    }

    KNavMode kNavMode = GetKNavMode(container);

    if (elem == NULL)
    {
        Element* activeElement = GetActiveElement(container);
        if (activeElement != NULL)
        {
            return GetPrevTab(NULL, activeElement, true);
        }

        if (kNavMode == KNavMode::knOnce)
        {
            Element* nextTab = GetNextTabInGroup(NULL, container, kNavMode);
            if (nextTab != NULL)
            {
                return GetPrevTab(NULL, nextTab, true);
            }

            if (IsTabStop(container))
            {
                return container;
            }

            if (goDownOnly)
            {
                return NULL;
            }

            return GetPrevTab(container, NULL, false);
        }
    }
    else
    {
        switch (kNavMode)
        {
        case KNavMode::knOnce:
        case KNavMode::knNone:
            if (goDownOnly || (container == elem))
            {
                return NULL;
            }
            if (IsTabStop(container))
            {
                return container;
            }
            else
            {
                return GetPrevTab(container, NULL, false);
            }
        }
    }

    Element* prevTemp = NULL;
    Element* prevTab = elem;

    while ((prevTab = GetPrevTabInGroup(prevTab, container, kNavMode)) != NULL)
    {
        if ((prevTab == container) && (kNavMode == KNavMode::knLocal))
        {
            break;
        }

        if (IsTabStop(prevTab) && !IsGroup(prevTab))
        {
            return prevTab;
        }

        if (prevTemp == prevTab)
        {
            break;
        }

        if (prevTemp == NULL)
        {
            prevTemp = prevTab;
        }
        
        Element* prevTab2 = GetPrevTab(NULL, prevTab, true);

        if (prevTab2 != NULL)
        {
            return prevTab2;
        }
    }

    if (kNavMode != KNavMode::knContained)
    {
        if ((elem != container) && IsTabStop(container))
        {
            return container;
        }

        if (!goDownOnly && (GetValidParent(container) != NULL))
        {
            return GetPrevTab(container, NULL, false);
        }
    }

    return NULL;
}

Element* KeyboardNavigationImpl::FindNextHwnd(Element* elem, bool bNext)
{
    VisualHost* pHost = NULL;
    HWND hwnd = HANDLETOHWND(HwndHelper::GetHostHwnd(elem));
    HWND hNext = ::GetNextWindow(hwnd, bNext ? GW_HWNDNEXT : GW_HWNDPREV);

    if (::IsWindow(hNext) && ::IsWindowVisible(hNext))
    {
        WORD dwStyle = ::GetWindowLongPtr(hwnd, GWL_STYLE);
        if ((dwStyle & WS_CHILD) != 0)
        {
            pHost = HwndSubclass::FindRenderTarget(hNext);
            if (NULL != pHost)
            {
                return pHost->GetRootElement();
            }
        }
    }
    return NULL;
}

Element* KeyboardNavigationImpl::GetNextSibling(Element* elem)
{
    Element* parent = elem->GetUIParent();

    if (NULL != parent)
    {
        for (int i = 0; i < parent->GetVisualChildrenCount(); ++i)
        {
            Element* currElem = parent->GetVisualChild(i);

            if (currElem == elem)
            {
                ++i;
                while (i < parent->GetVisualChildrenCount())
                {
                    currElem = parent->GetVisualChild(i);

                    if (currElem && IsElementVisible(currElem))
                    {
                        return currElem;
                    }
                    ++i;
                }

                break;
            }
        }
    }
    else
    {
        return FindNextHwnd(elem, true);
    }

    return NULL;
}

KNavMode KeyboardNavigationImpl::GetKNavMode(Element* elem)
{
    return (KNavMode)elem->GetValue(_navigationProperty)->ToInt();
}

Element* KeyboardNavigationImpl::GetActiveElement(Element* d)
{
    if (_navigationProperty != ControlTabNavigationProperty)
    {
        return GetTabOnceActiveElement(d);
    }
    else
    {
        return GetControlTabOnceActiveElement(d);
    }
}

Element* KeyboardNavigationImpl::GetTabOnceActiveElement(Element* d)
{
    WeakRefObj* refObj = RTTICast<WeakRefObj>(d->GetValue(TabOnceActiveElementProperty));
    if ((refObj != NULL) && refObj->IsAlive())
    {
        Element* target = refObj->GetElem<Element>();
        if (VisualTreeOp::GetVisualRoot(target) == VisualTreeOp::GetVisualRoot(d))
        {
            return target;
        }
        d->SetValue(TabOnceActiveElementProperty, NULL);
    }
    return NULL;
}

Element* KeyboardNavigationImpl::GetControlTabOnceActiveElement(Element* d)
{
    WeakRefObj* refObj = RTTICast<WeakRefObj>(d->GetValue(ControlTabOnceActiveElementProperty));
    if ((refObj != NULL) && refObj->IsAlive())
    {
        Element* target = refObj->GetElem<Element>();
        if (VisualTreeOp::GetVisualRoot(target) == VisualTreeOp::GetVisualRoot(d))
        {
            return target;
        }
        d->SetValue(ControlTabOnceActiveElementProperty, NULL);
    }
    return NULL;
}

bool KeyboardNavigationImpl::IsGroup(Element* elem)
{
    return (GetKNavMode(elem) != KNavMode::knContinue);
}

bool KeyboardNavigation::IsTabStop(Element* elem)
{
    Control* fe = CASTTOC(elem);

    if (fe != NULL && fe->IsVisible())
    {
        return (((fe->IsFocusable() && (fe->GetValue(IsTabStopProperty)->ToBool())) && fe->IsEnabled()) && fe->IsVisible());
    }
    else
    {
        return false;
    }
}

bool KeyboardNavigationImpl::IsTabStopOrGroup(Element* elem)
{
    if (!IsTabStop(elem))
    {
        return IsGroup(elem);
    }
    else
    {
        return true;
    }
}

int KeyboardNavigation::GetTabIndex(Element* elem)
{
    int iTabIndex = elem->GetValue(TabIndexProperty)->ToInt();
    return iTabIndex == 0 ? 0x7fff : iTabIndex;
}

bool KeyboardNavigation::IsAcceptsReturn(Element* elem)
{
    return elem->GetValue(AcceptsReturnProperty)->ToBool();
}

Element* KeyboardNavigationImpl::GetGroupParent(Element* elem, bool includeSelf)
{
    Element* grpElem = elem;

    if (!includeSelf)
    {
        elem = GetValidParent(elem);

        if (elem == NULL)
        {
            return grpElem;
        }
    }

    while (elem != NULL)
    {
        if (IsGroup(elem))
        {
            return elem;
        }

        grpElem = elem;
        elem = GetValidParent(elem);
    }

    return grpElem;
}

Element* KeyboardNavigationImpl::GetLastChild(Element* elem)
{
    Element* lastChild = GetFocusedElement(elem);

    // 先获取当前元素保存的焦点元素
    if (lastChild != NULL)
    {
        return lastChild;
    }
    else
    {
        int iCount = elem->GetVisualChildrenCount();

        for (int i = iCount - 1; i >= 0; --i)
        {
            Element* currElem = elem->GetVisualChild(i);

            // tbl
            if (currElem->IsVisible())
            {
                // 找到元素
                if (currElem && IsElementVisible(currElem))
                {
                    return currElem;
                }

                // 递归查找
                lastChild = GetLastChild(currElem);

                if (lastChild != NULL)
                {
                    return lastChild;
                }
            }
        }
    }

    return NULL;
}

bool KeyboardNavigationImpl::IsElementVisible(Element* elem)
{
    if (!elem->IsVisible())
    {
        return false;
    }
    else
    {
        return true;
    }
}

Element* KeyboardNavigationImpl::GetFirstChild(Element* elem)
{
    Element* firstChild = GetFocusedElement(elem);

    // 先获取当前元素保存的焦点元素
    if (firstChild != NULL)
    {
        return firstChild;
    }
    else
    {
        for (int i = 0; i < elem->GetVisualChildrenCount(); ++i)
        {
            Element* currElem = elem->GetVisualChild(i);

            // tbl
            if (IsElementVisible(currElem))
            {
                // 找到元素
                if (currElem && IsElementVisible(currElem))
                {
                    return currElem;
                }

                // 递归查找
                firstChild = GetFirstChild(currElem);

                if (firstChild != NULL)
                {
                    return firstChild;
                }
            }
        }
    }

    return NULL;
}

Element* KeyboardNavigationImpl::GetValidParent(Element* elem)
{
    Element* parent = elem;

    while (NULL != parent && (parent = parent->GetUIParent()) != NULL)
    {
        if (NULL != parent && parent->IsVisible())
        {
            return parent;
        }
    }

    return NULL;
}

Element* KeyboardNavigationImpl::GetNextInTree(Element* elem, Element* container)
{
    Element* firstChild = NULL;

    if ((elem == container) || !IsGroup(elem))
    {
        firstChild = GetFirstChild(elem);
    }

    if ((firstChild != NULL) || (elem == container))
    {
        return firstChild;
    }

    Element* parent = elem;

    do
    {
        Element* nextSibling = GetNextSibling(parent);

        if (nextSibling != NULL)
        {
            return nextSibling;
        }

        parent = GetValidParent(parent);
    }
    while ((parent != NULL) && (parent != container));

    if (parent != NULL)
    {
        return GetNextSibling(parent);
    }
    return NULL;
}

Element* KeyboardNavigationImpl::GetFirstTabInGroup(Element* container)
{
    Element* tabObj = NULL;
    int num = -200000000;
    Element* elem = container;

    while ((elem = GetNextInTree(elem, container)) != NULL)
    {
        if (IsTabStopOrGroup(elem))
        {
            int iTabIndex = GetTabIndex(elem);

            if ((iTabIndex < num) || (tabObj == NULL))
            {
                num = iTabIndex;
                tabObj = elem;
            }
        }
    }

    return tabObj;
}

Element* KeyboardNavigationImpl::GetNextTabWithSameIndex(Element* elem, Element* container)
{
    int index = GetTabIndex(elem);
    Element* obj = elem;

    while ((obj = GetNextInTree(obj, container)) != NULL)
    {
        if (IsTabStopOrGroup(obj) && (GetTabIndex(obj) == index))
        {
            return obj;
        }
    }

    return NULL;
}

Element* KeyboardNavigationImpl::GetNextTabWithNextIndex(Element* elem, Element* container, KNavMode tabType)
{
    Element* nextObj = NULL;
    Element* obj = NULL;
    Element* searchElem = container;

    int num = -20000000;
    int num2 = -20000000;
    int iTabIndex = GetTabIndex(elem);

    while ((searchElem = GetNextInTree(searchElem, container)) != NULL)
    {
        if (IsTabStopOrGroup(searchElem))
        {
            // 
            // 获取元素的TAB索引
            // 
            int searchIndex = GetTabIndex(searchElem);

            if (searchIndex > iTabIndex && (searchIndex < num2 || nextObj == NULL))
            {
                num2 = searchIndex;
                nextObj = searchElem;
            }

            if (searchIndex < num || (obj == NULL))
            {
                num = searchIndex;
                obj = searchElem;
            }
        }
    }

    if ((tabType == KNavMode::knCycle) && (nextObj == NULL))
    {
        nextObj = obj;
    }

    return nextObj;
}

Element* KeyboardNavigationImpl::GetNextTabInGroup(Element* elem, Element* container, KNavMode tabType)
{
    if (tabType == KNavMode::knNone)
    {
        return NULL;
    }

    if ((elem == NULL) || (elem == container))
    {
        return GetFirstTabInGroup(container);
    }

    if (tabType == KNavMode::knOnce)
    {
        return NULL;
    }

    Element* nextTabWithSameIndex = GetNextTabWithSameIndex(elem, container);

    if (nextTabWithSameIndex != NULL)
    {
        return nextTabWithSameIndex;
    }
    else
    {
        return GetNextTabWithNextIndex(elem, container, tabType);
    }
}

Element* KeyboardNavigationImpl::GetNextTab(Element* elem, Element* container, bool onlyDown)
{
    KNavMode keyNavMode = GetKNavMode(container);

    if (elem == NULL)
    {
        if (IsTabStop(container))
        {
            return container;
        }

        Element* activeElement = GetActiveElement(container);

        if (activeElement != NULL)
        {
            return GetNextTab(NULL, activeElement, true);
        }
    }
    else if ((keyNavMode == KNavMode::knOnce || keyNavMode == KNavMode::knNone) && container != elem)
    {
        if (onlyDown)
        {
            return NULL;
        }

        Element* groupParent = GetGroupParent(container, false);
        return GetNextTab(container, groupParent, onlyDown);
    }

    Element* nextTemp = NULL;
    Element* nextTab = elem;
    KNavMode tabType = keyNavMode;

    while ((nextTab = GetNextTabInGroup(nextTab, container, tabType)) != NULL)
    {
        if (nextTemp == nextTab)
        {
            break;
        }

        if (nextTemp == NULL)
        {
            nextTemp = nextTab;
        }

        Element* nextTab2 = GetNextTab(NULL, nextTab, true);

        if (nextTab2 != NULL)
        {
            return nextTab2;
        }

        if (tabType == KNavMode::knOnce)
        {
            tabType = KNavMode::knContained;
        }
    }

    if ((!onlyDown && (tabType != KNavMode::knContained)) && (GetValidParent(container) != NULL))
    {
        return GetNextTab(container, GetGroupParent(container, false), false);
    }

    return NULL;
}

void KeyboardNavigationImpl::SetNavigationProperty(ModifierKey modKeys)
{
    _navigationProperty = ((modKeys & ModifierKey::mkControl) == ModifierKey::mkControl) ? ControlTabNavigationProperty : TabNavigationProperty;
}

bool KeyboardNavigationImpl::Navigate(Element* elem, FocusNavDirection req, ModifierKey modKeys, Element* firstElem)
{
    Element* nextInDirection = NULL;

    switch (req)
    {
        case FocusNavDirection::fndNext:
            SetNavigationProperty(modKeys);        
            nextInDirection = GetNextTab(elem, GetGroupParent(elem, true), false);
            break;

        case FocusNavDirection::fndPrevious:
            SetNavigationProperty(modKeys);
            nextInDirection = GetPrevTab(elem, NULL, false);
            break;

        case FocusNavDirection::fndFirst:
            SetNavigationProperty(modKeys);
            nextInDirection = GetNextTab(NULL, elem, true);
            break;

        case FocusNavDirection::fndLast:
            SetNavigationProperty(modKeys);
            nextInDirection = GetPrevTab(NULL, elem, true);
            break;

        case FocusNavDirection::fndLeft:
        case FocusNavDirection::fndRight:
        case FocusNavDirection::fndUp:
        case FocusNavDirection::fndDown:
            _navigationProperty = DirectionalNavigationProperty;
            nextInDirection = GetNextInDirection(elem, req);
            break;
    }

    FrameworkElement* fe = CASTTOFE(nextInDirection);

    if (NULL != fe && fe != elem)
    {
        fe->Focus();
        fe->BringIntoView();

        return fe->IsKeyboardFocusWithin();
    }
    else
    {
        return false;
    }
}

bool KeyboardNavigationImpl::ProcessInput(Element* focused, MessageParam* e)
{
    bool bHandled = false;

    if (e->message == WM_KEYDOWN && focused != NULL)
    {
        int c = (int)e->wp;
        ModifierKey modKey = (ModifierKey)Keyboard::GetModifiers();

        switch (c)
        {
        case VK_TAB:
            if ((modKey & ModifierKey::mkShift) == ModifierKey::mkShift)
            {
                bHandled = Navigate(focused, FocusNavDirection::fndPrevious, modKey, NULL);
            }
            else
            {
                bHandled = Navigate(focused, FocusNavDirection::fndNext, modKey, NULL);
            }
            break;

        case Key::kLeft:
            bHandled = Navigate(focused, FocusNavDirection::fndLeft, modKey, NULL);
            break;

        case Key::kUp:
            bHandled = Navigate(focused, FocusNavDirection::fndUp, modKey, NULL);
            break;

        case Key::kRight:
            bHandled = Navigate(focused, FocusNavDirection::fndRight, modKey, NULL);
            break;

        case Key::kDown:
            bHandled = Navigate(focused, FocusNavDirection::fndDown, modKey, NULL);
            break;

        case Key::kMenu:
            KeyboardNavigation::ShowFocusVisual();

        case Key::kLeftAlt:
        case Key::kRightAlt:
            KeyboardNavigation::ShowFocusVisual();
            break;

        default:
            ;
        }
    }
    
    return bHandled;
}

}
