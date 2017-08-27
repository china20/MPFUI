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

}

bool GifBox::StaticInit()
{
    if (NULL == UriProperty)
    {
        UriProperty = suic::DpProperty::Register(_U("Uri"), RTTIType(), suic::OString::RTTIType(), 
            suic::DpPropMemory::GetPropMeta(NULL, suic::PropMetadataOptions::AffectsParentMeasure, &GifBox::OnUriPropChanged));
        PlayProperty = suic::DpProperty::Register(_U("Play"), RTTIType(), suic::Boolean::RTTIType(), 
            suic::DpPropMemory::GetPropMeta(suic::Boolean::False, suic::PropMetadataOptions::AffectsRender, &GifBox::OnPlayPropChanged));
    }

    return true;
}

suic::String GifBox::GetUri()
{
    return GetValue(UriProperty)->ToString();
}

void GifBox::SetUri(suic::String uri)
{
    SetValue(UriProperty, new suic::OString(uri));
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

void GifBox::OnRender(suic::Drawing * drawing)
{
    suic::FrameworkElement::OnRender(drawing);
    if (NULL != _parser)
    {
        int iDelay = _parser->GetDelay();
        suic::Bitmap bmp;
        _parser->GetImage(bmp);
        suic::fRect rcDraw(0, 0, GetActualWidth(), GetActualHeight());
        suic::fRect rcImg(0, 0, bmp.Width(), bmp.Height());

        if (iDelay <= 0)
        {
            iDelay = 100;
        }

        drawing->DrawImage(&bmp, rcDraw, rcImg, 255);
        _timer->SetInterval(iDelay);
        _timer->Start();
    }
}

void GifBox::OnTick(suic::Object* sender, suic::EventArg* e)
{
    if (NULL != _parser)
    {
        _parser->MoveNext();
        InvalidateVisual();
        _timer->Stop();
    }
}

void GifBox::OnInitialized(suic::EventArg* e)
{
    FrameworkElement::OnInitialized(e);
    _timer->SetTick(suic::EventHandler(this, &GifBox::OnTick));
}

void GifBox::OnLoaded(suic::LoadedEventArg* e)
{
    FrameworkElement::OnLoaded(e);    
}

}
