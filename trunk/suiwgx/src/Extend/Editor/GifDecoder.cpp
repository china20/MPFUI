
#include "stdafx.h"
#include "GifDecoder.h"

GifDecoder::GifDecoder()
{
    _wicFactory = NULL;
    _decoder = NULL;
}

GifDecoder::~GifDecoder()
{
    if (_decoder)
    {
        _decoder->Release();
    }
    if (_wicFactory)
    {
        _wicFactory->Release();
    }
}

void GifDecoder::TestGif(String strPath)
{
    GifDecoder gif;
    gif.LoadFromFile(strPath);

    for (int i = 0; i < gif.GetFrameCount(); ++i)
    {
        GifFrame frame;
        gif.GetGifFrame(i, frame);
    }
}

bool GifDecoder::LoadFromRes(String strRes)
{
    return false;
}

bool GifDecoder::LoadFromFile(String strPath)
{
    HRESULT hr = S_OK;

    hr = CoCreateInstance(
        CLSID_WICImagingFactory,
        NULL,
        CLSCTX_INPROC_SERVER,
        IID_PPV_ARGS(&_wicFactory));

    hr = _wicFactory->CreateDecoderFromFilename(
        strPath.c_str(),
        NULL,
        GENERIC_READ,
        WICDecodeMetadataCacheOnLoad,
        &_decoder);

    UINT _cFrames= 0;
    IWICMetadataQueryReader *pMetadataQueryReader = NULL;
	
    hr = _decoder->GetFrameCount(&_cFrames);

	if (SUCCEEDED(hr))
	{
		hr = _decoder->GetMetadataQueryReader(&pMetadataQueryReader);
	}

    return hr == S_OK;
}

bool GifDecoder::LoadFromStream(IStream* strm)
{
    HRESULT hr = S_OK;

    hr = CoCreateInstance(
        CLSID_WICImagingFactory,
        NULL,
        CLSCTX_INPROC_SERVER,
        IID_PPV_ARGS(&_wicFactory));

    hr = _wicFactory->CreateDecoderFromStream(
        strm,
        NULL,
        WICDecodeMetadataCacheOnLoad,
        &_decoder);

    return hr == S_OK;
}

bool GifDecoder::ReadGidMeta()
{
    return false;
}

int GifDecoder::GetFrameCount() const
{
    UINT uCount = 0;
    HRESULT hr = _decoder->GetFrameCount(&uCount);

    if (SUCCEEDED(hr))
    {
        return uCount;
    }
    else
    {
        return 0;
    }
}

