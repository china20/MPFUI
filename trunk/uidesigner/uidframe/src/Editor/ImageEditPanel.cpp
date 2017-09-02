/////////////////////////////////////////////////////
// ImageEditPanel.cpp

#include "stdafx.h"

#include <Main/DesignPanel.h>
#include <Editor/ImageEditPanel.h>
#include <Editor/ImageEditWindow.h>

ImplementRTTIOfClass(DImageGrid, suic::Grid)
ImplementRTTIOfClass(ImageEditPanel, suic::Control)

DImageGrid::DImageGrid()
{
    SetClipToBounds(true);
}

DImageGrid::~DImageGrid()
{
}

void DImageGrid::OnRender(Drawing* drawing)
{
    const int RECSIZE = 8;
    fRect rect(0, 0, GetRenderSize().Width(), GetRenderSize().Height());
    int iBeg = RECSIZE;
    Color clrs[2] = {0xFFFFFFFF, 0xFFCCCCCC};

    drawing->EraseRect(suic::DrawCtx::DefDraw, clrs[0], &rect);

    for (int y = 0; y < (int)rect.bottom;)
    {
        for (int x = iBeg; x < (int)rect.right;)
        {
            fRect rcDraw(x, y, RECSIZE, RECSIZE);
            drawing->EraseRect(suic::DrawCtx::DefDraw, clrs[1], &rcDraw);
            x += 16;
        }
        y += RECSIZE;
        iBeg = iBeg == 0 ? RECSIZE : 0;
    }
}

static ImageEditWindow* GetImageEditWindow(ImageEditPanel* imgPanel)
{
    FrameworkElement* parent = imgPanel->GetParent();
    while (parent != NULL)
    {
        ImageEditWindow* hostWnd = DynamicCast<ImageEditWindow>(parent);
        if (NULL != hostWnd)
        {
            return hostWnd;
        }
        parent = parent->GetParent();
    }
    return NULL;
}

ImageEditPanel::ImageEditPanel()
{
    _editBmp = NULL;

    _startValue = 0;
    _dragTracking = false;
    _dragMode = ImageDragMode::idmNone;

    int iRadius = 5;
    int iSize = 10;

    SetClipToBounds(true);

    _horiGap = CreatePathGeometry(Point(0, iRadius), Point(iSize, 0), Point(iSize, iSize));
    _vertGap = CreatePathGeometry(Point(iRadius, 0), Point(0, iSize), Point(iSize, iSize));
}

ImageEditPanel::~ImageEditPanel()
{
    _horiGap->unref();
    _vertGap->unref();
}

PathGeometry* ImageEditPanel::CreatePathGeometry(Point p0, Point p1, Point p2)
{
    PathGeometry* path = new PathGeometry();
    path->ref();

    path->MoveTo(p0.x, p0.y);
    path->LineTo(p1.x, p1.y);
    path->LineTo(p2.x, p2.y);
    path->Close();

    return path;
}

void ImageEditPanel::OnInitialized(EventArg* e)
{

}

void ImageEditPanel::OnSetCursor(CursorEventArg* e)
{
    OCursor* curs = NULL;
    suic::Point pt = PointFromScreen(e->GetMousePoint());

    _dragMode = ImageDragMode::idmNone;

    if (_drawMetas[0].PointIn(pt))
    {
        curs = suic::OCursor::SizeWe;
        _dragMode = ImageDragMode::idmImgLeft;
    }
    else if (_drawMetas[1].PointIn(pt))
    {
        curs = suic::OCursor::SizeNs;
        _dragMode = ImageDragMode::idmImgTop;
    }
    else if (_drawMetas[2].PointIn(pt))
    {
        curs = suic::OCursor::SizeWe;
        _dragMode = ImageDragMode::idmImgRight;
    }
    else if (_drawMetas[3].PointIn(pt))
    {
        curs = suic::OCursor::SizeNs;
        _dragMode = ImageDragMode::idmImgBottom;
    }
    else
    {
        fRect drawRc = GetDrawViewbox();
        fPoint point = GetHoriUp(drawRc);
        if (suic::Rect(point.x, point.y, 10, 10).PointIn(pt))
        {
            curs = suic::OCursor::Hand;
            _dragMode = ImageDragMode::idmGapTop;
        }
        else
        {
            point = GetHoriDown(drawRc);
            if (suic::Rect(point.x, point.y, 10, 10).PointIn(pt))
            {
                curs = suic::OCursor::Hand;
                _dragMode = ImageDragMode::idmGapBottom;
            }
            else
            {
                point = GetVertLeft(drawRc);
                if (suic::Rect(point.x, point.y, 10, 10).PointIn(pt))
                {
                    curs = suic::OCursor::Hand;
                    _dragMode = ImageDragMode::idmGapLeft;
                }
                else
                {
                    point = GetVertRight(drawRc);
                    if (suic::Rect(point.x, point.y, 10, 10).PointIn(pt))
                    {
                        curs = suic::OCursor::Hand;
                        _dragMode = ImageDragMode::idmGapRight;
                    }
                }
            }
        }
    }

    if (NULL != curs)
    {
        e->SetCursor(curs);
        e->SetHandled(true);
    }
}

