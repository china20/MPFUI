
// Content.cpp

#include <System/Windows/CoreTool.h>
#include <System/Tools/VisualTreeOp.h>

#include <Framework/Controls/Label.h>
#include <Framework/Controls/Content.h>
#include <Framework/Controls/TextBlock.h>

#include <Framework/Presenter/ContentPresenter.h>

#include <System/Interop/System.h>
#include <System/Interop/InternalOper.h>

#include <typeinfo>

namespace suic
{

DpProperty* ContentControl::HasContentProperty;
DpProperty* ContentControl::ContentProperty;
DpProperty* ContentControl::ContentTemplateProperty;
DpProperty* ContentControl::ContentTemplateSelectorProperty;

ImplementRTTIOfClass(ContentControl, Control)

ContentControl::ContentControl()
{
    SetContentIsLogical(true);
    _content = DpProperty::UnsetValue();
    _contentTemplate = NULL;
    _contentTemplateSelector = NULL;
    WriteDoFlag(ViewFlags::IsClipToBoundsVF, true);
}

ContentControl::~ContentControl()
{
}

void ContentControl::OnContentTemplatePropChanged(DpObject* sender, DpPropChangedEventArg* e)
{
    ContentControl* ctrl = RTTICast<ContentControl>(sender); 
    ctrl->_contentTemplate = RTTICast<DataTemplate>(e->GetNewValue());
    ctrl->OnContentTemplateChanged(RTTICast<DataTemplate>(e->GetOldValue()), ctrl->_contentTemplate);
}

void ContentControl::OnContentTemplateSelectorPropChanged(DpObject* sender, DpPropChangedEventArg* e)
{
    ContentControl* ctrl = RTTICast<ContentControl>(sender); 
    ctrl->_contentTemplateSelector = RTTICast<DataTemplateSelector>(e->GetNewValue());
    ctrl->OnContentTemplateSelectorChanged(RTTICast<DataTemplateSelector>(e->GetOldValue()), ctrl->_contentTemplateSelector);
}

void ContentControl::StaticInit()
{
    if (ContentProperty == NULL)
    {
        Control::StaticInit();

        HasContentProperty = DpProperty::RegisterReadOnly(_T("HasContent"), RTTIType(), Boolean::RTTIType()
            , DpPropMemory::GetPropMeta(Boolean::False, PropMetadataOptions::AffectsNone));
        ContentProperty = DpProperty::Register(_T("Content"), RTTIType(), Object::RTTIType()
            , DpPropMemory::GetPropMeta(NULL, PropMetadataOptions::AffectsMeasure, &ContentControl::OnContentPropChanged));
        ContentTemplateProperty = DpProperty::Register(_T("ContentTemplate"), RTTIType(), DataTemplate::RTTIType()
            , DpPropMemory::GetPropMeta(NULL, PropMetadataOptions::AffectsNone, &ContentControl::OnContentTemplatePropChanged));
        ContentTemplateSelectorProperty = DpProperty::Register(_T("ContentTemplateSelector"), RTTIType(), DataTemplateSelector::RTTIType()
            , DpPropMemory::GetPropMeta(NULL, PropMetadataOptions::AffectsNone, &ContentControl::OnContentTemplateSelectorPropChanged));

        ClipToBoundsProperty->OverrideMetadata(RTTIType(), DpPropMemory::GetPropMeta(Boolean::True, PropMetadataOptions::AffectsParentMeasure, &Element::OnClipToBoundsPropChanged));
    }
}

void ContentControl::OnContentPropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    ContentControl* control = RTTICast<ContentControl>(d);
    control->_content = e->GetNewValue();
    control->SetValue(HasContentProperty, (control->_content != DpProperty::UnsetValue()) ? Boolean::True : Boolean::False);
    control->OnContentChanged(e->GetOldValue(), e->GetNewValue());
}

Object* ContentControl::SetContent(Object* content)
{
    if (content == this)
    {
        Debug::Assert(false, _U("元素非法,不能和本元素相同!"));
    }

    if (content != NULL)
    {
        content->ref();
    }

    if (content != GetContent())
    {
        SetValue(ContentProperty, content);
    }

    if (content != NULL)
    {
        content->unref();
    }

    return content;
}

void ContentControl::SetText(const String& text)
{
    SetContent(new OString(text));
}

Element* ContentControl::GetLogicalChild(int index)
{
    Element* elem = RTTICast<Element>(GetContent());
    return elem;
}

int ContentControl::GetLogicalChildrenCount()
{
    if (ContentIsLogical())
    {
        Element* elem = RTTICast<Element>(GetContent());
        if (elem != NULL)
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }
    else
    {
        return 0;
    }
}

String ContentControl::ToString()
{
    Object* content = GetContent();

    if (!content)
    {
        return _U("");
    }
    else
    {
        return content->ToString();
    }
}

void ContentControl::SetContentIsLogical(bool val)
{
    WriteItemFlags(Control::ItemBoolFlags::ContentIsNotLogical, !val);
}

void ContentControl::SetContentIsItem(bool val)
{
    WriteItemFlags(Control::ItemBoolFlags::ContentEqualItem, val);
}

void ContentControl::ClearContentControl(Object* item)
{
    if ((item != this) && ContentIsItem())
    {
        SetContent(NULL);
    }
}

void ContentControl::PrepareContentControl(Object* item, DataTemplate* itemTemplate, DataTemplateSelector* itemTemplateSelector)
{
    if (item != this)
    {
        SetContentIsLogical(false);

        if (ContentIsItem() || GetContent() == DpProperty::UnsetValue())
        {
            SetContent(item);
            SetContentIsItem(true);
        }
    }
    else
    {
        SetContentIsLogical(true);
        if (GetContent() != DpProperty::UnsetValue() && 
            !ReadInnerFlag(InnerFlags::ContentIsElement))
        {
            SetDataContext(GetContent());
        }
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

void ContentControl::OnContentChanged(Object* oldContent, Object* newContent)
{
    FrameworkElement* feOld(RTTICast<FrameworkElement>(oldContent));
    FrameworkElement* fe(RTTICast<FrameworkElement>(newContent));

    RemoveLogicalChild(oldContent);
    WriteInnerFlag(InnerFlags::ContentIsElement, NULL != fe);

    if (ContentIsLogical())
    {
        if (fe != NULL)
        {
            if (GetTemplatedParent() != NULL)
            {
                DpObject* parent = fe->GetParent();
                FrameworkElement* element = RTTICast<FrameworkElement>(parent);

                if (element != NULL)
                {
                    element->RemoveLogicalChild(newContent);
                }
                else if (parent != NULL)
                {
                    return;
                }
            }

            AddLogicalChild(newContent);
        }
    }
}

void ContentControl::OnContentTemplateChanged(DataTemplate* oldTemplate, DataTemplate* newTemplate)
{
}

void ContentControl::OnContentTemplateSelectorChanged(DataTemplateSelector* oldTemplateSelector, DataTemplateSelector* newTemplateSelector)
{
}

ContentControl* ContentControl::Clone()
{
    ContentControl* pCont(RTTICast<ContentControl>(Control::Clone()));
    if (GetContent())
    {
        Object* newObj(GetContent()->Clone());
        pCont->SetContent(newObj);
    }
    return pCont;
}

void ContentControl::AddChild(Object* child)
{
    SetContent(child);
}

void ContentControl::AddText(const String val)
{
    AddChild(new OString(val));
}

void ContentControl::RemoveChild(Object* child)
{
    SetContent(NULL);
}

}
