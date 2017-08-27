
#include <System/Tools/DesignHelper.h>
#include <System/Windows/MemberVisitor.h>
#include <System/Resources/BindParser.h>
#include <System/Resources/StaticResource.h>
#include <System/Resources/DynamicResource.h>
#include <System/Interop/System.h>

namespace suic
{

static bool g_s_indesignmode = false;
static AppPtr g_s_designapp;

void DesignHelper::SetDesignApplication(Application* app)
{
    g_s_designapp = app;
}

void DesignHelper::EnterDesignMode()
{
    g_s_indesignmode = true;
}

void DesignHelper::ExitDesignMode()
{
    g_s_indesignmode = false;
}

bool DesignHelper::IsInDesignMode()
{
    return g_s_indesignmode;
}


Application* DesignHelper::GetDesignApplication()
{
    return g_s_designapp.get();
}

void DesignHelper::SetInDesignMode(DpObject* fe)
{
    MemberVisitor::SetInDesignMode(fe, true);
}

bool DesignHelper::GetInDesignMode(DpObject* fe)
{
    return MemberVisitor::GetInDesignMode(fe);
}

void DesignHelper::SetElementParent(Element* parent, FrameworkElement* child)
{
    MemberVisitor::SetParent(parent, child);
}

void DesignHelper::SetTemplateParent(DpObject* parent, FrameworkElement* child)
{
    MemberVisitor::SetTemplateParent(parent, child);
}

void DesignHelper::SetTemplateDesignMode(FrameworkElement* child)
{
    child->WriteInnerFlag(IsTemplateGenerating, true);
}

void DesignHelper::ReadDefaultThemeData(suic::ISStream* data)
{
    return suic::CoreSystem::Ins()->ReadDefaultThemeData(data);
}

}
