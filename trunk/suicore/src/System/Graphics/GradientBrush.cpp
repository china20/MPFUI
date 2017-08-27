
// ============================================================================
// uiGradientBrush

#include <System/Graphics/GradientBrush.h>

namespace suic
{

ImplementRTTIOfClass(GradientBrush, Brush)

GradientBrush::GradientBrush()
    : _stops(NULL)
{
}

GradientBrush::~GradientBrush()
{
    FREEREFOBJ(_stops);
}

static void ReadGradientStop(GradientStops* pInfo, IXamlNode* pNode)
{
    String strColor = pNode->FindAttri(_T("Color"));
    String strOffset = pNode->FindAttri(_T("Offset"));

    if (!strColor.Empty() && !strOffset.Empty())
    {
        pInfo->AddStops(strOffset.ToFloat(), OColor::FromString(strColor));        
    }
}

void GradientBrush::SetGradientStops(GradientStops* stops)
{
    SETREFOBJ(_stops, stops);
}

void GradientBrush::AddGradientStop(Color clr, Float radio)
{
    GetGradientStops()->AddStops(radio, clr);
}

bool GradientBrush::Parse(IXamlNode* pNode)
{
    //
    // ½âÎöGradientStop
    //
    pNode->Reset();

    while (pNode->HasNext())
    {
        String strChildName(pNode->CurrentName());

        if (strChildName.Equals(_T("GradientStop")))
        {
            IXamlNode* pChild = pNode->Current();
            ReadGradientStop(GetGradientStops(), pChild);
        }
    }
    return true;
}

}
