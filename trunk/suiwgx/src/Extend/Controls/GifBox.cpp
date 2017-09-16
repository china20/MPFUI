// ======================================================================
//
// Copyright (c) 2008-2022 Inc. All rights reserved.
//
// ◊˜’ﬂ£∫ÕÙ»Ÿ
// ” œ‰£∫china0851@foxmail.com
//
// ======================================================================

#include <Extend/Controls/GifBox.h>

namespace suic
{

ImplementRTTIOfClass(GifBox, suic::FrameworkElement)

suic::DpProperty* GifBox::UriProperty;
suic::DpProperty* GifBox::PlayProperty;

GifBox::GifBox()
    : _parser(NULL)
{
    GifBox::StaticInit();
    _timer = new suic::AssignerTimer();
    _timer->ref();
}

GifBox::~GifBox()
{
    if (NULL != _parser)
    {
        _parser->unref();
    }

    if (NULL != _timer)
    {
        _timer->unref();
    }
}

void GifBox::OnUriPropChanged(suic::DpObject* d, suic::DpPropChangedEventArg* e)
{
    GifBox* gifBox = suic::RTTICast<GifBox>(d);
    suic::ResourceUri resUri(e->GetNewValue()->ToString());
    if (NULL != gifBox->_parser)
    {
        gifBox->_parser->unref();
    }
    gifBox->_parser = new suic::GIFParser();
    gifBox->_parser->ref();
    gifBox->_parser->LoadUri(&resUri);
    gifBox->_parser->MoveNext();
}

void GifBox::OnPlayPropChanged(suic::DpObject* d, suic::DpPropChangedEventArg* e)
{
    GifBox* gifBox = suic::RTTICast<GifBox>(d);
    if (NULL != gifBox)
    {
        gifBox->StartGif();
    }
}

void GifBox::StaticInit()
{
    if (NULL == UriProperty)
    {
        UriProperty = suic::DpProperty::Register(_U("Uri"), RTTIType(), suic::OString::RTTIType(), 
            suic::DpPropMemory::GetPropMeta(NULL, suic::PropMetadataOptions::AffectsParentMeasure, &GifBox::OnUriPropChanged));
        PlayProperty = suic::DpProperty::Register(_U("Play"), RTTIType(), suic::Boolean::RTTIType(), 
            suic::DpPropMemory::GetPropMeta(suic::Boolean::False, suic::PropMetadataOptions::AffectsRender, &GifBox::OnPlayPropChanged));
    }
}

suic::String GifBox::GetUri()
{
    return GetValue(UriProperty)->ToString();
}

void GifBox::SetUri(suic::String uri)
{
    SetValue(UriProperty, new suic::OString(uri));
}

bool GifBox::IsPlay()
{
    return GetValue(PlayProperty)->ToBool();
}

suic::Size GifBox::OnMeasure(const suic::Size& constraint)
{
    suic::Size measureSize;
    if (NULL != _parser)
    {
        suic::ImageParse::Info info;
        _parser->GetInfo(info);
        measureSize.cx = info.wid;
        measureSize.cy = info.hei;
    }
    return measureSize;
}

void GifBox::OnArrange(const suic::Size& arrangeSize)
{
    FrameworkElement::OnArrange(arrangeSize);
}

void GifBox::OnRender(suic::Drawing * drawing)
{
    suic::FrameworkElement::OnRender(drawing);

    if (_bitmap.IsValid())
    {
        suic::fRect rect(0, 0, GetActualWidth(), GetActualHeight());
        suic::fRect rcImg(0, 0, _bitmap.Width(), _bitmap.Height());

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

        drawing->DrawImage(DrawCtx::DefDraw, &_bitmap, rect, rcImg);
    }
}

void GifBox::OnTick(suic::Object* sender, suic::EventArg* e)
{
    if (NULL != _parser)
    {
        int iDelay = 100;
        _parser->MoveNext();
        iDelay = _parser->GetDelay();
        _parser->GetImage(_bitmap);

        if (iDelay <= 0)
        {
            iDelay = 100;
        }
        
        _timer->Stop();
        _timer->SetInterval(iDelay);
        _timer->Start();

        InvalidateVisual();
    }
}

void GifBox::OnInitialized(suic::EventArg* e)
{
    FrameworkElement::OnInitialized(e);
    _timer->SetTick(suic::EventHandler(this, &GifBox::OnTick));

    StartGif();
}

void GifBox::OnLoaded(suic::LoadedEventArg* e)
{
    FrameworkElement::OnLoaded(e);    
}

void GifBox::StartGif()
{
    if (IsValid() && IsPlay())
    {
        _timer->Start();
    }
}

}
