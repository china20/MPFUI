/////////////////////////////////////////////////////////
// uicorehelper.cpp

#include <System/Tools/Debug.h>
#include <System/Input/Keyboard.h>
#include <System/Windows/CoreTool.h>
#include <Framework/Controls/Application.h>

namespace suic {

static void RecursiveLoadedWithTarget(ObjectPtr& target, Element* sender, ElementPtr& focused)
{
    LoadedEventArg e(sender, target.get());

    for (int i = 0; i < sender->GetVisualChildrenCount(); ++i)
    {
        Element* frame(sender->GetVisualChild(i));
        RecursiveLoadedWithTarget(target, frame, focused);
    }

    if (!sender->ReadFlag(CoreFlags::IsLoaded))
    {
        sender->WriteFlag(CoreFlags::IsLoaded, true);
        sender->OnLoaded(&e);

        Object* obj(sender->GetValue(Element::IsFocusedProperty));
        if (!focused && obj && obj->ToBool())
        {
            focused = sender;
        }

        e.SetRoutedEvent(sender->LoadedEvent);
        sender->RaiseEvent(&e);
    }
}

static void RecursiveUnloadedWithTarget(ObjectPtr& target, Element* sender)
{
    LoadedEventArg e(sender, target.get());

    for (int i = 0; i < sender->GetVisualChildrenCount(); ++i)
    {
        Element* frame(sender->GetVisualChild(i));
        RecursiveUnloadedWithTarget(target, frame);
    }

    sender->OnUnloaded(&e);
    sender->WriteFlag(CoreFlags::IsLoaded, false);
    if (sender->UnloadedEvent)
    {
        e.SetRoutedEvent(sender->UnloadedEvent);
        sender->RaiseEvent(&e);
    }
}

void RecursiveLoadedWithFocused(Element* sender, ElementPtr& focused)
{
    if (!sender->ReadFlag(CoreFlags::IsLoaded))
    {
        ObjectPtr target(VisualHost::GetVisualHost(sender));

        if (target)
        {
            RecursiveLoadedWithTarget(target, sender, focused);
        }
    }
}

void RecursiveLoaded(Element* sender)
{
    ElementPtr focused;
    RecursiveLoadedWithFocused(sender, focused);
}

void RecursiveUnloaded(Element* sender)
{
    ObjectPtr target(VisualHost::GetVisualHost(sender));
    if (target)
    {
        RecursiveUnloadedWithTarget(target, sender);
    }
}

void RunApplication(Object* main, const String& path)
{
    WindowPtr wnd(main);
    Application::RunApp(wnd.get(), path);
    wnd = NULL;
}

}