void ImageEditPanel::CoercedViewbox(int iMode)
{
    int imgWid = _editBmp->GetImageSource()->Width();
    int imgHei = _editBmp->GetImageSource()->Height();

    switch (iMode)
    {
    case ImageDragMode::idmImgLeft:
        if ((int)_viewBox.left < 0)
        {
            _viewBox.left = 0;
        }
        else if ((int)_viewBox.left > (int)_viewBox.right)
        {
            _viewBox.left = _viewBox.right;
        }

        if ((int)_viewBox.Width() < _cornerRect.left + _cornerRect.right)
        {
            _viewBox.left = _viewBox.right - (_cornerRect.left + _cornerRect.right);
        }
        break;

    case ImageDragMode::idmImgTop:
        if ((int)_viewBox.top < 0)
        {
            _viewBox.top = 0;
        }
        else if ((int)_viewBox.top > (int)_viewBox.bottom)
        {
            _viewBox.top = _viewBox.bottom;
        }

        if ((int)_viewBox.Height() < _cornerRect.top + _cornerRect.bottom)
        {
            _viewBox.top = _viewBox.bottom - (_cornerRect.top + _cornerRect.bottom);
        }
        break;

    case ImageDragMode::idmImgRight:
        if ((int)_viewBox.right > imgWid)
        {
            _viewBox.right = imgWid;
        }
        else if ((int)_viewBox.right < (int)_viewBox.left)
        {
            _viewBox.right = _viewBox.left;
        }
        if ((int)_viewBox.Width() < _cornerRect.left + _cornerRect.right)
        {
            _viewBox.right = _viewBox.left + (_cornerRect.left + _cornerRect.right);
        }
        break;

    case ImageDragMode::idmImgBottom:
        if ((int)_viewBox.bottom > imgHei)
        {
            _viewBox.bottom = imgHei;
        }
        else if ((int)_viewBox.bottom < (int)_viewBox.top)
        {
            _viewBox.bottom = _viewBox.top;
        }
        if ((int)_viewBox.Height() < _cornerRect.top + _cornerRect.bottom)
        {
            _viewBox.bottom = _viewBox.top + (_cornerRect.top + _cornerRect.bottom);
        }
        break;
    }
}

void ImageEditPanel::CoercedViewcorner(int iMode)
{
    switch (iMode)
    {
    case ImageDragMode::idmGapLeft:
        if (_cornerRect.left < 0)
        {
            _cornerRect.left = 0;
        }
        else if (_cornerRect.left > (_viewBox.Width() - _cornerRect.right))
        {
            _cornerRect.left = _viewBox.Width() - _cornerRect.right;
        }
        break;

    case ImageDragMode::idmGapTop:
        if (_cornerRect.top < 0)
        {
            _cornerRect.top = 0;
        }
        else if (_cornerRect.top > (_viewBox.Height() - _cornerRect.bottom))
        {
            _cornerRect.top = _viewBox.Height() - _cornerRect.bottom;
        }
        break;

    case ImageDragMode::idmGapRight:
        if (_cornerRect.right < 0)
        {
            _cornerRect.right = 0;
        }
        else if (_cornerRect.right > (_viewBox.Width() - _cornerRect.left))
        {
            _cornerRect.right = _viewBox.Width() - _cornerRect.left;
        }
        break;

    case ImageDragMode::idmGapBottom:
        if (_cornerRect.bottom < 0)
        {
            _cornerRect.bottom = 0;
        }
        else if (_cornerRect.bottom > (_viewBox.Height() - _cornerRect.top))
        {
            _cornerRect.bottom = _viewBox.Height() - _cornerRect.top;
        }
        break;
    }
}

