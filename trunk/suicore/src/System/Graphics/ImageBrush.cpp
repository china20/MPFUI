// ============================================================================
// ImageBrush.cpp

#include <System/Types/String.h>
#include <System/Windows/CoreTool.h>
#include <System/Graphics/ImageBrush.h>
#include <System/Render/ImageResourcePool.h>

namespace suic
{

ImplementRTTIOfClass(ImageBrush, TileBrush)
ImplementRTTIOfClass(Resource, Object)
ImplementRTTIOfClass(Brush, Resource)
ImplementRTTIOfClass(TileBrush, Brush)

TmParam::TmParam()
    : charCount(0)
    , tmFlag(0)
{

}

//===========================================================================
//
Brush* Brush::Clone() const
{
    return const_cast<Brush*>(this);
}

Color Brush::ToColor()
{
    return 0xFF000000;
}

bool Brush::Parse(const String& xml)
{
    return false;
}

bool Brush::Parse(IXamlNode* pNode)
{
    return false;
}

TileBrush::TileBrush()
    : _stretch(Stretch::stFill)
    , _tileMode(TileMode::tmNone)
    , _alignmentX(AlignmentX::axCenter)
    , _alignmentY(AlignmentY::ayCenter)
    , _viewBoxUnits(BrushMappingMode::bmmRelativeToBoundingBox)
    , _viewportUnits(BrushMappingMode::bmmRelativeToBoundingBox)
{
    _viewbox.right = 1.0f;
    _viewbox.bottom = 1.0f;
    _viewport.right = 1.0f;
    _viewport.bottom = 1.0f;
}

TileBrush::~TileBrush()
{

}

suic::Byte TileBrush::GetAlignmentX() const
{
    return _alignmentX;
}

void TileBrush::GetAlignmentX(suic::Byte val)
{
    _alignmentX = val;
}

suic::Byte TileBrush::GetAlignmentY() const
{
    return _alignmentY;
}

void TileBrush::SetAlignmentY(suic::Byte val)
{
    _alignmentY = val;
}

suic::Byte TileBrush::GetViewboxUnits() const
{
    return _viewBoxUnits;
}

void TileBrush::SetViewboxUnits(suic::Byte val)
{
    _viewBoxUnits = val;
}

suic::Byte TileBrush::GetViewportUnits() const
{
    return _viewportUnits;
}

void TileBrush::SetViewportUnits(suic::Byte val)
{
    _viewportUnits = val;
}

suic::Byte TileBrush::GetStretch() const
{
    return _stretch;
}

void TileBrush::SetStretch(suic::Byte val)
{
    _stretch = val;
}

suic::Byte TileBrush::GetTileMode() const
{
    return _tileMode;
}

void TileBrush::SetTileMode(suic::Byte val)
{
    _tileMode = val;
}

fRect TileBrush::GetViewbox() const
{
    return _viewbox;
}

void TileBrush::SetViewbox(fRect val)
{
    _viewbox = val;
}

fRect TileBrush::GetViewport() const
{
    return _viewport;
}

void TileBrush::SetViewport(fRect val)
{
    _viewport = val;
}

ImageBrush::ImageBrush(String path)
    : _opacity(255)
    , _flag(0)
    , _maskColor(ARGB(255,0xF0,0xF0,0xF0))
    , _grey(0)
    , _source(NULL)
    , _drawLevel(DrawCtx::eDrawLevel::flLow)
{
    InitImageSource(path);
    _index = Brush::eBrushIndex::biImage;
}

ImageBrush::ImageBrush()
    : _opacity(255)
    , _flag(0)
    , _maskColor(ARGB(255,0xF0,0xF0,0xF0))
    , _grey(0)
    , _source(NULL)
    , _drawLevel(DrawCtx::eDrawLevel::flLow)
{
    _index = Brush::eBrushIndex::biImage;
}

ImageBrush::~ImageBrush()
{
    if (_source != NULL)
    {
        String strUri = _source->GetUri().GenerateKey();
        int iRef = _source->getRefCount();
        FREEREFOBJ(_source);
        if (iRef == 2)
        {
            ImageSourcePool::FreeImage(strUri);
        }
    }
}

ImageBrush* ImageBrush::Clone() const
{
    return const_cast<ImageBrush*>(this);
}

Byte ImageBrush::GetOpacity() const
{
    return _opacity;
}

void ImageBrush::SetOpacity(Byte val)
{
    _opacity = val;
}

Byte ImageBrush::GetDrawLevel() const
{
    return _drawLevel;
}

void ImageBrush::SetDrawLevel(Byte val)
{
    _drawLevel = val;
}

Rect ImageBrush::GetViewCorner() const
{
    return _viewCorner;
}

void ImageBrush::SetViewcorner(Rect val)
{
    _viewCorner = val;

    if (!_viewCorner.IsZero())
    {
        _flag |= 0x02;
    }
    else
    {
        _flag &= ~0x02;
    }
}

Int16 ImageBrush::ParseStretch(const String& value)
{
    Int16 iRet = Stretch::stNone;

    if (value.Equals(_T("UniformToFill")))
    {
        iRet = Stretch::stUniformToFill;
    }
    else if (value.Equals(_T("Uniform")))
    {
        iRet = Stretch::stUniform;
    }
    else if (value.Equals(_T("Fill")))
    {
        iRet = Stretch::stFill;
    }

    return iRet;
}

void ImageBrush::InitImageSource(const String& value)
{
    if (NULL != _source)
    {
        _source->unref();
    }
    _source = ImageSourcePool::Lookup(ResourceUri(value).GenerateKey());
    if (NULL == _source)
    {
        _source = new ImageSource(value);
        ImageSourcePool::AddImage(_source);
    }
    if (NULL != _source)
    {
        _source->ref();
    }
}

void ImageBrush::SetValue(const String& key, const String& value)
{
    int iSize = key.Length();
    switch (iSize)
    {
    case 4:
        if (key.Equals(_T("Grey")))
        {
            if (value.Equals(_T("True")))
            {
                _grey = 100;
            }
            else if (value.Equals(_T("False")))
            {
                _grey = 0;
            }
            else
            {
                _grey = (Byte)value.ToInt();
            }
        }
        break;

    case 6:
        if (key.Equals(_T("Source")))
        {
            InitImageSource(value);
        }
        break;

    case 7:
        if (key.Equals(_T("Viewbox")))
        {
            _viewbox = value.TofRect();
        }
        else if (key.Equals(_T("Opacity")))
        {
            _opacity = (Byte)(value.ToFloat() * 255.0);
        }
        else if (key.Equals(_T("Stretch")))
        {
            _stretch = ParseStretch(value);
        }
        break;

    case 8:
        if (key.Equals(_T("Viewport")))
        {
            _viewport = value.TofRect();
        }
        else  if (key.Equals(_T("TileMode")))
        {
            if (value.Equals(_T("Tile")))
            {
                _tileMode = TileMode::tmTile;
            }
            else if (value.Equals(_T("FlipX")))
            {
                _tileMode = TileMode::tmFlipX;
            }
            else if (value.Equals(_T("FlipY")))
            {
                _tileMode = TileMode::tmFlipY;
            }
            else if (value.Equals(_T("FlipXY")))
            {
                _tileMode = TileMode::tmFlipXY;
            }
            else
            {
                _tileMode = TileMode::tmNone;
            }
        }
        break;

    case 9:
        if (key.Equals(_T("DrawLevel")))
        {
            if (value.Equals(_T("Low")))
            {
                _drawLevel = DrawCtx::eDrawLevel::flLow;
            }
            else if (value.Equals(_T("Medium")))
            {
                _drawLevel = DrawCtx::eDrawLevel::flMedium;
            }
            else if (value.Equals(_T("High")))
            {
                _drawLevel = DrawCtx::eDrawLevel::flHigh;
            }
            else
            {
                _drawLevel = DrawCtx::eDrawLevel::flNone;
            }
        }
        break;

    case 10:
        if (key.CompareI(_T("Viewcorner")) == 0)
        {
            SetViewcorner(value.ToRect());
        }
        else if (key.Equals(_U("AlignmentX")))
        {
            if (value.Equals(_T("Left")))
            {
                _alignmentX = AlignmentX::axLeft;
            }
            else if (value.Equals(_T("Center")))
            {
                _alignmentX = AlignmentX::axCenter;
            }
            else if (value.Equals(_T("Right")))
            {
                _alignmentX = AlignmentX::axRight;
            }
        }
        else if (key.Equals(_U("AlignmentY")))
        {
            if (value.Equals(_T("Top")))
            {
                _alignmentY = AlignmentY::ayTop;
            }
            else if (value.Equals(_T("Center")))
            {
                _alignmentY = AlignmentY::ayCenter;
            }
            else if (value.Equals(_T("Bottom")))
            {
                _alignmentY = AlignmentY::ayBottom;
            }
        }
        break;

    case 11:
        if (key.Equals(_T("ImageSource")))
        {
            InitImageSource(value);
        }
        else if (key.Equals(_T("OpacityMask")))
        {
            _maskColor = value.ToColor();
        }
        break;

    case 12:
        if (key.Equals(_T("ViewboxUnits")))
        {
            if (value.Equals(_U("Absolute")))
            {
                _viewBoxUnits = BrushMappingMode::bmmAbsolute;
            }
            else if (value.Equals(_U("RelativeToBoundingBox")))
            {
                _viewBoxUnits = BrushMappingMode::bmmRelativeToBoundingBox;
            }
        }
        break;

    case 13:
        if (key.Equals(_T("ViewportUnits")))
        {
            if (value.Equals(_U("Absolute")))
            {
                _viewportUnits = BrushMappingMode::bmmAbsolute;
            }
            else if (value.Equals(_U("RelativeToBoundingBox")))
            {
                _viewportUnits = BrushMappingMode::bmmRelativeToBoundingBox;
            }
        }
        break;
    }
}

bool ImageBrush::Parse(const String& xml)
{
    return false;
}

bool ImageBrush::Parse(IXamlNode* pNode)
{
    IXamlAttris* pAttri = pNode->GetAttris();

    pAttri->Reset();          

    while (pAttri->HasNext())
    {
        String key(pAttri->GetName());
        String value(pAttri->GetValue());

        SetValue(key, value);
    }

    _key = pNode->FindAttri(_T("Key"));
    if (_key.Empty())
    {
        _key = pNode->FindAttri(_T("x:Key"));
    }

    return true;
}

static void AdjustAlignment(ImageBrush* brImg, fRect* rcDc, const fRect* lprc, Size& szReal)
{
    if (brImg->GetAlignmentX() == AlignmentX::axCenter)
    {
        rcDc->left += (lprc->Width() - szReal.cx) / 2.0f;
    }
    else if (brImg->GetAlignmentX() == AlignmentX::axRight)
    {
        rcDc->left += lprc->Width() - szReal.cx;
    }

    if (brImg->GetAlignmentY() == AlignmentY::ayCenter)
    {
        rcDc->top += (lprc->Height() - szReal.cy) / 2.0f;
    }
    else if (brImg->GetAlignmentY() == AlignmentY::ayBottom)
    {
        rcDc->top += lprc->Height() - szReal.cy;
    }

    rcDc->right = rcDc->left + szReal.cx;
    rcDc->bottom = rcDc->top + szReal.cy;
}

static void ComputeViewbox(ImageBrush* brImg, fRect& rcImage)
{
    Bitmap* pImage = brImg->GetImageSource()->GetBitmap();

    // 先计算图像块区域
    if (brImg->GetViewboxUnits() == BrushMappingMode::bmmAbsolute)
    {
        rcImage.left = brImg->GetViewbox().left;
        rcImage.top = brImg->GetViewbox().top;

        rcImage.right = rcImage.left + brImg->GetViewbox().right;
        rcImage.bottom = rcImage.top + brImg->GetViewbox().bottom;
    }
    else
    {
        rcImage.left = pImage->Width() * brImg->GetViewbox().left;
        rcImage.top = pImage->Height() * brImg->GetViewbox().top;
        rcImage.right = rcImage.left + pImage->Width() * brImg->GetViewbox().right;
        rcImage.bottom = rcImage.top + pImage->Height() * brImg->GetViewbox().bottom;
    }
}

static void ComputeViewport(ImageBrush* brImg, const fRect* lprc, fRect& rcCanvas)
{
    fRect vRect;

    if (brImg->GetViewportUnits() == BrushMappingMode::bmmAbsolute)
    {
        vRect = brImg->GetViewport();
    }
    else
    {
        vRect.left = lprc->Width() * brImg->GetViewport().left;
        vRect.top = lprc->Height() * brImg->GetViewport().top;
        vRect.right = lprc->Width() * brImg->GetViewport().right;
        vRect.bottom = lprc->Height() * brImg->GetViewport().bottom;
    }

    if (brImg->GetTileMode() != TileMode::tmNone)
    {
        rcCanvas.left = (int)vRect.left % (int)vRect.right;
        rcCanvas.top = (int)vRect.top % (int)vRect.bottom;

        if ((int)vRect.left > 0)
        {
            rcCanvas.left -= vRect.right;
        }

        if ((int)vRect.top > 0)
        {
            rcCanvas.top -= vRect.bottom;
        }
    }
    else
    {
        rcCanvas.left = vRect.left;
        rcCanvas.top = vRect.top;
    }

    rcCanvas.right = rcCanvas.left + vRect.right;
    rcCanvas.bottom = rcCanvas.top + vRect.bottom;
}

static void DrawViewCorner(DrawCtx* drawCtx, Drawing* drawing, ImageBrush* brImg, const fRect* lprc, const fRect& rcImage)
{
    fRect rcDc;
    fRect rcImg;

    Bitmap* pImage = brImg->GetImageSource()->GetBitmap();
    suic::Byte opacity = brImg->GetOpacity();
    Rect viewCorner = brImg->GetViewCorner();

    drawCtx->SetAlpha(opacity);

    // 左边框
    if (viewCorner.left > 0)
    {
        // 左上角
        if (viewCorner.top > 0)
        {
            rcDc.SetXYWH(lprc->left, lprc->top, viewCorner.left, viewCorner.top);
            rcImg.SetXYWH(rcImage.left, rcImage.top, viewCorner.left, viewCorner.top);

            drawing->DrawImage(drawCtx, pImage, rcDc, rcImg);
        }

        // 左下角
        if (viewCorner.bottom > 0)
        {
            rcDc.SetXYWH(lprc->left, lprc->bottom - viewCorner.bottom, viewCorner.left, viewCorner.bottom);
            rcImg.SetXYWH(rcImage.left, rcImage.bottom - viewCorner.bottom, viewCorner.left, viewCorner.bottom);

            drawing->DrawImage(drawCtx, pImage, rcDc, rcImg);
        }

        // 左边中部
        rcDc.SetLTRB(lprc->left, lprc->top + viewCorner.top
            , lprc->left + viewCorner.left, lprc->bottom - viewCorner.bottom);
        rcImg.SetLTRB(rcImage.left, rcImage.top + viewCorner.top
            , rcImage.left + viewCorner.left, rcImage.bottom - viewCorner.bottom);

        drawing->DrawImage(drawCtx, pImage, rcDc, rcImg);
    }

    // 右边框
    if (viewCorner.right > 0 && viewCorner.left < lprc->Width())
    {
        // 右上角
        if (viewCorner.top > 0)
        {
            rcDc.SetXYWH(lprc->right - viewCorner.right, lprc->top, viewCorner.right, viewCorner.top);
            rcImg.SetXYWH(rcImage.right - viewCorner.right, rcImage.top, viewCorner.right, viewCorner.top);

            drawing->DrawImage(drawCtx, pImage, rcDc, rcImg);
        }

        // 右下角
        if (viewCorner.bottom > 0)
        {
            rcDc.SetXYWH(lprc->right - viewCorner.right, lprc->bottom - viewCorner.bottom, viewCorner.right, viewCorner.bottom);
            rcImg.SetXYWH(rcImage.right - viewCorner.right, rcImage.bottom - viewCorner.bottom, viewCorner.right, viewCorner.bottom);

            drawing->DrawImage(drawCtx, pImage, rcDc, rcImg);
        }

        // 右边中部
        rcDc.SetLTRB(lprc->right - viewCorner.right, lprc->top + viewCorner.top, lprc->right, lprc->bottom - viewCorner.bottom);
        rcImg.SetLTRB(rcImage.right - viewCorner.right, rcImage.top + viewCorner.top, rcImage.right, rcImage.bottom - viewCorner.bottom);
        drawing->DrawImage(drawCtx, pImage, rcDc, rcImg);
    }

    // 上边框
    if (viewCorner.top > 0)
    {
        // 填充上边中间
        rcDc.SetLTRB(lprc->left + viewCorner.left, lprc->top, lprc->right - viewCorner.right, lprc->top + viewCorner.top);
        rcImg.SetLTRB(rcImage.left + viewCorner.left, rcImage.top, rcImage.right - viewCorner.right, rcImage.top + viewCorner.top);
        drawing->DrawImage(drawCtx, pImage, rcDc, rcImg);
    }

    // 下边框
    if (viewCorner.bottom > 0)
    {
        // 填充下边中间
        rcDc.SetLTRB(lprc->left + viewCorner.left, lprc->bottom - viewCorner.bottom, lprc->right - viewCorner.right, lprc->bottom);
        rcImg.SetLTRB(rcImage.left + viewCorner.left, rcImage.bottom - viewCorner.bottom, rcImage.right - viewCorner.right, rcImage.bottom);
        drawing->DrawImage(drawCtx, pImage, rcDc, rcImg);
    }

    // 绘制中间
    rcDc.left = lprc->left + viewCorner.left;
    rcDc.top = lprc->top + viewCorner.top;
    rcDc.right = lprc->right - viewCorner.right;
    rcDc.bottom = lprc->bottom - viewCorner.bottom;

    rcImg.left = rcImage.left + viewCorner.left;
    rcImg.top = rcImage.top + viewCorner.top;
    rcImg.right = rcImage.right - viewCorner.right;
    rcImg.bottom = rcImage.bottom - viewCorner.bottom;

    drawing->DrawImage(drawCtx, pImage, rcDc, rcImg);
}

static void DrawRealBitmap(DrawCtx* drawCtx, Drawing* drawing, ImageBrush* brImg, const fRect* lprc, const fRect* rcImage, const Size* szCanvas, fRect rcDc)
{
    Bitmap* pImage = brImg->GetImageSource()->GetBitmap();
    suic::Byte opacity = brImg->GetOpacity();

    drawCtx->SetAlpha(opacity);

    //-------------------------------------------------
    if (brImg->GetTileMode() == TileMode::tmNone)
    {
        drawing->DrawImage(drawCtx, pImage, rcDc, rcImage);
    }
    else if (szCanvas->cx > 0 && szCanvas->cy > 0)
    {
        Float fLeft = rcDc.left;
        Float fRight = rcDc.right;

        for (;;)
        {
            drawing->DrawImage(drawCtx, pImage, rcDc, rcImage);
            rcDc.left += szCanvas->cx;
            rcDc.right += szCanvas->cx;

            if ((int)rcDc.left >= (int)lprc->right)
            {
                rcDc.top += szCanvas->cy;
                rcDc.bottom += szCanvas->cy;

                if ((int)rcDc.top >= (int)lprc->bottom)
                {
                    break;
                }

                rcDc.left = fLeft;
                rcDc.right = fRight;
            }
        }
    }
}

static void DrawFillImageBrush(DrawCtx* drawCtx, Drawing* drawing, ImageBrush* brImg, const fRect* lprc)
{
    fRect rcDc;
    fRect rcImage;

    Bitmap* pImage = brImg->GetImageSource()->GetBitmap();

    // 计算实际绘制图像块区域
    ComputeViewbox(brImg, rcImage);

    if (brImg->HasViewCorner())
    {
        //drawing->DrawImage(pImage, lprc, rcImage, 255);
        DrawViewCorner(drawCtx, drawing, brImg, lprc, rcImage);
    }
    else
    {
        fRect rcCanvas;

        // 计算画布区域
        ComputeViewport(brImg, lprc, rcCanvas);

        Size szCanvas(rcCanvas.Width(), rcCanvas.Height());

        // 调正实际在画布中的绘制区域

        rcDc.left = lprc->left;
        rcDc.top = lprc->top;
        rcDc.right = lprc->left + szCanvas.cx;
        rcDc.bottom = lprc->top + szCanvas.cy;

        //-------------------------------------------------

        DrawRealBitmap(drawCtx, drawing, brImg, lprc, rcImage, &szCanvas, rcDc);
    }
}

static void DrawUniformToFillImageBrush(DrawCtx* drawCtx, Drawing* drawing, ImageBrush* brImg, const fRect* lprc)
{
    fRect rcDc;
    fRect rcCanvas;
    fRect rcImage;

    Bitmap* pImage = brImg->GetImageSource()->GetBitmap();

    // 计算实际绘制图像块区域
    ComputeViewbox(brImg, rcImage);

    // 计算画布区域
    ComputeViewport(brImg, lprc, rcCanvas);

    Size szCanvas(rcCanvas.Width(), rcCanvas.Height());

    // 实际绘制大小
    Size szReal;
    szReal.cy = rcCanvas.Height();

    Float fRatio = rcImage.Width() / rcImage.Height();
    szReal.cx = (int)(szReal.cy * fRatio);

    if (szReal.cx < (int)rcCanvas.Width())
    {
        szReal.cx = (int)rcCanvas.Width();
        szReal.cy = (int)(rcCanvas.Width() / fRatio);
    }

    // 调正实际在画布中的绘制区域

    rcDc.left = lprc->left + rcCanvas.left;
    rcDc.top = lprc->top + rcCanvas.top;

    AdjustAlignment(brImg, &rcDc, &rcCanvas, szReal);

    //-------------------------------------------------

    drawing->Save();
    drawing->ClipRect(lprc, ClipOp::OpIntersect);
    drawing->DrawImage(drawCtx, pImage, rcDc, rcImage);
    drawing->Restore();
}

static void DrawUniformImageBrush(DrawCtx* drawCtx, Drawing* drawing, ImageBrush* brImg, const fRect* lprc)
{
    fRect rcDc;
    fRect rcCanvas;
    fRect rcImage;

    Bitmap* pImage = brImg->GetImageSource()->GetBitmap();

    // 计算实际绘制图像块区域
    ComputeViewbox(brImg, rcImage);

    // 计算画布区域
    ComputeViewport(brImg, lprc, rcCanvas);

    Size szCanvas(rcCanvas.Width(), rcCanvas.Height());

    // 实际绘制大小
    Size szReal;
    Float width = 1.0;
    Float height = 1.0;

    width = szCanvas.Width() / rcImage.Width();
    height = szCanvas.Height() / rcImage.Height();

    Float fRatio = (width < height) ? width : height;

    if ((int)fRatio >= 1)
    {
        fRatio = 1.0f;
    }

    szReal.cx = rcImage.Width() * fRatio;
    szReal.cy = rcImage.Height() * fRatio;

    // 调正实际在画布中的绘制区域

    rcDc.left = lprc->left + rcCanvas.left;
    rcDc.top = lprc->top + rcCanvas.top;

    AdjustAlignment(brImg, &rcDc, &rcCanvas, szReal);

    //-------------------------------------------------
    DrawRealBitmap(drawCtx, drawing, brImg, lprc, rcImage, &szCanvas, rcDc);
}

static void DrawDefaultImageBrush(DrawCtx* drawCtx, Drawing* drawing, ImageBrush* brImg, const fRect* lprc)
{
    fRect rcDc;
    fRect rcCanvas;
    fRect rcImage;

    Bitmap* pImage = brImg->GetImageSource()->GetBitmap();

    // 计算实际绘制图像块区域
    ComputeViewbox(brImg, rcImage);

    // 计算画布区域
    ComputeViewport(brImg, lprc, rcCanvas);

    Size szCanvas(rcCanvas.Width(), rcCanvas.Height());

    // 实际绘制大小
    Size szReal;

    szReal.cx = (int)rcImage.Width();
    szReal.cy = (int)rcImage.Height();

    // 调正实际在画布中的绘制区域

    rcDc.left = lprc->left + rcCanvas.left;
    rcDc.top = lprc->top + rcCanvas.top;

    AdjustAlignment(brImg, &rcDc, &rcCanvas, szReal);

    //-------------------------------------------------
    
    DrawRealBitmap(drawCtx, drawing, brImg, lprc, rcImage, &szCanvas, rcDc);
}

void DrawImageBrush(DrawCtx* drawCtx, Drawing* drawing, ImageBrush* brImg, const fRect* lprc)
{
    suic::Byte bStretch = brImg->GetStretch();
    ImageSource* pImage = brImg->GetImageSource();

    if (!pImage)
    {
        return;
    }

    Bitmap* bmp = pImage->GetBitmap();

    if (brImg->GetGrey())
    {
        bmp->Backup();
        bmp->EraseGray();
    }

    drawCtx->SetDrawLevel((DrawCtx::eDrawLevel)brImg->GetDrawLevel());
    drawCtx->SetAlpha(brImg->GetOpacity());

    switch (bStretch)
    {
    case Stretch::stFill:
        // 直接填充
        DrawFillImageBrush(drawCtx, drawing, brImg, lprc);
        break;

    case Stretch::stUniformToFill:
        // 按照显示区域最大比率缩放
        DrawUniformToFillImageBrush(drawCtx, drawing, brImg, lprc);
        break;

    case Stretch::stUniform:
        // 按照显示区域最小比率缩放
        DrawUniformImageBrush(drawCtx, drawing, brImg, lprc);
        break;

    default:
        // 默认按照图像实际大小绘制
        DrawDefaultImageBrush(drawCtx, drawing, brImg, lprc);
    }

    if (brImg->GetGrey())
    {
        bmp->Restore();
        bmp->ResetBackup();
    }
}

//void ImageBrush::Draw(Drawing* drawing, const fRect* lprc)
//{
//    DrawImageBrush(drawing, this, lprc);
//}

ImageSource* ImageBrush::GetImageSource() const
{
    return _source;
}

void ImageBrush::SetImageSource(ImageSource* source)
{
    SETREFOBJ(_source, source);
}

Rect ImageBrush::GetContentBrounds()
{
    ImageSource* img = GetImageSource();
    if (NULL != img)
    {
        fRect rcImage;
        // 计算实际绘制图像块区域
        ComputeViewbox(this, rcImage);
        return rcImage.ToRect();
    }
    else
    {
        return Rect();
    }
}

bool ImageBrush::HasViewCorner() const
{
    return (_flag & 0x02) == 0x02;
}

void ImageBrush::SetGrey(bool val)
{
    _grey = (val ? 100 : 0);
}

bool ImageBrush::GetGrey() const
{
    return (_grey != 0);
}

}