int GifDecoder::GetGifFrame(long index, GifFrame& frame)
{
    IWICBitmapFrameDecode *pWicFrame = NULL;
    IWICFormatConverter *pConverter = NULL;
    IWICMetadataQueryReader *pFrameMetadataQueryReader = NULL;
    PROPVARIANT propValue;
    PropVariantInit(&propValue);
    HRESULT hr = S_OK;

    //IWICImagingFactory* imgFactory = NULL;
    //IWICComponentInfo* comFactory = NULL;IID_IWICComponentInfo

    //hr = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, (LPVOID*)&imgFactory);
    
    hr = _decoder->GetFrame(index, &pWicFrame);

    if (SUCCEEDED(hr))
    {
        //hr = pWicFrame->GetMetadataQueryReader(&pFrameMetadataQueryReader);
        // Format convert to 32bppPBGRA which D2D expects
        hr = _wicFactory->CreateFormatConverter(&pConverter);
    }
    else
    {
        return 0;
    }

    if (SUCCEEDED(hr))
    {
        hr = pConverter->Initialize(
            pWicFrame,
            GUID_WICPixelFormat32bppPBGRA,
            WICBitmapDitherTypeNone,
            NULL,
            0.f,
            WICBitmapPaletteTypeCustom);
    }

    if (SUCCEEDED(hr))
    {
        UINT w = 0;
        UINT h = 0;

        pConverter->GetSize(&w, &h);
        frame.width = w;
        frame.height = h;

        //pConverter->CopyPixels();
        // Get Metadata Query Reader from the frame
        hr = pWicFrame->GetMetadataQueryReader(&pFrameMetadataQueryReader);
    }

    if (NULL == pFrameMetadataQueryReader)
    {
        return 0;
    }

    // Get the Metadata for the current frame
    if (SUCCEEDED(hr))
    {
        hr = pFrameMetadataQueryReader->GetMetadataByName(L"/imgdesc/Left", &propValue);

        if (SUCCEEDED(hr))
        {
            hr = (propValue.vt == VT_UI2 ? S_OK : E_FAIL); 

            if (SUCCEEDED(hr))
            {
                frame.offsetX = static_cast<FLOAT>(propValue.uiVal);
            }

            PropVariantClear(&propValue);
        }
    }

    if (SUCCEEDED(hr))
    {
        hr = pFrameMetadataQueryReader->GetMetadataByName(L"/imgdesc/Top", &propValue);
        if (SUCCEEDED(hr))
        {
            hr = (propValue.vt == VT_UI2 ? S_OK : E_FAIL); 
            if (SUCCEEDED(hr))
            {
                frame.offsetY = static_cast<FLOAT>(propValue.uiVal);
            }
            PropVariantClear(&propValue);
        }
    }

    if (SUCCEEDED(hr))
    {
        hr = pFrameMetadataQueryReader->GetMetadataByName(L"/imgdesc/Width", &propValue);
        if (SUCCEEDED(hr))
        {
            hr = (propValue.vt == VT_UI2 ? S_OK : E_FAIL); 
            if (SUCCEEDED(hr))
            {
                frame.width = static_cast<FLOAT>(propValue.uiVal);
            }
            PropVariantClear(&propValue);
        }
    }

    if (SUCCEEDED(hr))
    {
        hr = pFrameMetadataQueryReader->GetMetadataByName(L"/imgdesc/Height", &propValue);
        if (SUCCEEDED(hr))
        {
            hr = (propValue.vt == VT_UI2 ? S_OK : E_FAIL);
            if (SUCCEEDED(hr))
            {
                frame.height = static_cast<FLOAT>(propValue.uiVal);
            }
            PropVariantClear(&propValue);
        }
    }

    if (SUCCEEDED(hr))
    {
        // Get delay from the optional Graphic Control Extension
        if (SUCCEEDED(pFrameMetadataQueryReader->GetMetadataByName(L"/grctlext/Delay", &propValue)))
        {
            UINT uDelay = 0;
            hr = (propValue.vt == VT_UI2 ? S_OK : E_FAIL); 
            if (SUCCEEDED(hr))
            {
                // Convert the delay retrieved in 10 ms units to a delay in 1 ms units
                hr = UIntMult(propValue.uiVal, 10, &uDelay);
            }
            frame.delay = uDelay;
            PropVariantClear(&propValue);
        }
        else
        {
            // Failed to get delay from graphic control extension. Possibly a
            // single frame image (non-animated gif)
            frame.delay = 0;
        }

        if (SUCCEEDED(hr))
        {
            // Insert an artificial delay to ensure rendering for gif with very small
            // or 0 delay.  This delay number is picked to match with most browsers' 
            // gif display speed.
            //
            // This will defeat the purpose of using zero delay intermediate frames in 
            // order to preserve compatibility. If this is removed, the zero delay 
            // intermediate frames will not be visible.
            if (frame.delay < 90)
            {
                frame.delay = 90;
            }
        }
    }

    if (SUCCEEDED(hr))
    {
        if (SUCCEEDED(pFrameMetadataQueryReader->GetMetadataByName(L"/grctlext/Disposal", &propValue)))
        {
            hr = (propValue.vt == VT_UI1) ? S_OK : E_FAIL;

            if (SUCCEEDED(hr))
            {
                //m_uFrameDisposal = propValue.bVal;
            }
        }
        else
        {
            // Failed to get the disposal method, use default. Possibly a 
            // non-animated gif.
            //m_uFrameDisposal = DM_UNDEFINED;
        }

        UINT cbSize = frame.width * frame.height * 4;
        BYTE* buff = new BYTE[cbSize];
        WICRect wrc;

        wrc.X = 0;
        wrc.Y = 0;
        wrc.Width = frame.width;
        wrc.Height = frame.height;

        //pConverter->CopyPixels(&wrc, 0, cbSize, buff);

        //delete[] buff;
        /*CComPtr<IWICStream> spStream;
        _wicFactory->CreateStream(&spStream);

        suic::String strName;

        strName.Format(_T("d:\\output_%d.png"), index);

        LPCTSTR fileName = (LPCTSTR)strName.c_str();
        spStream->InitializeFromFilename(fileName, GENERIC_WRITE);

        CComPtr<IWICBitmapEncoder> spBitmapEncoder;
        CComPtr<IWICBitmapFrameEncode> spFrameEncode;

        _wicFactory->CreateEncoder(GUID_ContainerFormatPng, NULL, &spBitmapEncoder);

        spBitmapEncoder->Initialize(spStream,WICBitmapEncoderNoCache);
        spBitmapEncoder->CreateNewFrame(&spFrameEncode,NULL);
        spFrameEncode->Initialize(NULL);
        spFrameEncode->SetSize(frame.width, frame.height);

        WICPixelFormatGUID format = GUID_WICPixelFormatDontCare;

        spFrameEncode->SetPixelFormat(&format);
        spFrameEncode->WriteSource(pConverter, NULL);
        spFrameEncode->Commit();
        spBitmapEncoder->Commit();*/
    }

    return hr;
}