void ImageEditPanel::OnMouseMove(suic::MouseButtonEventArg* e)
{
    e->SetHandled(true);
    if (_dragTracking)
    {
        bool viewBoxChanged = false;
        int offsetX = e->GetMousePoint().x - _startPoint.x;
        int offsetY = e->GetMousePoint().y - _startPoint.y;

        Float newVal = 0.0f;

        switch (_dragMode)
        {
        case ImageDragMode::idmImgLeft:
            newVal = offsetX / _zoomRatio;
            _viewBox.left = _startValue + newVal;
            CoercedViewbox(_dragMode);
            viewBoxChanged = true;
            break;

        case ImageDragMode::idmImgTop:
            newVal = offsetY / _zoomRatio;
            _viewBox.top = _startValue + newVal;
            CoercedViewbox(_dragMode);
            viewBoxChanged = true;
            break;

        case ImageDragMode::idmImgRight:
            newVal = offsetX / _zoomRatio;
            _viewBox.right = _startValue + newVal;
            CoercedViewbox(_dragMode);
            viewBoxChanged = true;
            break;

        case ImageDragMode::idmImgBottom:
            newVal = offsetY / _zoomRatio;
            _viewBox.bottom = _startValue + newVal;
            CoercedViewbox(_dragMode);
            viewBoxChanged = true;
            break;

        case ImageDragMode::idmGapLeft:
            newVal = offsetX / _zoomRatio;
            _cornerRect.left = _startValue + newVal;
            CoercedViewcorner(_dragMode);
            break;

        case ImageDragMode::idmGapTop:
            newVal = offsetY / _zoomRatio;
            _cornerRect.top = _startValue + newVal;
            CoercedViewcorner(_dragMode);
            break;

        case ImageDragMode::idmGapRight:
            newVal = offsetX / _zoomRatio;
            _cornerRect.right = _startValue - newVal;
            CoercedViewcorner(_dragMode);
            break;

        case ImageDragMode::idmGapBottom:
            newVal = offsetY / _zoomRatio;
            _cornerRect.bottom = _startValue - newVal;
            CoercedViewcorner(_dragMode);
            break;
        }

        ImageEditWindow* imgWnd = GetImageEditWindow(this);

        if (viewBoxChanged)
        {
            imgWnd->NotifyViewboxChanged(_viewBox);
        }
        else
        {
            imgWnd->NotifyViewcornerChanged(_cornerRect);
        }

        InvalidateVisual();
    }
}

void ImageEditPanel::OnMouseLeftButtonDown(suic::MouseButtonEventArg* e)
{
    e->SetHandled(true);
    if (_dragMode != ImageDragMode::idmNone)
    {
        SetCaptureMouse();
        _dragTracking = true;
        _startPoint = e->GetMousePoint();

        switch (_dragMode)
        {
        case ImageDragMode::idmImgLeft:
            _startValue = _viewBox.left;
            break;

        case ImageDragMode::idmImgTop:
            _startValue = _viewBox.top;
            break;

        case ImageDragMode::idmImgRight:
            _startValue = _viewBox.right;
            break;

        case ImageDragMode::idmImgBottom:
            _startValue = _viewBox.bottom;
            break;

        case ImageDragMode::idmGapLeft:
            _startValue = _cornerRect.left;
            break;

        case ImageDragMode::idmGapTop:
            _startValue = _cornerRect.top;
            break;

        case ImageDragMode::idmGapRight:
            _startValue = _cornerRect.right;
            break;

        case ImageDragMode::idmGapBottom:
            _startValue = _cornerRect.bottom;
            break;
        }
    }
}

void ImageEditPanel::OnMouseLeftButtonUp(suic::MouseButtonEventArg* e)
{
    e->SetHandled(true);
    if (_dragTracking)
    {
        _dragTracking = false;
        ReleaseCaptureMouse();
    }
}

