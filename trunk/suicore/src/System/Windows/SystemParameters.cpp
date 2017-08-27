
#include <System/Tools/CoreHelper.h>
#include <System/Windows/SystemParameters.h>
#include <System/Interop/System.h>
#include <System/Animation/Matrix.h>

namespace suic
{

String SystemParameters::MessageFontName = L"ËÎÌו";
int SystemParameters::MessageFontSize = 10;

Float SystemParameters::DpiX = 96.0f;
Float SystemParameters::DpiY = 96.0f;

Float SystemParameters::DpiToX = 1.0f;
Float SystemParameters::DpiToY = 1.0f;

Float SystemParameters::DpiFromX = 1.0f;
Float SystemParameters::DpiFromY = 1.0f;

static suic::Matrix _transform;
static bool _needTransform = false;

#define ROUNDF(x) (x)

void InitializeSystemParameters()
{
    HDC hdc = ::GetDC(NULL);
    NONCLIENTMETRICS _ncm;
    OSVERSIONINFOEX _osvi;

    memset(&_osvi, 0, sizeof(OSVERSIONINFOEX));
    _osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

    if (!::GetVersionEx((OSVERSIONINFO*)&_osvi))
    {
        _osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
        ::GetVersionEx((OSVERSIONINFO*)&_osvi);
    }

    _ncm.cbSize = sizeof(NONCLIENTMETRICS);

    /*
     *if (_osvi.dwMajorVersion <= 5)
     *{
     *   _ncm.cbSize = sizeof(NONCLIENTMETRICS) - sizeof(int);
     *}
     */

    int iCapsX = ::GetDeviceCaps(hdc, LOGPIXELSX);
    int iCapsY = ::GetDeviceCaps(hdc, LOGPIXELSY);
    Float fRatio = CoreSystem::Ins()->GetScaleRatio();

    if (CoreSystem::Ins()->GetDpiScale() && iCapsX > 96 && iCapsY > 96)
    {
        _needTransform = true;
        SystemParameters::DpiX = iCapsX;
        SystemParameters::DpiY = iCapsY;
    }

    if (FloatUtil::GreaterThan(fRatio, 1.0f))
    {
        _needTransform = true;
        SystemParameters::DpiX *= fRatio;
        SystemParameters::DpiY *= fRatio;
    }

    SystemParameters::DpiToX = SystemParameters::DpiX / 96.0f;
    SystemParameters::DpiToY = SystemParameters::DpiY / 96.0f;

    SystemParameters::DpiFromX = 96.0f / SystemParameters::DpiX;
    SystemParameters::DpiFromY = 96.0f / SystemParameters::DpiY;

    _transform.SetScale(SystemParameters::DpiToX, SystemParameters::DpiToY);

    if (SystemParametersInfo(SPI_GETNONCLIENTMETRICS, _ncm.cbSize, &_ncm, 0))
    {
        SystemParameters::MessageFontName = _ncm.lfMessageFont.lfFaceName;
        SystemParameters::MessageFontSize = CoreHelper::FontPoundFromPixel(abs(_ncm.lfMessageFont.lfHeight));
    }

    ::ReleaseDC(NULL, hdc);
}

bool SystemParameters::isNeedTransform()
{
    return _needTransform;
}

suic::Matrix& SystemParameters::GetTransform()
{
    return _transform;
}

float SystemParameters::RoundDpiValue(float val, float dpiScale)
{
    if (!FloatUtil::AreClose(dpiScale, 1.0))
    {
        float num = ceilf((float)(val * dpiScale)) / dpiScale;
        if ((!FloatUtil::IsNaN(num) && 
            !FloatUtil::IsInfinity(num)) && 
            !FloatUtil::AreClose(num, FloatUtil::MaxFloat))
        {
            return num;
        }
        return val;
    }
    return ceilf(val);
}

fRect SystemParameters::TransformToDevice(fRect rect)
{
    rect.left = ROUNDF(rect.left * DpiToX);
    rect.right = ROUNDF(rect.right * DpiToX);
    rect.top = ROUNDF(rect.top * DpiToY);
    rect.bottom = ROUNDF(rect.bottom * DpiToY);
    return rect;
}

Rect SystemParameters::TransformToDevice(Rect rect)
{
    rect.left = ROUNDF(rect.left * DpiToX);
    rect.right = ROUNDF(rect.right * DpiToX);
    rect.top = ROUNDF(rect.top * DpiToY);
    rect.bottom = ROUNDF(rect.bottom * DpiToY);
    return rect;
}

Rect SystemParameters::TransformFromDevice(Rect rect)
{
    rect.left = ROUNDF(rect.left * DpiFromX);
    rect.right = ROUNDF(rect.right * DpiFromX);
    rect.top = ROUNDF(rect.top * DpiFromY);
    rect.bottom = ROUNDF(rect.bottom * DpiFromY);
    return rect;
}

Float SystemParameters::TransformXToDevice(Float x)
{
    return ROUNDF(x * DpiToX);
}

Float SystemParameters::TransformYToDevice(Float y)
{
    return ROUNDF(y * DpiToY);
}

Float SystemParameters::TransformXFromDevice(Float x)
{
    return ROUNDF(x * DpiFromX);
}

Float SystemParameters::TransformYFromDevice(Float y)
{
    return ROUNDF(y * DpiFromY);
}

fRect SystemParameters::TransformFromDevice(fRect rect)
{
    rect.left = ROUNDF(rect.left * DpiFromX);
    rect.right = ROUNDF(rect.right * DpiFromX);
    rect.top = ROUNDF(rect.top * DpiFromY);
    rect.bottom = ROUNDF(rect.bottom * DpiFromY);
    return rect;
}

Size SystemParameters::TransformToDevice(Size size)
{
    size.cx = ROUNDF(size.cx * DpiToX);
    size.cy = ROUNDF(size.cy * DpiToY);
    return size;
}

}
