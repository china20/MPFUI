// ======================================================================
//
// Copyright (c) 2008-2022 汪荣(视点UI), Inc. All rights reserved.
//
// MPF界面库遵循指定开源协议，团体或公司商用请根据协议购买授权，
// 任何个人、团体或公司不能居于此库衍生任何商业性质的库或代码。
//
// ======================================================================

/////////////////////////////////////////////////////////////////////////
// ActiveX.cpp

#include "stdafx.h"

#include <Extend/ActiveX/ActiveX.h>
#include <System/Windows/CoreTool.h>

#include <Common/SSE.h>

namespace suic
{

ImplementRTTIOfClass(ActiveX, suic::HwndHost)

ActiveX::ActiveX(ActiveXSite* active)
    : _site(active)
    , _oleObj(NULL)
    , _clsid(IID_NULL)
    , _container(NULL)
{
    if (_site)
    {
        _site->AddRef();
    }
    _container.AddRef();
}

ActiveX::ActiveX()
    : _container(NULL)
    , _oleObj(NULL)
    , _clsid(IID_NULL)
    , _site(NULL)
{
    _container.AddRef();
}

ActiveX::~ActiveX()
{
    Clear();
}

void ActiveX::Dispose()
{
    Clear();
}

bool ActiveX::DoVerb(int verb)
{ 
    if (_site)
    {
        _site->DoVerb(verb);
    }

    return false;
}

/////////////////////////////////////////////////////////////////
//
void ActiveX::OnArrange(const suic::Size& arrangeSize)
{
    suic::HwndHost::OnArrange(arrangeSize);
}

void ActiveX::ArrangeCore(const suic::Rect& arrangeRect)
{
    suic::HwndHost::ArrangeCore(arrangeRect);

    // 获取控件相对于整个窗口的偏移
    suic::Point point = PointToScreen(suic::Point());
    suic::Rect rect(point, GetRenderSize());

    if (_site)
    {
        _site->SetControlRect(&rect);
    }
}

void ActiveX::OnLoaded(suic::LoadedEventArg* e)
{
    HwndHost::OnLoaded(e);
}

void ActiveX::OnVisibilityChanged()
{
    if (_site)
    {
        // 获取控件相对于整个窗口的偏移
        suic::Point point = PointToScreen(suic::Point());
        // web窗口所在的区域（相对于整个实际窗口）
        suic::Rect rect(point.x, point.y, GetRenderSize().cx, GetRenderSize().cy);

        if (IsVisible())
        {
            //ShowWindowAsync(HANDLETOHWND(GetHandle()), SW_SHOWNA);
            _site->SetControlRect(&rect);
            DoVerb(OLEIVERB_SHOW);
        }
        else
        {
            // 隐藏时设置其区域为0，不显示实际的web控件
            suic::Rect rc;
            _site->SetControlRect(&rc);
            DoVerb(OLEIVERB_HIDE);
        }
    }
}

void ActiveX::OnInitialized(suic::EventArg* e)
{
    suic::HwndHost::OnInitialized(e);
}

void ActiveX::OnRender(suic::Drawing* drawing)
{
    suic::HwndHost::OnRender(drawing);

    if (_oleObj && _site && _site->IsWindowless())
    {
        suic::Bitmap bmp;
        HBITMAP hbmp;
        BITMAPINFO bmi = {0};

        int w = GetActualWidth();
        int h = GetActualHeight();

        bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmi.bmiHeader.biWidth = w;
        bmi.bmiHeader.biHeight = -h;
        bmi.bmiHeader.biPlanes = 1;
        bmi.bmiHeader.biBitCount = 32;
        bmi.bmiHeader.biCompression = BI_RGB;
        bmi.bmiHeader.biSizeImage = 4 * h * w;
        suic::Byte* rq = NULL;

        hbmp = ::CreateDIBSection(NULL, &bmi, DIB_RGB_COLORS, (LPVOID*)&rq, NULL, 0);
        CSSE::SetColorKey(rq, bmi.bmiHeader.biSizeImage, 0x00FFFFFF);

        HDC hdc = CreateCompatibleDC(NULL);
        HBITMAP oldbmp = (HBITMAP)SelectObject(hdc, hbmp);

        suic::fRect rcdc(0, 0, w, h);
        suic::fRect rcimg(0, 0, w, h);

        bmp.SetConfig(w, h, 32);
        bmp.SetPixels(rq);
        _site->OnRender(hdc);
        drawing->DrawImage(DrawCtx::DefDraw, &bmp, &rcdc, &rcimg);

        SelectObject(hdc, oldbmp);
        DeleteObject(hbmp);
        DeleteDC(hdc);
    }
}

bool ActiveX::IsValid() const
{
    return (_oleObj != NULL);
}

bool ActiveX::LoadActiveX(suic::String strClsid)
{
    CLSID clsid = {0};

    if (strClsid.IndexOf(_T("{")) == 0)
    {
        ::CLSIDFromString(strClsid.c_str(), &clsid);
    }
    else
    {
        ::CLSIDFromProgID(strClsid.c_str(), &clsid);
    }

    return LoadActiveX(clsid);
}

bool ActiveX::LoadActiveX(const CLSID& clsid)
{
    if (IID_NULL == clsid || !_site)
    {
        return false;
    }

    if (IsEqualIID(_clsid, clsid))
    {
        return true;
    }
    else
    {
        _clsid = clsid;

        return Reopen();
    }
}

void ActiveX::Clear()
{
    Close();

    if (_site)
    {
        _site->Release();
        _site = NULL;
    }
}

bool ActiveX::Reopen()
{
    IOleControl* pOleCtrl = NULL;

    // 
    // 创建IID_IOleControl
    //
    HRESULT hr = ::CoCreateInstance(_clsid, NULL, CLSCTX_ALL, IID_IOleControl, (LPVOID*)&pOleCtrl);

    if (FAILED(hr)) 
    {
        return false;
    }

    Close();

    _site->SetContainer(&_container);

    pOleCtrl->QueryInterface(IID_IOleObject, (LPVOID*)&_oleObj);
    pOleCtrl->Release();

    if (!_oleObj) 
    {
        return false;
    }

    _site->SetOwner(this);
    _container.SetHandle(this);
    _container.SetOleObject(_oleObj);

    DWORD dwMisc = 0;
    _oleObj->GetMiscStatus(DVASPECT_CONTENT, &dwMisc);

    IOleClientSite* pOleClntSite = NULL;
    _site->QueryInterface(IID_IOleClientSite, (LPVOID*)&pOleClntSite);

    if ((dwMisc & OLEMISC_SETCLIENTSITEFIRST) != 0) 
    {
        _oleObj->SetClientSite(pOleClntSite);
    }

    IPersistStreamInit* pPersistStreamInit = NULL;
    _oleObj->QueryInterface(IID_IPersistStreamInit, (LPVOID*) &pPersistStreamInit);

    if (pPersistStreamInit != NULL) 
    {
        hr = pPersistStreamInit->InitNew();
        pPersistStreamInit->Release();
    }

    _site->InitWrapper(_oleObj);
    _oleObj->SetHostNames(OLESTR("ActiveX"), NULL);

    IObjectWithSite* pSite = NULL;
    _oleObj->QueryInterface(IID_IObjectWithSite, (LPVOID*) &pSite);

    if (pSite != NULL) 
    {
        pSite->SetSite(static_cast<IOleClientSite*>(_site));
        pSite->Release();
    }

    pOleClntSite->Release();

    return OpenOverride();
}

void ActiveX::Close()
{
    CloseOveride();
}

bool ActiveX::OpenOverride()
{
    return true;
}

void ActiveX::CloseOveride()
{
    if (_site)
    {
        _site->Clear();
    }

    if (_oleObj != NULL) 
    {
        IObjectWithSite* pObjSite = NULL;
        _oleObj->QueryInterface(IID_IObjectWithSite, (LPVOID*)&pObjSite);

        if (pObjSite != NULL) 
        {
            pObjSite->SetSite(NULL);
            pObjSite->Release();
        }

        _oleObj->Close(OLECLOSE_NOSAVE);
        _oleObj->SetClientSite(NULL);
        _oleObj->Release(); 
        _oleObj = NULL;
    }
}

bool ActiveX::QueryControl(const IID iid, LPVOID* ppRet)
{
    if (ppRet == NULL) 
    {
        return false;
    }

    if (_oleObj == NULL) 
    {
        return false;
    }

    return SUCCEEDED(_oleObj->QueryInterface(iid, (LPVOID*)ppRet));
}

void ActiveX::OnUnloaded(suic::LoadedEventArg* e)
{
    Clear();
    suic::HwndHost::OnUnloaded(e);
}

}