void ImageEditPanel::DrawPanelBk(Drawing* drawing)
{
    const int RECSIZE = 8;
    fRect rect(0, 0, GetRenderSize().Width(), GetRenderSize().Height());
    int iBeg = RECSIZE;
    Color clrs[2] = {0xFFFFFFFF, 0xFFCCCCCC};

    drawing->EraseRect(suic::DrawCtx::DefDraw, clrs[0], &rect);

    for (int y = 0; y < (int)rect.bottom;)
    {
        for (int x = iBeg; x < (int)rect.right;)
        {
            fRect rect(x, y, RECSIZE, RECSIZE);
            drawing->EraseRect(suic::DrawCtx::DefDraw, clrs[1], &rect);
            x += 16;
        }
        y += RECSIZE;
        iBeg = iBeg == 0 ? RECSIZE : 0;
    }
}

void ImageEditPanel::DrawEditImage(Drawing* drawing)
{
    if (NULL != _editBmp)
    {
        suic::Bitmap* pBmp = _editBmp->GetImageSource()->GetBitmap();
        suic::fRect rcdc = GetImageDrawRect();
        suic::fRect rcimg(0, 0, pBmp->Width(), pBmp->Height());

        suic::Float widRatio = rcdc.Width() / rcimg.Width();
        suic::Float heiRatio = rcdc.Height() / rcimg.Height();

        if (heiRatio > widRatio)
        {
            _zoomRatio = widRatio;
        }
        else
        {
            _zoomRatio = heiRatio;
        }

        if (_zoomRatio > 1.0f)
        {
            _zoomRatio = 1.0f;
        }

        suic::Float fWid = _zoomRatio * rcimg.Width();
        suic::Float fHei = _zoomRatio * rcimg.Height();

        _drawRect.left = rcdc.left + (rcdc.Width() - fWid) / 2;
        _drawRect.top = rcdc.top + (rcdc.Height() - fHei) / 2;
        _drawRect.right = _drawRect.left + fWid;
        _drawRect.bottom = _drawRect.top + fHei;

        drawing->DrawImage(suic::DrawCtx::DefDraw, pBmp, &_drawRect, &rcimg, 255);

        CalcDrawRectMeta();
    }
}

fPoint ImageEditPanel::ConvertPoint(fPoint pt)
{
    return pt;
}

fPoint ImageEditPanel::GetHoriUp(fRect rect)
{
    suic::Float iTop = _cornerRect.top * _zoomRatio;
    fPoint pt(rect.right + 10, rect.top + iTop - 5);
    return ConvertPoint(pt);
}

fPoint ImageEditPanel::GetHoriDown(fRect rect)
{
    suic::Float iBottom = _cornerRect.bottom * _zoomRatio;
    fPoint pt(rect.right + 10, rect.bottom - iBottom - 5);
    return ConvertPoint(pt);
}

fPoint ImageEditPanel::GetVertLeft(fRect rect)
{
    suic::Float iLeft = _cornerRect.left * _zoomRatio;
    fPoint pt(rect.left + iLeft - 5, rect.bottom + 10);
    return ConvertPoint(pt);
}

fPoint ImageEditPanel::GetVertRight(fRect rect)
{
    suic::Float iRight = _cornerRect.right * _zoomRatio;
    fPoint pt(rect.right - iRight - 5, rect.bottom + 10);
    return ConvertPoint(pt);
}

fRect ImageEditPanel::GetDrawViewbox()
{
    fRect rect = ConvertDrawRect(GetViewbox());
    rect.Offset(_drawRect.left, _drawRect.top);
    return rect;
}

fRect ImageEditPanel::ConvertDrawRect(fRect val)
{
    fRect fRc;
    fRc.left = val.left * _zoomRatio;
    fRc.top = val.top * _zoomRatio;
    fRc.right = val.right * _zoomRatio;
    fRc.bottom = val.bottom * _zoomRatio;
    return fRc;
}

fPoint ImageEditPanel::ConvertDrawPoint(fPoint val)
{
    fPoint fPt;
    fPt.x = val.x * _zoomRatio;
    fPt.y = val.y * _zoomRatio;
    return fPt;
}

Float ImageEditPanel::ConvertDrawCoor(Float val)
{
    return val * _zoomRatio;
}

