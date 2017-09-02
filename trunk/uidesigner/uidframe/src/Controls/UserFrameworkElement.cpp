
#include "stdafx.h"
#include <Controls/UserFrameworkElement.h>

ImplementRTTIOfClass(UserFrameworkElement, suic::FrameworkElement)

//==============================================
// UserFrameworkElement
UserFrameworkElement::UserFrameworkElement()
{
}

UserFrameworkElement::~UserFrameworkElement()
{
}

void UserFrameworkElement::OnRender(Drawing* drawing)
{
    suic::fRect rect(Point(), GetRenderSize());
    suic::TmParam tm;
    suic::FormattedText fmt;
    suic::String strText(_U("UserControl"));

    fmt.SetColor(suic::Colors::White);
    fmt.SetSingleline(true);

    drawing->FillRect(suic::DrawCtx::DefDraw, 0x8800FF00, &rect);

    drawing->MeasureString(tm, &fmt, strText.c_str(), strText.Length());

    rect.left += (rect.Width() - tm.size.cx) / 2;
    rect.top += (rect.Height() - tm.size.cy) / 2;

    drawing->DrawString(&fmt, strText.c_str(), strText.Length(), &rect);
}


