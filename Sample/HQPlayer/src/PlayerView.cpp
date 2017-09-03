/////////////////////////////////////////////////////
// PlayerView.cpp

#include "stdafx.h"
#include "PlayerView.h"

PlayerView::PlayerView()
{
    _bmp = NULL;
}

PlayerView::~PlayerView()
{
   if (NULL != _bmp)
    {
        _bmp->unref();
    }
}

void PlayerView::Dispose()
{
    suic::FrameworkElement::Dispose();
}

void PlayerView::OnRender(suic::Drawing* drawing)
{
    suic::fRect rect(suic::fPoint(), GetRenderSize().TofSize());
    drawing->FillRect(suic::DrawCtx::DefDraw, suic::Colors::Black, rect);
    
    if (NULL != _bmp)
    {
        DrawVideo(drawing, &(_bmp->bmp));
    }
}

void PlayerView::PostRender(BmpInfo* bmp)
{
    if (NULL != _bmp)
    {
        _bmp->unref();
    }

    _bmp = bmp;
    InvalidateVisual();
}

void PlayerView::DrawVideo(suic::Drawing* drawing, suic::Bitmap* bmp)
{
    suic::fRect rect(suic::fPoint(), GetRenderSize().TofSize());
    suic::fRect rcImg(0, 0, _bmp->bmp.Width(), _bmp->bmp.Height());

    suic::Float width = 1.0;
    suic::Float height = 1.0;

    width = rect.Width() / rcImg.Width();
    height = rect.Height() / rcImg.Height();

    suic::Float fRatio = (width < height) ? width : height;

    if (fRatio > 1)
    {
        fRatio = 1.0f;
    }

    suic::Float w = rcImg.Width() * fRatio;
    suic::Float h = rcImg.Height() * fRatio;

    rect.left = (rect.Width() - w) / 2.0f;
    rect.top = (rect.Height() - h) / 2.0f;
    rect.right = rect.left + w;
    rect.bottom = rect.top + h; 

    suic::DrawCtx drawCtx(suic::DrawCtx::eDrawLevel::flNone, false);
    drawing->DrawImage(&drawCtx, &(_bmp->bmp), rect, rcImg);
}
