// ======================================================================
//
// Copyright (c) 2008-2022 汪荣(视点UI), Inc. All rights reserved.
//
// MPF界面库遵循指定开源协议，团体或公司商用请根据协议购买授权，
// 任何个人、团体或公司不能居于此库衍生任何商业性质的库或代码。
//
// ======================================================================

/////////////////////////////////////////////////////////////////////////
// Label.cpp

#include <Framework/Controls/Label.h>
#include <System/Tools/Math.h>

namespace suic
{

ImplementRTTIOfClass(Label, ContentControl)

Label::Label()
{
}

Label::~Label()
{
}

void Label::StaticInit()
{
    static bool s_init = false;
    if (!s_init)
    {
        s_init = true;
        ContentControl::StaticInit();
        IsEnabledProperty->OverrideMetadata(RTTIType(), suic::DpPropMemory::GetPropMeta(Boolean::True, PropMetadataOptions::AffectsNone, &OnVisualStatePropChanged));
    }
}

Label* Label::Clone()
{
    Label* pObj(DynamicCast<Label>(ContentControl::Clone()));
    return pObj;
}

}
