// ======================================================================
//
// Copyright (c) 2008-2022 汪荣(视点UI), Inc. All rights reserved.
//
// MPF界面库遵循指定开源协议，团体或公司商用请根据协议购买授权，
// 任何个人、团体或公司不能居于此库衍生任何商业性质的库或代码。
//
// ======================================================================
/////////////////////////////////////////////////////////////////////////
// InheritStateProperty.cpp

#include <System/Input/Mouse.h>
#include <System/Input/Keyboard.h>
#include <System/Windows/CoreTool.h>
#include <System/Windows/FrameworkElement.h>
#include <System/Windows/InheritStateProperty.h>

#include <System/Tools/VisualTreeOp.h>
#include <System/Interop/InternalWindowOper.h>

namespace suic
{

InheritStateProperty::InheritStateProperty(DpProperty* dp, int cacheFlag, int changedFlag)
    : _dp(dp)
    , _cacheFlag(cacheFlag)
    , _changedFlag(changedFlag)
{
}

void InheritStateProperty::DoRelative(Element* oldElem, Element* newElem)
{

}

void InheritStateProperty::OnOriginalValueChanged(Element* oldElem, Element* newElem)
{
    // 第一步：先设置鼠标离开的元素状态(IsMouseOverChange)
    Element* pElem = oldElem;

    while (pElem != NULL)
    {
        // 
        // 设置状态改变
        //
        pElem->WriteFlag(_changedFlag, true);
        // 
        // 取消鼠标悬停标志
        //
        pElem->WriteFlag(_cacheFlag, false);

        if (pElem->BlockVisualState())
        {
            break;
        }

        pElem = pElem->GetUIParent();
    }

    // 第二步设置拥有鼠标的元素状态(IsMouseOverChange)
    pElem = newElem;
    while (pElem != NULL)
    {
        // 
        // 设置状态改变
        //
        pElem->WriteFlag(_changedFlag, !pElem->ReadFlag(_changedFlag));

        // 
        // 取消鼠标悬停标志
        //
        pElem->WriteFlag(_cacheFlag, true);

        if (pElem->BlockVisualState())
        {
            break;
        }

        pElem = pElem->GetUIParent();
    }

    //DoRelative(oldElem, newElem);

    Element* pRoot = oldElem ? VisualTreeOp::GetVisualRoot(oldElem) : VisualTreeOp::GetVisualRoot(newElem);

    // 第三步，检测状态IsMouseOverChange发生改变的元素
    // 触发对应的MouseEnter和MouseLeave事件
    pElem = oldElem;
    while (pElem != NULL)
    {
        if (pElem->ReadFlag(_changedFlag))
        {
            pElem->WriteFlag(_changedFlag, false);
            pElem->WriteFlag(_cacheFlag, false);
            // 触发状态改变
            pElem->SetValue(_dp, Boolean::False);
            FireState(pElem, true);
        }

        if (pElem->BlockVisualState())
        {
            break;
        }

        pElem = pElem->GetUIParent();
    }

    // 鼠标进入
    pElem = newElem;
    while (pElem != NULL)
    {
        if (pElem->ReadFlag(_changedFlag))
        {
            pElem->WriteFlag(_changedFlag, false);
            pElem->WriteFlag(_cacheFlag, true);

            if (pElem->IsEnabled())
            {
                // 触发状态改变
                pElem->SetValue(_dp, Boolean::True);
                FireState(pElem, false);
            }
        }

        if (pElem->BlockVisualState())
        {
            break;
        }

        pElem = pElem->GetUIParent();
    }
}

//===========================================================
// MouseOverProperty
MouseOverProperty::MouseOverProperty()
    : InheritStateProperty(Element::IsMouseOverProperty
    , CoreFlags::IsMouseOverWithinCache
    , CoreFlags::IsMouseOverWithinChanged)
{
}

void MouseOverProperty::FireState(Element* sender, bool oldValue)
{
    Point pt = __GetCusorPoint(sender);
    MouseButtonEventArg e(sender, pt);

    if (oldValue)
    {
        e.SetRoutedEvent(sender->MouseLeaveEvent);
    }
    else
    {
        e.OnOverrideOriginalSource(MouseDevice::GetMouseOver());
        e.SetRoutedEvent(sender->MouseEnterEvent);
    }

    sender->RaiseEvent(&e);
}

void MouseOverProperty::HandleRelativeMouse(Element* elem)
{
}

void MouseOverProperty::DoRelative(Element* oldElem, Element* newElem)
{
}

//===========================================================
// MouseCaptureProperty
MouseCaptureProperty::MouseCaptureProperty()
    : InheritStateProperty(Element::IsMouseCaptureWithinProperty
    , CoreFlags::IsMouseCapturedWithinCache
    , CoreFlags::IsMouseCapturedWithinChanged)
{
}

void MouseCaptureProperty::FireState(Element* sender, bool oldValue)
{
    sender->OnMouseCaptureWithinChanged(!oldValue);
}

//===========================================================
// FocusWithinProperty
FocusWithinProperty::FocusWithinProperty()
    : InheritStateProperty(Element::IsKeyboardFocusWithinProperty
    , CoreFlags::IsKeyboardFocusWithinCache
    , CoreFlags::IsKeyboardFocusWithinChanged)
{

}

void FocusWithinProperty::FireState(Element* sender, bool oldValue)
{
    if (sender->IsFocusable())
    {
        sender->OnKeyboardFocusWithinChanged(!oldValue);
    }
}

}