void ImageEditPanel::DrawMetas(Drawing* drawing)
{
    suic::Pen pen;
    suic::Pen penGap;
    fPoint point;
    fRect drawRc = GetDrawViewbox();

    // 不需要反锯齿
    pen.SetAntiAlias(false);
    // 先绘制实际需要显示的图像框线
    pen.SetThickness(1);
    pen.SetBrush(suic::SolidColorBrush::Red);

    penGap.SetAntiAlias(false);
    penGap.GetDashStyle()->dashes.Add(3);
    penGap.GetDashStyle()->dashes.Add(2);
    penGap.SetThickness(1);
    penGap.SetBrush(suic::SolidColorBrush::Blue);

    drawing->DrawRect(suic::DrawCtx::DefDraw, NULL, &pen, drawRc);

    // 绘制拖曳框
    for (int i = 0; i < 4; ++i)
    {
        drawing->DrawRect(suic::DrawCtx::DefDraw, SolidColorBrush::Green, NULL, _drawMetas[i].TofRect());
    }

    //-----------------------------------------------------------

    // 绘制水平
    point = GetHoriUp(drawRc);
    drawing->PushOffset(point);
    drawing->DrawGeometry(suic::DrawCtx::DefDraw, SolidColorBrush::Blue, NULL, _horiGap);                   
    drawing->PopOffset();

    drawing->DrawLine(suic::DrawCtx::DefDraw, &penGap, fPoint(drawRc.left, point.y + 5), fPoint(drawRc.right + 10, point.y + 5));

    point = GetHoriDown(drawRc);
    drawing->PushOffset(point);
    drawing->DrawGeometry(suic::DrawCtx::DefDraw, SolidColorBrush::Blue, NULL, _horiGap);                   
    drawing->PopOffset();

    drawing->DrawLine(suic::DrawCtx::DefDraw, &penGap, fPoint(drawRc.left, point.y + 5), fPoint(drawRc.right + 10, point.y + 5));

    // 绘制垂直
    point = GetVertLeft(drawRc);
    drawing->PushOffset(point);
    drawing->DrawGeometry(suic::DrawCtx::DefDraw, SolidColorBrush::Blue, NULL, _vertGap);                   
    drawing->PopOffset();

    drawing->DrawLine(suic::DrawCtx::DefDraw, &penGap, fPoint(point.x + 5, drawRc.top), fPoint(point.x + 5, drawRc.bottom + 10));

    point = GetVertRight(drawRc);
    drawing->PushOffset(point);
    drawing->DrawGeometry(suic::DrawCtx::DefDraw, SolidColorBrush::Blue, NULL, _vertGap);                   
    drawing->PopOffset();

    drawing->DrawLine(suic::DrawCtx::DefDraw, &penGap, fPoint(point.x + 5, drawRc.top), fPoint(point.x + 5, drawRc.bottom + 10));
}

void ImageEditPanel::OnRender(Drawing* drawing)
{
    DrawPanelBk(drawing);
    DrawEditImage(drawing);

    CheckViewbox();
    DrawMetas(drawing);
}

void ImageEditPanel::CheckViewbox()
{
    int iImgWid = _editBmp->GetImageSource()->Width();
    int iImgHei = _editBmp->GetImageSource()->Height();
    suic::Rect rect = _viewBox.ToRect();

    rect.left = max(0, rect.left);
    rect.right = min(iImgWid, rect.right);
    rect.right = max(0, rect.right);

    if (rect.left > rect.right)
    {
        rect.left = rect.right;
    }

    rect.top = max(0, rect.top);
    rect.bottom = min(iImgHei, rect.bottom);
    rect.bottom = max(0, rect.bottom);

    if (rect.top > rect.bottom)
    {
        rect.top = rect.bottom;
    }

    //_cornerRect.left = max(0, _cornerRect.left);
    //_cornerRect.right = max(0, _cornerRect.right);

    _viewBox = rect.TofRect();
}

void ImageEditPanel::CheckViewCorner()
{
}

suic::fRect ImageEditPanel::GetImageDrawRect()
{
    suic::fRect rcdc(Point(), GetRenderSize());
    rcdc.Deflate(10, 10);
    rcdc.right -= 12;
    rcdc.bottom -= 12;

    return rcdc;
}

