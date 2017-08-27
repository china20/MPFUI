// ======================================================================
//
// Copyright (c) 2008-2022 汪荣(视点UI), Inc. All rights reserved.
//
// MPF界面库遵循指定开源协议，团体或公司商用请根据协议购买授权，
// 任何个人、团体或公司不能居于此库衍生任何商业性质的库或代码。
//
// ======================================================================

/////////////////////////////////////////////////////////////////////////
// SuiTypeface.cpp

#include <System/Tools/Debug.h>
#include <System/Interop/System.h>

#include <System/Graphics/SuiTypeface.h>

namespace suic
{

SuiTypeface::SuiTypeface()
    : _uniqueId(0)
    , _style(Style::fNormal)
    , _isFixedWidth(true)
{
    _data = NULL;
}

bool SuiTypeface::CreateFromName(SuiTypeface* tf, const char familyName[], Style style)
{
    return (tf->CreateFromName(familyName, style));
}

bool SuiTypeface::CreateFromByte(SuiTypeface* tf, const Byte* data, Uint32 size, Style style)
{
    return (tf->CreateFromByte(data, size, style));
}

bool SuiTypeface::CreateFromFile(SuiTypeface* tf, const char filePath[])
{
    return (tf->CreateFromFile(filePath));
}

bool SuiTypeface::CreateFromName(const char familyName[], Style style)
{
    _style = style;
    _data = SkTypeface::CreateFromName(familyName, SkTypeface::Style(style));
    return (GetData() != NULL);
}

bool SuiTypeface::CreateFromByte(const Byte* data, Uint32 size, Style style)
{
    _style = style;
    //_data = SkTypeface::CreateForChars(data, size, SkTypeface::Style(style));
    return (GetData() != NULL);
}

bool SuiTypeface::CreateFromFile(const char filePath[])
{
    _data = SkTypeface::CreateFromFile(filePath);
    return (GetData() != NULL);
}

}
