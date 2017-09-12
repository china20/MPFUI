// ======================================================================
//
// Copyright (c) 2008-2022 汪荣(视点UI), Inc. All rights reserved.
//
// MPF界面库遵循指定开源协议，团体或公司商用请根据协议购买授权，
// 任何个人、团体或公司不能居于此库衍生任何商业性质的库或代码。
//
// ======================================================================

/////////////////////////////////////////////////////////////////////////
// ContentPresenter.cpp

#include <System/Tools/Array.h>
#include <System/Tools/Debug.h>
#include <System/Tools/StyleHelper.h>
#include <System/Tools/VisualTreeOp.h>

#include <System/Windows/TemplateBindingExpression.h>

#include <Framework/Controls/Content.h>
#include <Framework/Controls/TextBlock.h>
#include <Framework/Presenter/ContentPresenter.h>

namespace suic
{

ImplementRTTIOfClass(ContentPresenter, suic::FrameworkElement)
ImplementRTTIOfClass(DataTemplateSelector, Object)

DpProperty* ContentPresenter::ContentProperty;
DpProperty* ContentPresenter::ContentSourceProperty;
DpProperty* ContentPresenter::ContentTemplateProperty;
DpProperty* ContentPresenter::ContentTemplateSelectorProperty;

ContentPresenter::ContentPresenter()
{
    _content = NULL;
    _templateCache = NULL;
    WriteDoFlag(ViewFlags::IsClipToBoundsVF, true);
}

ContentPresenter::~ContentPresenter()
{
}

void ContentPresenter::OnContentTemplatePropChanged(DpObject* sender, DpPropChangedEventArg* e)
{
    ContentPresenter* fe(RTTICast<ContentPresenter>(sender));
    DataTemplate* oldTemp = fe->_templateCache;

    fe->WriteInnerFlag(InnerFlags::TemplateIsCurrent, false);
    fe->_templateCache = RTTICast<DataTemplate>(e->GetNewValue());

    StyleHelper::UpdateTemplateCache(fe, oldTemp, fe->_templateCache, ContentTemplateProperty);

    fe->OnContentTemplateChanged(oldTemp, fe->_templateCache);
}

void ContentPresenter::OnContentTemplateSelectorPropChanged(DpObject* sender, DpPropChangedEventArg* e)
{
}

void ContentPresenter::OnContentPropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    ContentPresenter* control = RTTICast<ContentPresenter>(d);
    control->_content = e->GetNewValue();
    control->OnContentChanged(e->GetOldValue(), e->GetNewValue());
}

DataTemplate* DefaultTemplateSelector::SelectTemplate(Object* item, FrameworkElement* container)
{
    if (NULL != item)
    {
        DataTemplate* temp = new DataTemplate();
        FEFactory* root = new FEFactory(TextBlock::RTTIType(), _U("TextBlock"));
        Binding* bind = new Binding();

        root->SetValue(TextBlock::TextProperty, bind);
        root->SetValue(TextBlock::HorizontalAlignmentProperty, HoriAlignBox::LeftBox);
        root->SetValue(TextBlock::VerticalAlignmentProperty, VertAlignBox::CenterBox);

        root->SetValue(TextBlock::ForegroundProperty, new TemplateBindingExtension(TextElement::ForegroundProperty));
        root->SetValue(TextBlock::FontSizeProperty, new TemplateBindingExtension(TextElement::FontSizeProperty));
        root->SetValue(TextBlock::FontWeightProperty, new TemplateBindingExtension(TextElement::FontWeightProperty));

        //root->SetValue(TextBlock::TextProperty, bind);
        //root->SetValue(TextBlock::TextAlignmentProperty, Integer::GetPosInt(TextAlignment::tCenter));
        //root->SetValue(TextBlock::HorizontalAlignmentProperty, HoriAlignBox::LeftBox);
        //root->SetValue(TextBlock::VerticalAlignmentProperty, VertAlignBox::CenterBox);

        temp->SetVisualTree(root);
        //temp->SetTargetType(container->GetRTTIType());

        return temp;
    }
    else
    {
        return NULL;
    }
}

void ContentPresenter::StaticInit()
{
    if (ContentSourceProperty == NULL)
    {
        ContentControl::StaticInit();

        ContentProperty = ContentControl::ContentProperty->AddOwner(RTTIType()
            , DpPropMemory::GetPropMeta(NULL, PropMetadataOptions::AffectsMeasure, &ContentPresenter::OnContentPropChanged));

        ContentSourceProperty = DpProperty::Register(_T("ContentSource"), RTTIType(), OString::RTTIType(), DpPropMemory::GetPropMeta(new OString(_U("Content")), 0));
        ContentTemplateProperty = ContentControl::ContentTemplateProperty->AddOwner(RTTIType()
            , DpPropMemory::GetPropMeta(NULL, PropMetadataOptions::AffectsNone, &ContentPresenter::OnContentTemplatePropChanged));
        ContentTemplateSelectorProperty = ContentControl::ContentTemplateSelectorProperty->AddOwner(RTTIType()
            , DpPropMemory::GetPropMeta(NULL, PropMetadataOptions::AffectsNone, &ContentPresenter::OnContentTemplateSelectorPropChanged));

        ClipToBoundsProperty->OverrideMetadata(RTTIType(), DpPropMemory::GetPropMeta(Boolean::True, PropMetadataOptions::AffectsParentMeasure, &Element::OnClipToBoundsPropChanged));
    }
}

