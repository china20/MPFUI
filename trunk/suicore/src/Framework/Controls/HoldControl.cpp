
#include <Framework/Controls/HoldControl.h>

namespace suic
{

ImplementRTTIOfClass(HoldControl, suic::FrameworkElement)

HoldControl::HoldControl()
{
}

HoldControl::HoldControl(const String& name)
    : _name(name)
{
}

HoldControl::~HoldControl()
{

}

suic::String HoldControl::GetName() const
{
    return _name;
}

void HoldControl::OnRender(Drawing* drawing)
{
    suic::FormattedText fmt;
    suic::fRect rect(Point(), GetRenderSize());

    drawing->FillRect(DrawCtx::DefDraw, suic::Colors::White, &rect);
    drawing->DrawString(&fmt, _name.c_str(), _name.Length(), &rect);
}

}
