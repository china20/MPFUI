
#include <Framework/Presenter/ItemsPresenter.h>
#include <Framework/Presenter/ScrollContentPresenter.h>
#include <Framework/Controls/ItemsControl.h>
#include <System/Windows/FrameworkElementFactory.h>
#include <System/Tools/StyleHelper.h>

namespace suic
{

ImplementRTTIOfClass(ItemsPresenter, FrameworkElement)

DpProperty* ItemsPresenter::TemplateProperty = NULL;

ItemsPresenter::ItemsPresenter()
    : _owner(NULL)
    , _templateCache(NULL)
{
    WriteDoFlag(ViewFlags::IsClipToBoundsVF, true);
}

ItemsPresenter::~ItemsPresenter()
{
}

void ItemsPresenter::StaticInit()
{
    if (NULL == TemplateProperty)
    {
        TemplateProperty = DpProperty::Register(_T("Template"), RTTIType(), FrameworkTemplate::RTTIType()
            , DpPropMemory::GetPropMeta(NULL, PropMetadataOptions::AffectsNone
            , &ItemsPresenter::OnTemplateChanged));
        ClipToBoundsProperty->OverrideMetadata(RTTIType(), DpPropMemory::GetPropMeta(Boolean::True, PropMetadataOptions::AffectsParentMeasure, &Element::OnClipToBoundsPropChanged));
    }
}

ItemsPresenter* ItemsPresenter::FromPanel(Panel* panel)
{
    // ItemsPresenter的子元素必须从panel继承
    // 如果Panel是其控件模板，则其TemplateParent必定为ItemsPresenter
    if (panel == NULL)
    {
        return NULL;
    }
    else
    {
        return RTTICast<ItemsPresenter>(panel->GetTemplatedParent());
    }
}

void ItemsPresenter::OnTemplateChanged(DpObject* d, DpPropChangedEventArg* e)
{
    ItemsPresenter* fe(DynamicCast<ItemsPresenter>(d));
    FrameworkTemplate* oldTemp = DynamicCast<FrameworkTemplate>(e->GetOldValue());
    FrameworkTemplate* newTemp = DynamicCast<FrameworkTemplate>(e->GetNewValue());

    StyleHelper::UpdateTemplateCache(fe, oldTemp, newTemp, TemplateProperty);
}

ItemContainerGenerator* ItemsPresenter::GetGenerator() const
{
    return (ItemContainerGenerator*)(_generator.GetTarget());
}

ItemsControl* ItemsPresenter::GetOwner() const
{
    return _owner;
}

FrameworkTemplate* ItemsPresenter::GetTemplateCache()
{
    return _templateCache;
}

void ItemsPresenter::SetTemplateCache(FrameworkTemplate* temp)
{
    SETREFOBJ(_templateCache, DynamicCast<ItemsPanelTemplate>(temp));
}

FrameworkTemplate* ItemsPresenter:: GetTemplateInternal()
{
     return GetTemplate();
}

Size ItemsPresenter::OnMeasure(const Size& constraint)
{
    if (GetVisualChildrenCount() > 0)
    {
        Element* child = GetVisualChild(0);

        if (child != NULL)
        {
            MeasureChild(child, constraint);
            return child->GetDesiredSize();
        }
    }

    return Size();
}

void ItemsPresenter::OnArrange(const Size& arrangeSize)
{
    if (GetVisualChildrenCount() > 0)
    {
        Element* child = GetVisualChild(0);

        if (child != NULL)
        {
            Rect rect(0, 0, arrangeSize.cx, arrangeSize.cy);
            child->Arrange(rect);
        }
    }
}

void ItemsPresenter::OnPreApplyTemplate()
{
    FrameworkElement::OnPreApplyTemplate();
    AttachToOwner();
}

void ItemsPresenter::OnTemplateChildChanged()
{
    
}

void ItemsPresenter::OnApplyTemplate()
{
    // 其子元素基类必须为Panel，而且此时没有产生任何可视元素
    // 否则无效
    Panel* visualChild = RTTICast<Panel>(GetVisualChild(0));

    if ((visualChild == NULL) || visualChild->GetVisualChildrenCount() > 0)
    {
        throw InvalidOperationException(_U("ItemsPanelNotSingleNode"), __FILELINE__);
    }
    
    FrameworkElement::OnApplyTemplate();
    visualChild->SetIsItemsHost(true);

    // 可视父元素
    Element* parent = GetUIParent();
    ScrollContentPresenter* scp = NULL;

    while (parent != NULL)
    {
        scp = RTTICast<ScrollContentPresenter>(parent);
        if (NULL != scp)
        {
            break;
        }
        parent = parent->GetUIParent();
    }

    if (scp != NULL)
    {
        scp->LinkScrollInfo();
        scp->InvalidateMeasure();
    }

    if (_generator.IsAlive())
    {
        GetGenerator()->NotifyItemHost(visualChild);
    }
}

void ItemsPresenter::OnTemplateChangedInternal(FrameworkTemplate* oldTemplate, FrameworkTemplate* newTemplate)
{
}

void ItemsPresenter::OnPanelChanged(Object* sender, EventArg* e)
{
    InvalidateMeasure();
}

void ItemsPresenter::UseGenerator(ItemContainerGenerator* generator)
{
    if (generator != _generator.GetTarget())
    {
        if (_generator.IsAlive())
        {
            GetGenerator()->PanelChanged -= EventHandler(this, &ItemsPresenter::OnPanelChanged);
        }
        _generator = generator;
        if (generator != NULL)
        {
            generator->PanelChanged += EventHandler(this, &ItemsPresenter::OnPanelChanged);
        }
    }
}

ItemsPresenter* ItemsPresenter::FromGroupItem(GroupItem* groupItem)
{
    ItemsPresenter* presenter = NULL;
    if (NULL != groupItem)
    {
        Element* parent = groupItem->GetUIParent();
        if (NULL != parent)
        {
            presenter = RTTICast<ItemsPresenter>(parent->GetUIParent());
        }
    }
    return presenter;
}

void ItemsPresenter::AttachToOwner()
{
    // 
    // 其TemplateParent必须为ItemsControl基类型
    //
    ItemContainerGenerator* itemContainerGenerator = NULL;
    ItemsControl* templatedParent = RTTICast<ItemsControl>(GetTemplatedParent());

    if (templatedParent != NULL)
    {
        itemContainerGenerator = templatedParent->GetItemContainerGenerator();
    }
    else
    {
        GroupItem* groupItem = RTTICast<GroupItem>(GetTemplatedParent());
        ItemsPresenter* presenter = FromGroupItem(groupItem);
        if (NULL != presenter)
        {
            templatedParent = presenter->GetOwner();
        }
        itemContainerGenerator = (NULL != groupItem) ? groupItem->GetGenerator() : NULL;
    }

    // 记录owner
    _owner = templatedParent;
    UseGenerator(itemContainerGenerator);

    GroupStyle* groupStyle = NULL;
    if (_generator.IsAlive())
    {
        groupStyle = GetGenerator()->GetGroupStyle();
    }

    if (NULL != groupStyle)
    {
        SetTemplate(groupStyle->GetPanel());
    }
    // 控件模板设置为ItemsControl的ItemsPanel
    else if (_owner != NULL)
    {
        SetTemplate(_owner->GetItemsPanel());
    }
    else
    {
        SetTemplate(NULL);
    }
}

void ItemsPresenter::ClearPanel()
{
    Panel* panel = NULL;
    RTTIOfInfo* underlyingType = NULL;
    
    if (GetVisualChildrenCount() > 0)
    {
        panel = RTTICast<Panel>(GetVisualChild(0));
        if (panel)
        {
            underlyingType = panel->GetRTTIType();
        }
    }

    if (GetTemplate() != NULL)
    {
        if (GetTemplate()->GetVisualTree() != NULL)
        {
            underlyingType = GetTemplate()->GetVisualTree()->GetType();
        }
    }

    if ((panel != NULL) && underlyingType && (underlyingType->InheritFrom(Panel::RTTIType())))
    {
        panel->SetIsItemsHost(false);
    }
}

ItemsPanelTemplate* ItemsPresenter::GetTemplate()
{
    return _templateCache;
}

void ItemsPresenter::SetTemplate(ItemsPanelTemplate* val)
{
    SetValue(TemplateProperty, val);
}

}
