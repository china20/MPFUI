
#include "stdafx.h"

#include <Extend/Editor/ImageEmbbed.h>
#include <Extend/Editor/RichTextBox.h>

namespace suic
{

ImplementRTTIOfClass(Embbed, suic::Object)
ImplementRTTIOfClass(ImageEmbbed, Embbed)
ImplementRTTIOfClass(PlayImageEmbbed, Embbed)

struct ImageHead
{
    int cbSize;
    int bits;
    int wid;
    int hei;
    int bytes;
};

struct PlayImageStream
{
    int cbSize;
    int bytes;
    int count;
};

struct PlayImageHead : public ImageHead
{
    int delay;
    suic::Size size;
    suic::Point offet;
};

Embbed::Embbed()
{
}

Embbed::~Embbed()
{
}

Object* Embbed::GetHolder() const
{
    return _holder.GetTarget();
}

void Embbed::SetHolder(suic::Object* holder)
{
    _holder = holder;
}

void Embbed::Invalidate(bool bForce)
{
    suic::Element* pTarget = _holder.GetElement<suic::Element>();
    if (NULL != pTarget)
    {
        pTarget->InvalidateRect(NULL, bForce);
    }
}

void Embbed::Render(suic::Drawing* drawing)
{
    OnRender(drawing);
}

int Embbed::GetType()
{
    return 0;
}

void Embbed::Restore(int type, suic::ISStream* strm)
{
    OnRestore(type, strm);
}

void Embbed::Save(suic::ISStream* strm)
{
    OnSave(strm);
}

suic::Size Embbed::GetDesiredSize()
{
    return _desiredSize;
}

suic::Point Embbed::GetVisualOffet() const
{
    return _offset;
}

suic::Size Embbed::GetRenderSize() const
{
    return _renderSize;
}

void Embbed::Arrange(const suic::Rect& arrangeRect)
{
    OnArrange(arrangeRect);
}

void Embbed::Measure(const suic::Size& contraint)
{
    OnMeasure(contraint);
}

void Embbed::OnArrange(const suic::Rect& arrangeRect)
{
    _offset = arrangeRect.LeftTop();
    _renderSize = arrangeRect.ToSize();
}

void Embbed::OnMeasure(const suic::Size& contraint)
{

}

void Embbed::OnRender(suic::Drawing* drawing)
{

}

void Embbed::OnSave(suic::ISStream* strm)
{

}

void Embbed::OnRestore(int type, suic::ISStream* strm)
{

}

//==============================================
//

ImageEmbbed::ImageEmbbed()
{
    _bitmap.setAutoDelete(false);
}

ImageEmbbed::~ImageEmbbed()
{

}

void ImageEmbbed::OnMeasure(const suic::Size& contraint)
{
    suic::Bitmap* bmp = GetBitmap();
    if (bmp->IsValid())
    {
        _desiredSize.cx = bmp->Width();
        _desiredSize.cy = bmp->Height();
    }
    else
    {
        _desiredSize = suic::Size();
    }
}

void ImageEmbbed::OnRender(suic::Drawing* drawing)
{
    suic::fRect rect(GetVisualOffet(), GetRenderSize());
    suic::fRect rcImg(0, 0, GetBitmap()->Width(), GetBitmap()->Height());
    drawing->DrawImage(DrawCtx::DefDraw, GetBitmap(), rect, rcImg);
}

void ImageEmbbed::OnSave(suic::ISStream* strm)
{
    if (GetType() == 0)
    {
        suic::String strPath = _bitmap.GetUri().ToString();
        suic::Uint32 size = strPath.Length();
        strm->Write((suic::Byte*)&size, 4);
        strm->Write((suic::Byte*)strPath.c_str(), strPath.Length() * 2);
    }
    else
    {
        ImageHead imgHead;
        suic::Bitmap* bmp = _bitmap.GetBitmap();

        imgHead.cbSize = sizeof(ImageHead);
        imgHead.bits = bmp->GetBitsPerPixel();
        imgHead.wid = bmp->Width();
        imgHead.hei = bmp->Height();
        imgHead.bytes = bmp->GetSize();

        strm->Write((suic::Byte*)&imgHead, sizeof(imgHead));
        strm->Write(bmp->GetPixels(), imgHead.bytes);
    }
}

void ImageEmbbed::OnRestore(int type, suic::ISStream* strm)
{
    suic::Uint32 size = 0;

    if (type == 0)
    {
        suic::String strPath;
        strm->Read((suic::Byte*)&size, 4);
        strPath.Resize(size);
        strm->Read((suic::Byte*)strPath.c_str(), size * 2);
        _bitmap.SetUri(strPath);
    }
    else
    {
        ImageHead imgHead;
        suic::Bitmap* bmp = GetBitmap();

        strm->Read((suic::Byte*)&imgHead, sizeof(imgHead));

        if (imgHead.cbSize == sizeof(ImageHead))
        {
            bmp->SetConfig(imgHead.wid, imgHead.hei, imgHead.bits);
            bmp->AllocPixel(0);
            strm->Read(bmp->GetPixels(), imgHead.bytes);
        }
    }
}

//void ImageEmbbed::Reflesh()
//{
//    if (GetBitmap()->IsValid())
//    {
//        SetSize(suic::Size(GetBitmap()->Width(), GetBitmap()->Height()));
//    }
//}

suic::Bitmap* ImageEmbbed::GetBitmap()
{
    return _bitmap.GetBitmap();
}

suic::ImageSource* ImageEmbbed::GetImageSource()
{
    return &_bitmap;
}

bool ImageEmbbed::Load(const suic::String& path)
{
    _bitmap.SetUri(path);
    return GetBitmap()->IsValid();
}

int ImageEmbbed::GetType()
{
    if (!_bitmap.GetUri().GetResourcePath().Empty())
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

//---------------------------------------------

PlayImageEmbbed::PlayImageEmbbed()
{
    _currentFrame = 0;
    _timer = NULL;
}

PlayImageEmbbed::~PlayImageEmbbed()
{
    Clear();
}

void PlayImageEmbbed::Clear()
{
    if (NULL != _timer)
    {
        _timer->SetEnabled(false);
        _timer->unref();
        _timer = NULL;
    }

    for (int i = 0; i < _aniFrames.GetCount(); ++i)
    {
        _aniFrames[i].bmp->unref();
    }
    _aniFrames.Clear();
}

void PlayImageEmbbed::OnTimer(suic::Object* sender, suic::EventArg* e)
{
    ++_currentFrame;
    
    if (_currentFrame >= _aniFrames.GetCount())
    {
        _currentFrame = 0;
    }

    _timer->SetEnabled(false);
    Invalidate(false);
}

void PlayImageEmbbed::SetPlaySize(suic::Size size)
{
    _playSize = size;
}

void PlayImageEmbbed::AddImage(suic::ImageSource* imgSource, suic::Point offset, suic::Size size, int iDelay)
{
    if (NULL == imgSource)
    {
        return;
    }

    imgSource->ref();

    suic::Bitmap* bmp = imgSource->GetBitmap();

    if (bmp->IsValid())
    {
        ImFrame frm;

        frm.bmp = imgSource;
        frm.offet = offset;
        frm.size = size;
        frm.delay = iDelay;
        imgSource->ref();

        if (size.cx == 0 || size.cy == 0)
        {
            frm.size.cx = bmp->Width();
            frm.size.cy = bmp->Height();
        }

        _aniFrames.Add(frm);

        if (_playSize.cx == 0 || _playSize.cy == 0)
        {
            _desiredSize.cx = max(frm.size.cx, _desiredSize.cx);
            _desiredSize.cy = max(frm.size.cy, _desiredSize.cy);
        }
        else
        {
            _desiredSize = _playSize;
        }
    }

    imgSource->unref();
}

void PlayImageEmbbed::OnRender(suic::Drawing* drawing)
{
    suic::fRect rect(GetVisualOffet(), GetRenderSize());
    ImFrame* pImf = GetCurrentFrame();

    if (NULL != pImf)
    {
        if (NULL == _timer)
        {
            _timer = new suic::AssignerTimer(pImf->delay);
            _timer->ref();
            _timer->SetTick(suic::EventHandler(this, &PlayImageEmbbed::OnTimer));
        }

        _timer->Start();

        suic::fRect rcImg(pImf->offet, pImf->size);
        drawing->DrawImage(DrawCtx::DefDraw, pImf->bmp->GetBitmap(), rect, rcImg);
    }
    else if (NULL != _timer)
    {
        _timer->SetEnabled(false);
    }
}

void PlayImageEmbbed::OnSave(suic::ISStream* strm)
{
    if (_aniFrames.GetCount() > 0)
    {
        PlayImageStream playHead;

        playHead.cbSize = sizeof(PlayImageStream);
        playHead.count = _aniFrames.GetCount();
        playHead.bytes = 8899;

        strm->Write((suic::Byte*)&playHead, sizeof(playHead));

        for (int i = 0; i < _aniFrames.GetCount(); ++i)
        {
            PlayImageHead imgHead;
            ImFrame& imf = _aniFrames[i];
            suic::Bitmap* bmp = imf.bmp->GetBitmap();

            imgHead.cbSize = sizeof(PlayImageHead);
            imgHead.bits = bmp->GetBitsPerPixel();
            imgHead.wid = bmp->Width();
            imgHead.hei = bmp->Height();
            imgHead.bytes = bmp->GetSize();
            imgHead.delay = imf.delay;
            imgHead.offet = imf.offet;
            imgHead.size = imf.size;

            strm->Write((suic::Byte*)&imgHead, sizeof(imgHead));
            strm->Write(bmp->GetPixels(), imgHead.bytes);
        }
    }
}

void PlayImageEmbbed::OnRestore(int type, suic::ISStream* strm)
{
    PlayImageStream playHead;
    strm->Read((suic::Byte*)&playHead, sizeof(PlayImageStream));

    if (playHead.bytes == 8899 && playHead.cbSize == sizeof(PlayImageStream))
    {
        Clear();

        for (int i = 0; i < playHead.count; ++i)
        {
            _aniFrames.Add(ImFrame());
            PlayImageHead imgHead;
            ImFrame& imf = _aniFrames.GetItem(_aniFrames.GetCount() - 1);

            strm->Read((suic::Byte*)&imgHead, sizeof(PlayImageHead));

            if (imgHead.cbSize == sizeof(PlayImageHead))
            {
                imf.bmp = new suic::ImageSource();
                imf.bmp->ref();
                suic::Bitmap* bmp = imf.bmp->GetBitmap();

                imf.delay = imgHead.delay;
                imf.offet = imgHead.offet;
                imf.size = imgHead.size;

                bmp->SetConfig(imgHead.wid, imgHead.hei, imgHead.bits);
                bmp->AllocPixel(0);
                strm->Read(bmp->GetPixels(), imgHead.bytes);
            }
            else
            {
                break;
            }
        }
    }
}

PlayImageEmbbed::ImFrame* PlayImageEmbbed::GetCurrentFrame()
{
    if (_currentFrame >= 0 && _currentFrame < _aniFrames.GetCount())
    {
        return &(_aniFrames[_currentFrame]);
    }
    else
    {
        return NULL;
    }
}

}