void ImageEditPanel::CalcDrawRectMeta()
{
    suic::Rect rect = ConvertDrawRect(GetViewbox()).ToRect();
    rect.Offset(_drawRect.left, _drawRect.top);

    int iWid = 4;
    int iSelfWid = 2;
    int iVertCenter = (rect.top + rect.bottom) / 2;
    int iHoriCenter = (rect.left + rect.right) / 2;

    _drawMetas[0].left = rect.left - iWid;
    _drawMetas[0].right = rect.left;
    _drawMetas[0].top = iVertCenter - iWid;
    _drawMetas[0].bottom = iVertCenter + iWid;

    _drawMetas[1].left = iHoriCenter - iWid;
    _drawMetas[1].right = iHoriCenter + iWid;
    _drawMetas[1].top = rect.top - iWid;
    _drawMetas[1].bottom = rect.top;

    _drawMetas[2].left = rect.right + 1;
    _drawMetas[2].right = rect.right + iWid + 1;
    _drawMetas[2].top = _drawMetas[0].top;
    _drawMetas[2].bottom = _drawMetas[0].bottom;

    _drawMetas[3].left = _drawMetas[1].left;
    _drawMetas[3].right = _drawMetas[1].right;
    _drawMetas[3].top = rect.bottom + 1;
    _drawMetas[3].bottom = rect.bottom + iWid + 1;
}

suic::fRect ImageEditPanel::GetViewbox()
{
    return _viewBox;
}

suic::ImageBrush* ImageEditPanel::GetEditBitmap() const
{
    return _editBmp;
}

void ImageEditPanel::SetEditBitmap(suic::ImageBrush* bmp)
{
    _editBmp = bmp;
    _cornerRect = bmp->GetViewCorner().TofRect();
    _viewBox = bmp->GetViewbox();

    // 图像实际绘制区域
    _rcViewbox = bmp->GetViewbox();
    // 图像四个边角
    _rcViewCorner = bmp->GetViewCorner().TofRect();
    // 图像绘制区域像素单位
    _viewboxMode = bmp->GetViewboxUnits();

    // 如果是相对坐标，则先换算为绝对坐标，方便统一编辑
    if (bmp->GetViewboxUnits() == BrushMappingMode::bmmRelativeToBoundingBox)
    {
        int iImgWid = bmp->GetImageSource()->Width();
        int iImgHei = bmp->GetImageSource()->Height();

        _viewBox.left = iImgWid * _viewBox.left;
        _viewBox.right = iImgWid * _viewBox.right;
        _viewBox.top = iImgHei * _viewBox.top;
        _viewBox.bottom = iImgHei * _viewBox.bottom;
    }

    _viewBox.right += _viewBox.left;
    _viewBox.bottom += _viewBox.top;

    CheckViewbox();

    ImageEditWindow* imgWnd = GetImageEditWindow(this);

    imgWnd->NotifyViewboxChanged(_viewBox);
    imgWnd->NotifyViewcornerChanged(_cornerRect);
}

suic::fRect ImageEditPanel::UpdateViewbox(suic::fRect rect, int iMode)
{
    _viewBox = rect;
    CoercedViewbox(iMode);
    CalcDrawRectMeta();

    InvalidateVisual();
    return _viewBox;
}

suic::fRect ImageEditPanel::UpdateViewcorner(suic::fRect rect, int iMode)
{
    _cornerRect = rect;
    CoercedViewcorner(iMode);
    CalcDrawRectMeta();
    InvalidateVisual();
    return _cornerRect;
}

void ImageEditPanel::Restore()
{
    //suic::fRect viewBox = GetViewbox();

    _editBmp->SetViewbox(_rcViewbox);
    _editBmp->SetViewcorner(_rcViewCorner.ToRect());
    _editBmp->SetViewboxUnits(_viewboxMode);
}

fRect ImageEditPanel::CalcRealViewbox()
{
    suic::fRect viewBox;
    suic::Float fImgWid = (suic::Float)_editBmp->GetImageSource()->Width();
    suic::Float fImgHei = (suic::Float)_editBmp->GetImageSource()->Height();

    if (_editBmp->GetViewboxUnits() == BrushMappingMode::bmmRelativeToBoundingBox)
    {
        viewBox.left = _viewBox.left / fImgWid;
        viewBox.top = _viewBox.top / fImgHei;
        viewBox.right = _viewBox.Width() / fImgWid;
        viewBox.bottom = _viewBox.Height() / fImgHei;
    }
    else
    {
        viewBox.left = _viewBox.left;
        viewBox.top = _viewBox.top;
        viewBox.right = _viewBox.Width();
        viewBox.bottom = _viewBox.Height();
    }

    return viewBox;
}

void ImageEditPanel::Commit()
{
    suic::fRect viewBox = CalcRealViewbox();

    _editBmp->SetViewbox(viewBox);
    _editBmp->SetViewcorner(_cornerRect.ToRect());
}
