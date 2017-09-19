
// ============================================================================
// LinearGradientBrush

#include <System/Graphics/LinearGradientBrush.h>
#include <System/Tools/Array.h>
#include <System/Types/Interface.h>
#include <System/Render/Skia/SkiaDrawing.h>
#include <Skia/effects/SkGradientShader.h>

namespace suic
{

void DrawGradient(Drawing* drawing, const Rect* lprc, Color dwFirst, Color dwSecond);

BOOL DrawGraden(HDC hdc, const RECT *pRect, const DWORD *cl, int Num, DWORD dwMode)
{
    int iWid;
    int iHei;
    TRIVERTEX *pvert;
    GRADIENT_RECT *pgRect;

    if (cl == NULL || Num < 1 || pRect == NULL || dwMode == GRADIENT_FILL_TRIANGLE)
    {
        return TRUE;
    }

    if (Num == 1)
    {
        return TRUE;
    }

    pvert = new TRIVERTEX[Num * 2 - 2];
    pgRect = new GRADIENT_RECT[Num];

    iWid = pRect->right - pRect->left;
    iHei = pRect->bottom - pRect->top;

    for (int i = 0; i < Num - 1; i++)
    {
        if (dwMode == GRADIENT_FILL_RECT_V)
        {
            pvert[i * 2].x = pRect->left;
            pvert[i * 2].y = pRect->top + (Float) iHei / (Float) (Num - 1) * i;

            pvert[i * 2 + 1].x = pRect->right;
            pvert[i * 2 + 1].y = pRect->top + (Float) iHei / (Float) (Num - 1) * (i + 1);
        }
        else if (dwMode == GRADIENT_FILL_RECT_H)
        {
            pvert[i * 2].x = pRect->left + (Float) iWid / (Float) (Num - 1) * i;
            pvert[i * 2].y = pRect->top;

            pvert[i * 2 + 1].x = pRect->left + (Float) iWid / (Float) (Num - 1) * (i + 1);
            pvert[i * 2 + 1].y = pRect->bottom;
        }

        pvert[i * 2] .Red    = (WORD)GetRValue((cl[i])) << 8;
        pvert[i * 2] .Green  = (WORD)GetGValue((cl[i])) << 8;
        pvert[i * 2] .Blue   = (WORD)GetBValue((cl[i])) << 8;
        pvert[i * 2] .Alpha  = 0x0000;

        pvert[i * 2 + 1] .Red    = (WORD)GetRValue((cl[i + 1])) << 8;
        pvert[i * 2 + 1] .Green  = (WORD)GetGValue((cl[i + 1])) << 8;
        pvert[i * 2 + 1] .Blue   = (WORD)GetBValue((cl[i + 1])) << 8;
        pvert[i * 2 + 1] .Alpha  = 0x0000;

        pgRect[i].UpperLeft  = i * 2;
        pgRect[i].LowerRight = i * 2 + 1;
    }

    BOOL bRet = ::GradientFill(hdc, pvert, Num * 2, pgRect, Num - 1, dwMode);

    delete []pvert;
    delete []pgRect;

    return bRet;
}

/*
<LinearGradientBrush MappingMode="" StartPoint="0,0s" EndPoint="1,1">
      <GradientStop Color="Yellow" Offset="0.0" />
      <GradientStop Color="Red" Offset="0.25" />
      <GradientStop Color="Blue" Offset="0.75" />
      <GradientStop Color="LimeGreen" Offset="1.0" />
</LinearGradientBrush>

*/
ImplementRTTIOfClass(LinearGradientBrush, GradientBrush)

LinearGradientBrush::LinearGradientBrush()
{
    _endPoint.x = 1;
    _endPoint.y = 1;
    _index = Brush::eBrushIndex::biLinearBrush;
}

LinearGradientBrush::~LinearGradientBrush()
{

}

void LinearGradientBrush::SetStartPoint(fPoint start)
{
    _startPoint = start;    
}

void LinearGradientBrush::SetEndPoint(fPoint end)
{
    _endPoint = end;
}

LinearGradientBrush* LinearGradientBrush::Clone() const
{
    return const_cast<LinearGradientBrush*>(this);
}

bool LinearGradientBrush::Parse(const String& xml)
{
    return false;
}

bool LinearGradientBrush::Parse(IXamlNode* pNode)
{
    int iCount = 0;
    IXamlAttris* pAttri = pNode->GetAttris();

    pAttri->Reset();

    while (pAttri->HasNext())
    {
        String name(pAttri->GetName());
        String val(pAttri->GetValue());

        if (name.Equals(_T("StartPoint")))
        {
            _startPoint = val.TofPoint();            
        }
        else if (name.Equals(_T("EndPoint")))
        {
            _endPoint = val.TofPoint();            
        }        
    }

    _key = pNode->FindAttri(_T("Key"));
    if (_key.Empty())
    {
        _key = pNode->FindAttri(_T("x:Key"));
    }

    //
    // ½âÎöGradientStop
    //
    return GradientBrush::Parse(pNode);
}

//void LinearGradientBrush::Draw(Drawing* drawing, const fRect* lprc)
//{
//    SkiaDrawing::DrawLinearGradientBrush((SkiaDrawing*)drawing, this, lprc);
//
//    //int iCount = _data.colors.Length();
//
//    //if (iCount > 0)
//    //{
//    //    SkPaint skpaint;
//    //    SkPoint pts[2];
//
//    //    pts[0].fX = lprc->left + lprc->Width() * _data.startX;
//    //    pts[0].fY = lprc->top + lprc->Height() * _data.startY;
//
//    //    pts[1].fX = lprc->left + lprc->Width() * _data.endX;
//    //    pts[1].fY = lprc->top + lprc->Height() * _data.endY;
//
//    //    //SkShader* shader = SkGradientShader::CreateLinear(pts, &_data.colors[0], &_data.posinfo[0], iCount, SkShader::TileMode::kMirror_TileMode);
//
//    //    //skpaint.setShader(shader);
//    //    //drawing->DrawRect(paint, lprc);
//
//    //    //shader->unref();
//    //}
//}

//
//void DrawGradient(DrawingContext* drawing, const Rect* lprc, COLOR dwFirst, COLOR dwSecond)
//{
//    typedef BOOL (WINAPI *LPALPHABLEND)(HDC, int, int, int, int,HDC, int, int, int, int, BLENDFUNCTION);
//
//    static LPALPHABLEND lpAlphaBlend = (LPALPHABLEND)::GetProcAddress(::GetModuleHandle(_T("msimg32.dll")), "AlphaBlend");
//
//    if (lpAlphaBlend == NULL) 
//    {
//        lpAlphaBlend = AlphaBitBlt;
//    }
//
//    typedef BOOL (WINAPI *PGradientFill)(HDC, PTRIVERTEX, ULONG, PVOID, ULONG, ULONG);
//    static PGradientFill lpGradientFill = (PGradientFill) ::GetProcAddress(::GetModuleHandle(_T("msimg32.dll")), "GradientFill");
//
//    BYTE bAlpha = (BYTE)(((dwFirst >> 24) + (dwSecond >> 24)) >> 1);
//
//    if (bAlpha == 0) 
//    {
//        return;
//    }
//
//    int cx = lprc->right - lprc->left;
//    int cy = lprc->bottom - lprc->top;
//    RECT rcPaint = rc;
//    HDC hPaintDC = hdc;
//    HBITMAP hPaintBitmap = NULL;
//    HBITMAP hOldPaintBitmap = NULL;
//
//    if (bAlpha < 255) 
//    {
//        rcPaint.left = rcPaint.top = 0;
//        rcPaint.right = cx;
//        rcPaint.bottom = cy;
//        hPaintDC = ::CreateCompatibleDC(hdc);
//        hPaintBitmap = ::CreateCompatibleBitmap(hdc, cx, cy);
//
//        hOldPaintBitmap = (HBITMAP) ::SelectObject(hPaintDC, hPaintBitmap);
//    }
//
//    if (lpGradientFill != NULL) 
//    {
//        TRIVERTEX triv[2] = 
//        {
//            {rcPaint.left, rcPaint.top, GetRValue(dwFirst) << 8, GetGValue(dwFirst) << 8, GetBValue(dwFirst) << 8, 0xFF00},
//            {rcPaint.right, rcPaint.bottom, GetRValue(dwSecond) << 8, GetGValue(dwSecond) << 8, GetBValue(dwSecond) << 8, 0xFF00}
//        };
//
//        GRADIENT_RECT grc = {0, 1};
//        lpGradientFill(hPaintDC, triv, 2, &grc, 1, bVertical ? GRADIENT_FILL_RECT_V : GRADIENT_FILL_RECT_H);
//    }
//    else 
//    {
//        // Determine how many shades
//        int nShift = 1;
//        if (nSteps >= 64) 
//        {
//            nShift = 6;
//        }
//        else if (nSteps >= 32) 
//        {
//            nShift = 5;
//        }
//        else if (nSteps >= 16) 
//        {
//            nShift = 4;
//        }
//        else if (nSteps >= 8) 
//        {
//            nShift = 3;
//        }
//        else if (nSteps >= 4) 
//        {
//            nShift = 2;
//        }
//
//        int nLines = 1 << nShift;
//
//        for (int i = 0; i < nLines; i++) 
//        {
//            // Do a little alpha blending
//            BYTE bR = (BYTE) ((GetBValue(dwSecond) * (nLines - i) + GetBValue(dwFirst) * i) >> nShift);
//            BYTE bG = (BYTE) ((GetGValue(dwSecond) * (nLines - i) + GetGValue(dwFirst) * i) >> nShift);
//            BYTE bB = (BYTE) ((GetRValue(dwSecond) * (nLines - i) + GetRValue(dwFirst) * i) >> nShift);
//            // ... then paint with the resulting color
//            HBRUSH hBrush = ::CreateSolidBrush(RGB(bR,bG,bB));
//            RECT r2 = rcPaint;
//
//            if (bVertical) 
//            {
//                r2.bottom = lprc->bottom - ((i * (lprc->bottom - lprc->top)) >> nShift);
//                r2.top = lprc->bottom - (((i + 1) * (lprc->bottom - lprc->top)) >> nShift);
//
//                if ((r2.bottom - r2.top) > 0) 
//                {
//                    ::FillRect(hdc, &r2, hBrush);
//                }
//            }
//            else 
//            {
//                r2.left = lprc->right - (((i + 1) * (lprc->right - lprc->left)) >> nShift);
//                r2.right = lprc->right - ((i * (lprc->right - lprc->left)) >> nShift);
//
//                if ((r2.right - r2.left) > 0) 
//                {
//                    ::FillRect(hPaintDC, &r2, hBrush);
//                }
//            }
//            ::DeleteObject(hBrush);
//        }
//    }
//
//    if (bAlpha < 255) 
//    {
//        BLENDFUNCTION bf = {AC_SRC_OVER, 0, bAlpha, AC_SRC_ALPHA};
//        lpAlphaBlend(hdc, lprc->left, lprc->top, cx, cy, hPaintDC, 0, 0, cx, cy, bf);
//        ::SelectObject(hPaintDC, hOldPaintBitmap);
//        ::DeleteObject(hPaintBitmap);
//        ::DeleteDC(hPaintDC);
//    }
//}

};