void ContentPresenter::AddChild(Object* obj)
{
    SetContent(obj);
}

void ContentPresenter::AddText(String val)
{
    SetContent(new OString(val));
}

Size ContentPresenter::OnMeasure(const Size& constraitSize)
{
    FrameworkElement* fe = GetTemplateChild();

    if (!fe)
    {
        fe = RTTICast<FrameworkElement>(_content);
        SetTemplateChild(fe);
    }

    if (!fe)
    {
        return Size();
    }
    else
    {
        MeasureChild(fe, constraitSize);
        return fe->GetDesiredSize();
    }
}

void ContentPresenter::OnArrange(const Size& availableSize)
{
    if (GetTemplateChild())
    {
        Rect rect(0, 0, availableSize.cx, availableSize.cy);
        GetTemplateChild()->Arrange(rect);
    }
}

void ContentPresenter::OnPreApplyTemplate()
{
    FrameworkElement::OnPreApplyTemplate();

    if (GetTemplatedParent() == NULL)
    {
        InvalidateProp(ContentProperty);
    }

    if (!ReadInnerFlag(InnerFlags::TemplateIsCurrent))
    {
        EnsureTemplate();
    }
}

void ContentPresenter::OnTemplateChangedInternal(FrameworkTemplate* oldTemplate, FrameworkTemplate* newTemplate)
{

}

void ContentPresenter::OnTemplateChanged(DataTemplate* oldTemplate, DataTemplate* newTemplate)
{

}

void ContentPresenter::OnContentChanged(Object* oldContent, Object* newContent)
{
    FrameworkElement* elem = RTTICast<FrameworkElement>(newContent);

    if (elem != NULL)
    {
        WriteInnerFlag(InnerFlags::ContentIsElement, true);
        SetTemplateChild(elem);
    }
    else if (ReadInnerFlag(InnerFlags::ContentIsItem) || NULL == elem)
    {
        if (GetTemplateChild() == oldContent)
        {
            SetTemplateChild(NULL);
            // 重新产生模版
            FrameworkTemplate* pTemp = GetTemplateCache();
            if (NULL != pTemp && ReadInnerFlag(InnerFlags::IsTemplateGenerated))
            {
                pTemp->InstantiateTree(this);
            }
        }

        SetDataContext(newContent);
        WriteInnerFlag(InnerFlags::ContentIsElement, false);

        if (GetTemplateCache() == NULL)
        {
            WriteInnerFlag(InnerFlags::TemplateIsCurrent, false);
        }
    }
}

void ContentPresenter::OnContentTemplateChanged(DataTemplate* oldTemplate, DataTemplate* newTemplate)
{
}

void ContentPresenter::PrepareContentPresenter(Object* item, DataTemplate* itemTemplate, DataTemplateSelector* itemTemplateSelector)
{
    if (item != this)
    {
        if (!ReadInnerFlag(InnerFlags::ContentIsItem))
        {
            SetContent(item);
            WriteInnerFlag(InnerFlags::ContentIsItem, true);
        }

        if (itemTemplate != NULL)
        {
            SetValue(ContentTemplateProperty, itemTemplate);
        }

        if (itemTemplateSelector != NULL)
        {
            SetValue(ContentTemplateSelectorProperty, itemTemplateSelector);
        }
    }
}

Object* ContentPresenter::GetContent()
{
    return _content;
}

void ContentPresenter::ClearContentPresenter(Object* item)
{
    if ((item != this) && ReadInnerFlag(InnerFlags::ContentIsItem))
    {
        SetContent(NULL);
        WriteInnerFlag(InnerFlags::ContentIsItem, false);
    }
}

FrameworkTemplate* ContentPresenter::GetTemplateInternal()
{
    if (ReadInnerFlag(InnerFlags::ContentIsElement))
    {
        return NULL;
    }
    else
    {
        return GetTemplateCache();
    }
}

DataTemplate* ContentPresenter::ChooseTemplate() 
{
    DataTemplate* temp = NULL; 
    Object* content = GetContent();

    temp = GetContentTemplate(); 

    if (temp == NULL) 
    {
        DataTemplateSelector* contSel = GetContentTemplateSelector();

        if (contSel != NULL) 
        {
            temp = contSel->SelectTemplate(content, this);
        }
    }

    if (NULL == temp && !ReadInnerFlag(InnerFlags::ContentIsElement)) 
    {
        temp = DefaultTemplateSelector::SelectTemplate(content, this);
    }

    return temp;
} 

void ContentPresenter::EnsureTemplate()
{
    if (!ReadInnerFlag(InnerFlags::TemplateIsCurrent))
    {
        WriteInnerFlag(InnerFlags::TemplateIsCurrent, true);

        FrameworkTemplate* oldTemplate = GetTemplateCache();

        // 找到新的内容数据模板（DataTemplate）作为
        // ContentPresenter的控件模板
        DataTemplate* newTemplate = ChooseTemplate();

        if (NULL != newTemplate)
        {
            newTemplate->ref();
        }

        if (oldTemplate != newTemplate)
        { 
            // 设置新的控件模板
            SetContentTemplate(newTemplate); 
        }

        if (NULL != newTemplate)
        {
            newTemplate->unref();
        }
    }
}

}
