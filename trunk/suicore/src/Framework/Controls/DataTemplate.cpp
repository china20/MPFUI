// ======================================================================
//
// Copyright (c) 2008-2022 汪荣(视点UI), Inc. All rights reserved.
//
// MPF界面库遵循指定开源协议，团体或公司商用请根据协议购买授权，
// 任何个人、团体或公司不能居于此库衍生任何商业性质的库或代码。
//
// ======================================================================

/////////////////////////////////////////////////////////////////////////
// DataTemplate.cpp

#include <Framework/Controls/DataTemplate.h>
#include <Framework/Presenter/ContentPresenter.h>

namespace suic
{

ImplementRTTIOfClass(DataTemplate, FrameworkTemplate)

DataTemplate::DataTemplate()
    : _dataType(NULL)
    , _triggers(NULL)
{
    _dataType = ContentPresenter::RTTIType();
}

DataTemplate::DataTemplate(RTTIOfInfo* targetType) 
{
    _dataType = targetType;
    _triggers = NULL;
}

DataTemplate::~DataTemplate()
{

}

TriggerCollection* DataTemplate::GetTriggers()
{
    if (!_triggers)
    {
        _triggers = new TriggerCollection();
    }
    return _triggers;
}

void DataTemplate::ValidateTemplatedParent(FrameworkElement* templatedParent)
{ 
    if (templatedParent == NULL)
    { 
        throw ArgumentNullException(_T("templatedParent"), __FILELINE__);
    }

    // 数据模板必须是一个ContentPresenter 
    if (!(templatedParent->GetRTTIType()->InheritFrom(ContentPresenter::RTTIType())))
    { 
        throw ArgumentException(templatedParent->GetRTTIType()->typeName, _U("TemplateTargetTypeMismatch"), __FILELINE__); 
    }
}

void DataTemplate::ReadDataTemplateTriggers(const TemplateReadInfo* info, IXamlNode* node)
{

}

}
