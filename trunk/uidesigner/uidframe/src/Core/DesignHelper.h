
#ifndef _UIDESIGNHELPER_H_
#define _UIDESIGNHELPER_H_

#include <System/Windows/EffectiveValue.h>
#include <System/Windows/FrameworkElement.h>
#include <Framework/Controls/Application.h>

namespace suic
{

class SUICORE_API DesignHelper
{
public:

    static void SetDesignApplication(Application* app);

    static void EnterDesignMode();
    static void ExitDesignMode();
    static bool IsInDesignMode();

    static Application* GetDesignApplication();
    static void SetInDesignMode(DpObject* fe);
    static bool GetInDesignMode(DpObject* fe);

    static void SetElementParent(Element* parent, FrameworkElement* child);
    static void SetTemplateParent(DpObject* parent, FrameworkElement* child);
    static void SetTemplateDesignMode(FrameworkElement* child);

    static void ReadDefaultThemeData(suic::ISStream* data);
};

}

#endif
