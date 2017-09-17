
#include <Framework/Controls/ItemsControl.h>
#include <Framework/Controls/Content.h>

#include <Framework/Controls/HierarchicalItem.h>
#include <Framework/Controls/VisualizeStackPanel.h>
#include <Framework/Controls/HeaderedItemsControl.h>
#include <Framework/Controls/HeaderedContentControl.h>

#include <Framework/Presenter/ContentPresenter.h>
#include <Framework/Presenter/ItemsPresenter.h>
#include <Framework/Presenter/ItemContainerGenerator.h>

#include <System/Tools/Debug.h>
#include <System/Tools/EventHelper.h>
#include <System/Tools/VisualTreeOp.h>
#include <System/Tools/LogicalTreeOp.h>

#include <System/Input/Keyboard.h>

namespace suic
{

ImplementRTTIOfClass(ItemsControl, Control)
ImplementRTTIOfClass(ItemEntry, Object)

DpProperty* ItemsControl::HasItemsProperty;
DpProperty* ItemsControl::IsGroupingProperty;
DpProperty* ItemsControl::AlternationIndexProperty;
DpProperty* ItemsControl::ItemContainerStyleProperty;
DpProperty* ItemsControl::GroupStyleSelectorProperty;
DpProperty* ItemsControl::ItemContainerStyleSelectorProperty;
DpProperty* ItemsControl::ItemsPanelProperty;
DpProperty* ItemsControl::ItemsSourceProperty;
DpProperty* ItemsControl::ItemTemplateProperty;
DpProperty* ItemsControl::ItemTemplateSelectorProperty;

//DpProperty* ItemsControl::ItemSourceNameProperty;
//RoutedEvent* ItemsControl::ItemClickEvent;

ItemsChangedEventArg::ItemsChangedEventArg(int action, int prevPos, int pos, int count)
{
    _action = action;
    _newPos = pos;
    _oldPos = prevPos;
    _count = count;
}

ItemsControl::ItemsControl()
    : _itemContainerGenerator(NULL)
    , _items(NULL)
    , _itemsHost(NULL)
    , _scrollHost(NULL)
    , _itemContainerStyle(NULL)
    , _itemTemplate(NULL)
    , _itemTempSelector(NULL)
    , _isVirtualizing(true)
{
    WriteDoFlag(ViewFlags::IsClipToBoundsVF, true);
}

ItemsControl::~ItemsControl()
{
    if (_items)
    {
        _items->unref();
    }

    if (_itemContainerStyle)
    {
        _itemContainerStyle->unref();
    }

    if (_itemTempSelector)
    {
        _itemTempSelector->unref();
    }

    if (_itemContainerGenerator)
    {
        _itemContainerGenerator->unref();
    }
}

void ItemsControl::StaticInit()
{
    if (HasItemsProperty == NULL)
    {
        Control::StaticInit();
        ItemsPresenter::StaticInit();

        ItemsPanelTemplate* temp = new ItemsPanelTemplate();
        FEFactory* root = new FEFactory(VirtualizingStackPanel::RTTIType(), _U("VirtualizingStackPanel"));

        temp->SetVisualTree(root);
        temp->SetTargetType(ItemsPresenter::RTTIType());

        HasItemsProperty = DpProperty::RegisterReadOnly(_U("HasItems"), RTTIType(), Boolean::RTTIType()
            , DpPropMemory::GetPropMeta(Boolean::False, PropMetadataOptions::AffectsNone, &ItemsControl::OnHasItemsPropChanged));

        IsGroupingProperty = DpProperty::RegisterReadOnly(_U("IsGrouping"), RTTIType(), Boolean::RTTIType()
            , DpPropMemory::GetPropMeta(Boolean::False, PropMetadataOptions::AffectsParentMeasure));

        AlternationIndexProperty = DpProperty::Register(_U("AlternationIndex"), RTTIType(), Integer::RTTIType()
            , DpPropMemory::GetPropMeta(Integer::GetZeroInt(), PropMetadataOptions::AffectsNone));
        ItemContainerStyleProperty = DpProperty::Register(_U("ItemContainerStyle"), RTTIType(), Style::RTTIType()
            , DpPropMemory::GetPropMeta(NULL, PropMetadataOptions::AffectsNone, &ItemsControl::OnItemContainerStylePropChanged));
        GroupStyleSelectorProperty = DpProperty::Register(_U("GroupStyleSelector"), RTTIType(), GroupStyleSelector::RTTIType()
            , DpPropMemory::GetPropMeta(NULL, PropMetadataOptions::AffectsNone, &ItemsControl::OnGroupStyleSelectorPropChanged));

        ItemContainerStyleSelectorProperty = DpProperty::Register(_U("ItemContainerStyleSelector"), RTTIType(), StyleSelector::RTTIType()
            , DpPropMemory::GetPropMeta(NULL, PropMetadataOptions::AffectsNone, &ItemsControl::OnItemContainerStyleSelectorPropChanged));
        ItemsPanelProperty = DpProperty::Register(_U("ItemsPanel"), RTTIType(), ItemsPanelTemplate::RTTIType()
            , DpPropMemory::GetPropMeta(temp, PropMetadataOptions::AffectsMeasure, &ItemsControl::OnItemsPanelPropChanged));
        ItemsSourceProperty = DpProperty::Register(_U("ItemsSource"), RTTIType(), ItemCollection::RTTIType()
            , DpPropMemory::GetPropMeta(NULL, PropMetadataOptions::AffectsMeasure, &ItemsControl::OnItemsSourcePropChanged));
        ItemTemplateProperty = DpProperty::Register(_U("ItemTemplate"), RTTIType(), DataTemplate::RTTIType()
            , DpPropMemory::GetPropMeta(NULL, PropMetadataOptions::AffectsMeasure, &ItemsControl::OnItemTemplatePropChanged));
        ItemTemplateSelectorProperty = DpProperty::Register(_U("ItemTemplateSelector")
            , RTTIType(), DataTemplateSelector::RTTIType()
            , DpPropMemory::GetPropMeta(NULL, PropMetadataOptions::AffectsMeasure, &ItemsControl::OnItemTemplateSelectorPropChanged));

        ClipToBoundsProperty->OverrideMetadata(RTTIType(), DpPropMemory::GetPropMeta(Boolean::True, PropMetadataOptions::AffectsParentMeasure, &Element::OnClipToBoundsPropChanged));
        //ItemSourceNameProperty = DpProperty::Register(_U("ItemSourceName"), RTTIType(), OString::RTTIType()
        //    , DpPropMemory::GetPropMeta(new OString(ITEMCOLLNAME), PropMetadataOptions::AffectsNone, &ItemsControl::OnItemSourceNamePropChanged));
        //ItemClickEvent = EventHelper::RegisterRoutedEvent(_U("ItemClick"), RoutingStrategy::Bubble, RoutedEventHandler::RTTIType(), RTTIType());

        EventHelper::RegisterClassHandler(RTTIType(), GotKeyboardFocusEvent, new KeyboardFocusEventHandler(&ItemsControl::OnGotKeyboardFocusThunk), false);
        EventHelper::RegisterClassHandler(RTTIType(), LostKeyboardFocusEvent, new KeyboardFocusEventHandler(&ItemsControl::OnLostKeyboardFocusThunk), false);
    }
}

ItemCollection* ItemsControl::GetItemsSource()
{
    // 先初始化项容器
    if (_items == NULL)
    {
        CreateItemCollectionAndGenerator();
    }
    return _items;
}

void ItemsControl::Reset()
{
    if (_items != NULL && _items->GetCount() > 0)
    {
        SetItemsSource(new ObservableCollection());
    }
}

void ItemsControl::SetItemsSource(ItemCollection* val)
{
    if (NULL != ItemsSourceProperty)
    {
        if (val == NULL)
        {
            val = new ObservableCollection();
        }

        if (NULL != val)
        {
            val->ref();
            SetValue(ItemsSourceProperty, val);
            val->unref();
        }
    }
}

void ItemsControl::SetItemsSourceInternal(ItemCollection* newColl)
{
    if (newColl == NULL)
    {
        //throw InvalidValueException(_U("ItemCollectoin is not null"), __FILELINE__);
    }

    if (_items != newColl)
    {
        ObjectPtr tmp = _items;
        ItemCollection* oldColl = _items;

        if (_itemsHost != NULL)
        {
            _itemsHost->DisconnectFromGenerator();
        }

        if (NULL != _itemContainerGenerator)
        {
            _itemContainerGenerator->unref();
        }

        SETREFOBJ(_items, newColl);

        _itemContainerGenerator = new ItemContainerGenerator(this);
        _itemContainerGenerator->ref();

        if (IsInitPending())
        {
            _items->BeginInit();
        }
        else if (Control::IsInitialized())
        {
            _items->BeginInit();
            _items->EndInit();
        }

        if (NULL != oldColl)
        {
            oldColl->CollectionChanged.Clear();
        }

        if (newColl != NULL)
        {
            SetValue(HasItemsProperty, BOOLTOBOOLEAN(newColl->GetCount() > 0));
            newColl->SetLogicalParent(this);
            newColl->CollectionChanged += NotifyCollChangedEventHandler(this, &ItemsControl::OnItemCollectionChanged);
        }

        OnItemsSourceChanged(oldColl, newColl);
    }
}

DataTemplate* ItemsControl::GetItemTemplate()
{
    return _itemTemplate;
}

void ItemsControl::SetItemTemplate(DataTemplate* val)
{
    SetValue(ItemTemplateProperty, val);
}

DataTemplateSelector* ItemsControl::GetItemTemplateSelector()
{
    return _itemTempSelector;
}

void ItemsControl::SetItemTemplateSelector(DataTemplateSelector* val)
{
    SetValue(ItemTemplateSelectorProperty, val);
}

StyleSelector* ItemsControl::GetItemContainerStyleSelector()
{
    return RTTICast<StyleSelector>(GetValue(ItemContainerStyleSelectorProperty));
}

void ItemsControl::SetItemContainerStyleSelector(StyleSelector* style)
{
    SetValue(ItemContainerStyleSelectorProperty, style);
}

void ItemsControl::SetFocusedItem(ItemEntry* item)
{
    _focusedItem = item;
}

bool ItemsControl::IsGrouping()
{
    return GetValue(IsGroupingProperty)->ToBool();
}

void ItemsControl::SetIsGrouping(bool val)
{
    SetValue(IsGroupingProperty, BOOLTOBOOLEAN(val));
}

GroupStyleSelector* ItemsControl::GetGroupStyleSelector()
{
    return RTTICast<GroupStyleSelector>(GetValue(GroupStyleSelectorProperty));
}

void ItemsControl::SetGroupStyleSelector(GroupStyleSelector* val)
{
    SetValue(GroupStyleSelectorProperty, val);
}

GroupStyle* ItemsControl::GetGroupStyle(ViewGroup* group, int level)
{
    GroupStyle* style = NULL;
    GroupStyleSelector* groupSelector = GetGroupStyleSelector();
    if (groupSelector != NULL)
    {
        style = groupSelector->SelectStyle(group, level);
    }
    if (NULL == style)
    {
        if (level >= _groupStyles.GetCount())
        {
            level = _groupStyles.GetCount() - 1;
        }
        if (level >= 0)
        {
            style = RTTICast<GroupStyle>(_groupStyles.GetItem(level));
        }
    }
    return style;
}

ItemsControl* ItemsControl::GetItemsOwner(DpObject* element)
{
    ItemsControl* control = NULL;
    Panel* panel = RTTICast<Panel>(element);

    if ((panel == NULL) || !panel->IsItemsHost())
    {
        return control;
    }

    ItemsPresenter* presenter = ItemsPresenter::FromPanel(panel);

    if (presenter != NULL)
    {
        return presenter->GetOwner();
    }
    else
    {
        return RTTICast<ItemsControl>(panel->GetTemplatedParent());
    }
}

void ItemsControl::ClearBeforeLinkContainer(Element* container)
{
    ItemsControl* itemCtrl = RTTICast<ItemsControl>(container);
    if (NULL != itemCtrl)
    {
        itemCtrl->Reset();
    }
}

ItemsControl* ItemsControl::ItemsControlFromItemContainer(DpObject* container)
{
    Element* current = RTTICast<Element>(container);
    if (current == NULL)
    {
        return NULL;
    }

    // 逻辑父元素
    ItemsControl* parent = RTTICast<ItemsControl>(LogicalTreeOp::GetParent(current));
    if (parent != NULL)
    {
        if (parent->IsItemItsOwnContainer(current))
        {
            return parent;
        }
        return NULL;
    }

    // 可视父元素
    current = RTTICast<Element>(current->GetUIParent());
    return GetItemsOwner(current);
}

void ItemsControl::OnItemContainerStylePropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    ItemsControl* itemsControl = RTTICast<ItemsControl>(d);
    if (itemsControl)
    {
        itemsControl->_itemContainerStyle = RTTICast<Style>(e->GetNewValue());
        itemsControl->OnItemContainerStyleChanged(RTTICast<Style>(e->GetOldValue()), itemsControl->_itemContainerStyle);
    }
}

void ItemsControl::OnItemContainerStyleSelectorPropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    ItemsControl* itemsControl = RTTICast<ItemsControl>(d);
    if (itemsControl)
    {
        itemsControl->OnItemContainerStyleSelectorChanged(RTTICast<StyleSelector>(e->GetOldValue()), RTTICast<StyleSelector>(e->GetNewValue()));
    }
}

void ItemsControl::OnItemsPanelPropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    ItemsControl* itemsControl = RTTICast<ItemsControl>(d);
    ItemsPanelTemplate* oldPanel = RTTICast<ItemsPanelTemplate>(e->GetOldValue());
    ItemsPanelTemplate* newPanel = RTTICast<ItemsPanelTemplate>(e->GetNewValue());

    itemsControl->OnItemsPanelChanged(oldPanel, newPanel);
}

void ItemsControl::OnItemsSourcePropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    ItemsControl* itemsControl = RTTICast<ItemsControl>(d);
    ItemCollection* newTemp = RTTICast<ItemCollection>(e->GetNewValue());
    itemsControl->SetItemsSourceInternal(newTemp);
}

void ItemsControl::OnHasItemsPropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    ItemsControl* itemsControl = RTTICast<ItemsControl>(d);
    if (itemsControl)
    {
        itemsControl->WriteItemFlags(HasItemsFlag, e->GetNewValue()->ToBool());
    }
}

void ItemsControl::OnItemTemplatePropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    ItemsControl* itemsControl = RTTICast<ItemsControl>(d);
    DataTemplate* oldTemp = RTTICast<DataTemplate>(e->GetOldValue());
    DataTemplate* newTemp = RTTICast<DataTemplate>(e->GetNewValue());

    itemsControl->_itemTemplate = newTemp;
    itemsControl->OnItemTemplateChanged(oldTemp, newTemp);
}

void ItemsControl::OnGroupStyleSelectorPropChanged(DpObject* d, DpPropChangedEventArg* e)
{

}

void ItemsControl::OnItemTemplateSelectorPropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    ItemsControl* itemsControl = RTTICast<ItemsControl>(d);
    DataTemplateSelector* oldTemp = RTTICast<DataTemplateSelector>(e->GetOldValue());
    DataTemplateSelector* newTemp = RTTICast<DataTemplateSelector>(e->GetNewValue());

    if (itemsControl->_itemTempSelector != newTemp)
    {
        DataTemplateSelector* oldSel = itemsControl->_itemTempSelector;

        if (oldSel)
        {
            oldSel->ref();
        }

        SETREFOBJ(itemsControl->_itemTempSelector, newTemp);
        itemsControl->OnItemTemplateSelectorChanged(oldSel, newTemp);

        if (oldSel)
        {
            oldSel->unref();
        }
    }
}

//--------------------------------------------------------------------------------
//

void ItemsControl::OnItemCollectionChanged(Object* sender, NotifyCollChangedEventArg* e)
{
    bool bEmpty = (_items != NULL) && !_items->IsEmpty();
    ItemEntry* focusObj = GetFocusedItem();

    if (HasItems() != bEmpty)
    {
        SetValue(HasItemsProperty, BOOLTOBOOLEAN(bEmpty));
    }

    // 
    // 检查移除的是否为当前焦点子项，如果是需要重置
    //
    if ((e->GetAction() == NotifyCollectionChangedAction::Remove && focusObj == e->GetOldItem()) || 
        e->GetAction() == NotifyCollectionChangedAction::Reset)
    {
        _focusedItem = WeakRef();
    }

    UpdateLayout();
    OnItemsChanged(e);
}

void ItemsControl::OnItemContainerStyleChanged(Style* oldItemContainerStyle, Style* newItemContainerStyle)
{
    if (_itemContainerGenerator != NULL)
    {
        _itemContainerGenerator->Refresh();
    }
}

void ItemsControl::OnItemContainerStyleSelectorChanged(StyleSelector* oldItemContainerStyleSelector, StyleSelector* newItemContainerStyleSelector)
{
}

void ItemsControl::OnItemsChanged(NotifyCollChangedEventArg* e)
{
}

void ItemsControl::OnItemsPanelChanged(ItemsPanelTemplate* oldItemsPanel, ItemsPanelTemplate* newItemsPanel)
{
    if (_itemContainerGenerator != NULL)
    {
        _itemContainerGenerator->OnPanelChanged();
    }
}

void ItemsControl::OnItemsSourceChanged(ItemCollection* oldValue, ItemCollection* newValue)
{
}

void ItemsControl::OnItemTemplateChanged(DataTemplate* oldItemTemplate, DataTemplate* newItemTemplate)
{
    if (_itemContainerGenerator != NULL)
    {
        _itemContainerGenerator->Refresh();
    }
}

void ItemsControl::OnItemTemplateSelectorChanged(DataTemplateSelector* oldItemTemplateSelector, DataTemplateSelector* newItemTemplateSelector)
{
}

//--------------------------------------------------------------------------------
//

void ItemsControl::OnGotKeyboardFocusThunk(Element* sender, KeyboardFocusEventArg* e)
{
    ItemsControl* control = RTTICast<ItemsControl>(sender);
    Element* oriSource = RTTICast<Element>(e->GetOriginalSource());

    if (!control->ReadItemFlags(Control::CantCachedFocusedItem) && 
        oriSource != NULL && 
        oriSource != control)
    {
        ItemEntry* obj = oriSource->GetItemEntry();
        if (obj != NULL && obj->GetItem() != DpProperty::UnsetValue())
        {
            control->_focusedItem = obj;

            // 
            //  滚动到此元素
            // 
            // control->ScrollIntoView(obj->GetItem(), false);
        }
    }
}

void ItemsControl::OnLostKeyboardFocusThunk(Element* sender, KeyboardFocusEventArg* e)
{
    ItemsControl* control = RTTICast<ItemsControl>(sender);
    Element* originalSource = RTTICast<Element>(e->GetOriginalSource());

    if (control != NULL && originalSource != control)
    {
        control->_focusedItem = NULL;
    }
}

void ItemsControl::AddText(String text)
{
    if (GetItemsSource())
    {
        GetItemsSource()->AddItem(new OString(text));
    }
}

void ItemsControl::RemoveChild(Object* child)
{
    GetItemsSource()->RemoveItem(child);
}

void ItemsControl::AddChild(Object* val)
{
    ItemCollection* itemColl = GetItemsSource();
    val->ref();
    if (itemColl)
    {
        itemColl->AddItem(val);
        AddLogicalChild(val);
    }
    val->unref();
}

void ItemsControl::PrepareItemsControl(ItemEntry* item, ItemsControl* parentItemsControl)
{
    if (item->GetItem() != this)
    {
        DataTemplate* itemTemplate = parentItemsControl->GetItemTemplate();
        DataTemplateSelector* itemTemplateSelector = parentItemsControl->GetItemTemplateSelector();
        Style* itemContainerStyle = parentItemsControl->GetItemContainerStyle();
        StyleSelector* itemContainerStyleSelector = parentItemsControl->GetItemContainerStyleSelector();

        // int alternationCount = parentItemsControl->GetAlternationCount();

        if (itemTemplate != NULL)
        {
            SetValue(ItemTemplateProperty, itemTemplate);
        }

        if (itemTemplateSelector != NULL)
        {
            SetItemTemplateSelector(itemTemplateSelector);
        }

        if (itemContainerStyle != NULL)
        {
            SetValue(ItemContainerStyleProperty, itemContainerStyle);
        }

        if (itemContainerStyleSelector != NULL)
        {
            SetValue(ItemContainerStyleSelectorProperty, itemContainerStyleSelector);
        }
    }
}

ScrollViewer* ItemsControl::GetScrollHost()
{
    if (NULL == _scrollHost && GetItemsHost() != NULL)
    {
        Element* elem = GetItemsHost()->GetUIParent();
        while (elem != NULL)
        {
            ScrollViewer* sv = RTTICast<ScrollViewer>(elem);
            if (NULL != sv)
            {
                _scrollHost = sv;
                break;
            }
            elem = elem->GetUIParent();
        }
    }
    return _scrollHost;
}

Size ItemsControl::EstimatedContainerSize(bool isHorizontal)
{
    return Size(DEFAULTITEMSIZE, DEFAULTITEMSIZE);
}

void ItemsControl::ReadItemSize(ItemEntry* item, bool bHori, int index, Size& size)
{
    if (item->IsCached())
    {
        size = item->GetSize();
    }
    else
    {
        size = EstimatedContainerSize(bHori);
    }
}

void ItemsControl::StoreItemSize(ItemEntry* item, int index, Size size)
{
    item->SetSize(size);
}

void ItemsControl::RemoveItemContainerStyle(DpObject* container)
{
}

void ItemsControl::OnTemplateChangedInternal(FrameworkTemplate* oldTemplate, FrameworkTemplate* newTemplate)
{
    _itemsHost = NULL;
    _scrollHost = NULL;

    Control::OnTemplateChangedInternal(oldTemplate, newTemplate);
}

void ItemsControl::PrepareContainerForItemOverride(DpObject* element, ItemEntry* item)
{
    HeaderedContentControl* headCtrl = RTTICast<HeaderedContentControl>(element);
    if (headCtrl != NULL)
    {
        headCtrl->PrepareHeaderedContentControl(item->GetItem(), GetItemTemplate(), GetItemTemplateSelector());
    }
    else
    {
        ContentControl* contCtrl = RTTICast<ContentControl>(element);
        if (contCtrl != NULL)
        {
            contCtrl->PrepareContentControl(item->GetItem(), GetItemTemplate(), GetItemTemplateSelector());
        }
        else
        {
            ContentPresenter* presenter = RTTICast<ContentPresenter>(element);
            if (presenter != NULL)
            {
                presenter->PrepareContentPresenter(item->GetItem(), GetItemTemplate(), GetItemTemplateSelector());
            }
            else
            {
                HeaderedItemsControl* headItemCtrl = RTTICast<HeaderedItemsControl>(element);
                if (headItemCtrl != NULL)
                {
                    headItemCtrl->PrepareHeaderedItemsControl(item, this);
                }
                else
                {
                    ItemsControl* itemsCtrl(RTTICast<ItemsControl>(element));
                    if ((itemsCtrl != NULL) && (itemsCtrl != this))
                    {
                        itemsCtrl->PrepareItemsControl(item, this);
                    }
                }
            }
        }
    }
}

void ItemsControl::ClearContainerForItemOverride(DpObject* element, ItemEntry* item)
{
    Object* realItem = item->GetItem();
    HeaderedContentControl* headCtrl(RTTICast<HeaderedContentControl>(element));

    if (headCtrl != NULL)
    {
        headCtrl->ClearHeaderedContentControl(realItem);
    }
    else
    {
        ContentControl* contCtrl = RTTICast<ContentControl>(element);
        if (contCtrl != NULL)
        {
            contCtrl->ClearContentControl(realItem);
        }
        else
        {
            ContentPresenter* presenter = RTTICast<ContentPresenter>(element);
            if (presenter != NULL)
            {
                presenter->ClearContentPresenter(realItem);
            }
            else
            {
                HeaderedItemsControl* headItemCtrl = RTTICast<HeaderedItemsControl>(element);
                if (headItemCtrl != NULL)
                {
                    headItemCtrl->ClearHeaderedItemsControl(item);
                }
                else
                {
                    ItemsControl* itemsCtrl(RTTICast<ItemsControl>(element));
                    if ((itemsCtrl != NULL) && (itemsCtrl != this))
                    {
                        itemsCtrl->ClearItemsControl(item);
                    }
                }
            }
        }
    }
}

bool ItemsControl::IsHostForItemContainer(DpObject* container)
{
    ItemsControl* control = ItemsControlFromItemContainer(container);

    if (control != NULL)
    {
        return (control == this);
    }

    if (LogicalTreeOp::GetParent(container) != NULL)
    {
        return false;
    }

    return ((IsItemItsOwnContainer(container) && HasItems()) && GetItemsSource()->Contains(container));
}

Element* ItemsControl::GetContainerForItem(Object* item)
{
    Element* container = NULL;

    if (IsItemItsOwnContainer(item))
    {
        container = RTTICast<Element>(item);
    }
    else
    {
        container = GetContainerForItemOverride();
    }

    if (container != NULL)
    {
        // 引用加1，外部必须调用__release()
        container->ref();
    }

    return container;
}

void ItemsControl::ClearContainerForItem(DpObject* container, ItemEntry* item)
{
    ClearContainerForItemOverride(container, item);
}

Element* ItemsControl::GetContainerForItemOverride()
{
    return new ContentPresenter();
}

void ItemsControl::OnPrepareContainer()
{

}

bool ItemsControl::IsItemItsOwnContainer(Object* item)
{
    return (item->GetRTTIType() == ContentPresenter::RTTIType());
}

void ItemsControl::ApplyItemContainerStyle(DpObject* container, ItemEntry* item)
{
    FrameworkElement* fe = CASTTOFE(container);
    if (NULL != fe && NULL != item)
    {
        if (fe->GetIsStyleSetFromGenerator() || 
            fe->ReadLocalValue(StyleProperty, false) == DpProperty::UnsetValue())
        {
            Style* itemContainerStyle = GetItemContainerStyle();
            if (itemContainerStyle == NULL)
            {
                StyleSelector* styleSelctor = GetItemContainerStyleSelector();
                if (NULL != styleSelctor)
                {
                    itemContainerStyle = styleSelctor->SelectStyle(item->GetItem(), fe);
                }
            }
            if (itemContainerStyle != NULL)
            {
                if (!fe->GetRTTIType()->InheritFrom(itemContainerStyle->GetTargetType()))
                {
                    throw InvalidOperationException(_U("StyleForWrongType"), __FILELINE__);
                }
                fe->SetStyle(itemContainerStyle);
                fe->SetIsStyleSetFromGenerator(true);
            }
            else if (fe->GetIsStyleSetFromGenerator())
            {
                fe->ClearValue(FrameworkElement::StyleProperty);
                fe->InvalidateStyle();
                fe->SetIsStyleSetFromGenerator(false);
            }
        }
    }
}

bool ItemsControl::ShouldApplyItemContainerStyle(DpObject* container, ItemEntry* item)
{
    return true;
}

void ItemsControl::PrepareItemContainer(DpObject* container, ItemEntry* item)
{
    GroupItem* groupItem = RTTICast<GroupItem>(container);
    if (NULL != groupItem)
    {
        groupItem->PrepareItemContainer(item->GetItem());
    }
    // 应用容器样式
    else 
    {
        if (ShouldApplyItemContainerStyle(container, item))
        {
            ApplyItemContainerStyle(container, item);
        }

        PrepareContainerForItemOverride(container, item);
    }
}

void ItemsControl::SetItemValuesOnContainer(DpObject* container, ItemEntry* item, int* dpIndices)
{
}

void ItemsControl::BeginInit()
{
    Control::BeginInit();
}

void ItemsControl::EndInit()
{
    Control::EndInit();
}

bool ItemsControl::IsInitPending()
{
    return ReadFlag(CoreFlags::IsInitPending);
}

bool ItemsControl::IsVirtualizing()
{
    return _isVirtualizing;
}

bool ItemsControl::IsLogicalVertical()
{
    if (GetItemsHost() == NULL)
    {
        return false;
    }
    else
    {
        return GetItemsHost()->GetLogicalOrientation() == Orientation::Vertical;
    }
}

bool ItemsControl::IsLogicalHorizontal()
{
    if (GetItemsHost() == NULL)
    {
        return false;
    }
    else
    {
        return GetItemsHost()->GetLogicalOrientation() == Orientation::Horizontal;
    }
}

ItemEntry* ItemsControl::ItemEntryFromItem(Object* item)
{
    ItemEntry* itemb = NULL;
    ItemCollection* items = GetItemsSource();

    if (NULL == item)
    {
        return NULL;
    }

    if (item->GetRTTIType() == ItemEntry::RTTIType())
    {
        return (ItemEntry*)item;
    }
    else
    {
        return RecursiveFindItemEntry(items, item);
    }
}

ItemEntry* ItemsControl::RecursiveFindItemEntry(ItemCollection* items, Object* item)
{
    ItemEntry* itemb = NULL;

    for (int i = 0; i < items->GetCount(); ++i)
    {
        ItemEntry* pItem = items->GetItemEntry(i);
        if (pItem->GetItem() == item)
        {
            itemb = pItem;
            break;
        }
        else
        {
            HierarchicalItem* hiItem = HiItemFromEntry(pItem);
            if (NULL != hiItem)
            {
                itemb = RecursiveFindItemEntry(hiItem->GetChildren(), item);
                if (NULL != itemb)
                {
                    return itemb;
                }
            }
        }
    }
    return itemb;
}

int ItemsControl::FindFromText(const String& name)
{
    ItemCollection* items = GetItemsSource();
    int index = -1;

    for (int i = 0; i < items->GetCount(); ++i)
    {
        if (items->GetItem(i)->ToString().Equals(name))
        {
            index = i;
            break;
        }
    }
    return index;
}

int ItemsControl::FindFromItem(const Object* item)
{
    ItemCollection* items = GetItemsSource();
    int index = -1;

    for (int i = 0; i < items->GetCount(); ++i)
    {
        if (items->GetItem(i) == item)
        {
            index = i;
            break;
        }
    }
    return index;
}

Element* ItemsControl::GetContainerFromItem(Object* item)
{
    return GetItemContainerGenerator()->ContainerFromItem(item);
}

Element* ItemsControl::HitTestItem(Point point)
{
    Element* hitElem = NULL;
    Panel* pItemHost = GetItemsHost();
    Rect rect = Rect(PointToScreen(Point()), GetRenderSize());

    if (NULL != pItemHost && rect.PointIn(point))
    {
        for (int i = 0; i < pItemHost->GetVisibleChildrenCount(); ++i)
        {
            Element* elem = pItemHost->GetVisibleChild(i);

            if (elem != NULL)
            {
                ItemsControl* pItem = RTTICast<ItemsControl>(elem);
                if (NULL != pItem)
                {
                    hitElem = pItem->HitTestItem(point);

                    if (NULL != hitElem)
                    {
                        break;
                    }
                }
                else
                {
                    rect = Rect(PointToScreen(Point()), GetRenderSize());
                    if (rect.PointIn(point))
                    {
                        hitElem = elem;
                        break;
                    }
                }
            }
        }
    }

    return hitElem;
}

void ItemsControl::ClearItemsControl(ItemEntry* item)
{

}

void ItemsControl::DoAutoScroll()
{

}

void ItemsControl::DoAutoScroll(Object* startingItem)
{

}

int ItemsControl::ComputeVirtualOffsetFromItem(Object* item, bool bHori, eItemDirection id, int& offset, int& itemSize)
{
    Size size;
    int index = -1;
    ItemEntry* itemb = NULL;

    for (int i = 0; i < GetCount(); ++i)
    {
        itemb = GetItemsSource()->GetItemEntry(i);
        ReadItemSize(itemb, bHori, i, size);

        if (item == itemb->GetItem())
        {
            index = i;
            itemSize = (bHori ? size.cx : size.cy);

            break;
        }

        offset += (bHori ? size.cx : size.cy);
    }

    switch (id)
    {
    case eItemDirection::idPrev:
        if (index > 0)
        {
            index -= 1;
            itemb = GetItemsSource()->GetItemEntry(index);
            ReadItemSize(itemb, bHori, index, size);
            offset -= (bHori ? size.cx : size.cy);
            itemSize = (bHori ? size.cx : size.cy);
        }
        break;

    case eItemDirection::idNext:
        if (index < GetCount() - 1)
        {
            index += 1;
            offset += itemSize;

            itemb = GetItemsSource()->GetItemEntry(index);
            ReadItemSize(itemb, bHori, index, size);
            itemSize = (bHori ? size.cx : size.cy);
        }
        break;

    default:
        ;
    }

    return index;
}

int ItemsControl::ComputeOffsetFromItem(Object* item, eItemDirection id, int& offset, int& itemSize)
{
    bool bHori = false;
    int index = -1;

    suic::Panel* pPanel = GetItemsHost();

    offset = 0;
    itemSize = 0;

    if (IsLogicalHorizontal())
    {
        bHori = true;
    }

    if (pPanel != NULL)
    {
        index = pPanel->ComputeOffsetFromItem(item, id, offset, itemSize);
    }

    if (index == -1)
    {
        if (IsVirtualizing())
        {
            index = ComputeVirtualOffsetFromItem(item, bHori, id, offset, itemSize);
        }
        else if (pPanel != NULL)
        {
            ElementColl* pColl = pPanel->GetChildren();

            index = FindFromItem(item);

            switch (id)
            {
            case eItemDirection::idPrev:
                if (index > 0)
                {
                    --index;
                }
                break;

            case eItemDirection::idNext:
                if (index >= 0 && index < pColl->GetCount() - 1)
                {
                    ++index;
                }

                break;

            default:
                ;
            }

            if (index >= 0 && index < pColl->GetCount())
            {
                Element* pItemElem = pColl->GetAt(index);

                Size size = pItemElem->GetRenderSize();
                itemSize = (bHori ? size.cx : size.cy);
                Point ptOffset = pItemElem->GetCanvasOffset();

                ptOffset -= pPanel->GetCanvasOffset();
                offset = (bHori ? ptOffset.x : ptOffset.y);
            }
            else
            {
                index = -1;
            }
        }
    }

    return index;
}

int ItemsControl::ComputeOffsetFromIndex(int index, int& offset, int& itemSize)
{
    int iOri = -1;
    bool bHori = false;

    offset = 0;
    itemSize = 0;

    if (IsLogicalHorizontal())
    {
        iOri = 0;
        bHori = true;
    }
    else if (IsLogicalVertical())
    {
        iOri = 1;
    }

    if (-1 == index || index > GetItemsSource()->GetCount() - 1)
    {
        index = GetItemsSource()->GetCount() - 1;
    }

    for (int i = 0; i <= index; ++i)
    {
        Size size;
        ItemEntry* item = GetItemsSource()->GetItemEntry(i);
        
        ReadItemSize(item, bHori, i, size);

        if (i == index)
        {
            itemSize = iOri == 0 ? size.cx : size.cy;
            break;
        }

        if (0 == iOri)
        {
            offset += size.cx;
        }
        else if (1 == iOri)
        {
            offset += size.cy;
        }
    }

    return index;
}

Object* ItemsControl::FindFocusable(int iStart, int dir, int& outIndex)
{
    if (HasItems())
    {
        int count = GetItemsSource()->GetCount();

        while ((iStart >= 0) && (iStart < count))
        {
            FrameworkElement* elem = NULL;

            if ((elem == NULL) || Keyboard::IsFocusable(elem))
            {
                outIndex = iStart;
                return GetItemsSource()->GetItem(iStart);
            }
            iStart += dir;
        }
    }

    outIndex = -1;

    return NULL;
}

bool ItemsControl::IsOnCurrentPage(Element* elem, bool fullyVisible)
{
    return IsOnCurrentPage(elem, AxisDirection::xyAxis, fullyVisible);
}

bool ItemsControl::IsOnCurrentPage(Element* elem, AxisDirection axis, bool fullyVisible)
{
    if (NULL == elem)
    {
        return false;
    }

    Element* scrollHost = GetItemsHost();

    if (NULL != scrollHost)
    {
        scrollHost = scrollHost->GetUIParent();
    }

    if (scrollHost == NULL)
    {
        scrollHost = GetScrollHost();
    }

    if (scrollHost != NULL)
    {
        if ((elem == NULL) || !scrollHost->IsAncestorOf(elem))
        {
            return false;
        }

        Rect rectHost(Point(), scrollHost->GetRenderSize());
        Rect rectElem(elem->TransformToAncestor(scrollHost), elem->GetRenderSize());

        if (fullyVisible)
        {
            // 水平
            if (AxisDirection::xAxis == axis)
            {
                rectElem.top = rectHost.top;
                rectElem.bottom = rectHost.bottom;
            }
            // 垂直
            else if (AxisDirection::yAxis == axis)
            {
                rectElem.left = rectHost.left;
                rectElem.right = rectHost.right;
            }
            
            return rectHost.Contains(rectElem);
        }
        else
        {
            Rect rc = rectHost.IntersectRet(&rectElem);
            return !rc.IsZero();
        }
    }
    else
    {
        return false;
    }
}


int ItemsControl::ScrollIntoView(Object* item, bool atTopOfViewport)
{
    int index = -1;
    int offset = 0;
    int itemSize = 0;

    Panel* itemHost = GetItemsHost();
    Element* elem = NULL;
    ScrollViewer* scrollView = GetScrollHost();

    if (NULL != itemHost && NULL != scrollView)
    {
        int iViewSize = scrollView->GetViewportHeight();
        bool bHori = itemHost->GetLogicalOrientation() == Orientation::Horizontal;
        AxisDirection axis = AxisDirection::yAxis;

        if (bHori)
        {
            iViewSize = scrollView->GetViewportWidth();
            axis = AxisDirection::xAxis;
        }

        elem = RTTICast<Element>(GetItemContainerGenerator()->ContainerFromItem(item));
        if (elem != NULL)
        {
            if (IsOnCurrentPage(elem, axis, true))
            {
                return -2;
            }
        }

        index = ComputeOffsetFromItem(item, eItemDirection::idCurr, offset, itemSize);

        MakeVisible(item, offset, itemSize, atTopOfViewport);

        UpdateLayout();
    }

    return index;
}

bool ItemsControl::MakeVisible(int index)
{
    return MakeVisible(index, false);
}

bool ItemsControl::MakeVisible(int index, bool atTopOfViewport)
{
    if (index < 0)
    {
        return false;
    }

    ScrollViewer* scrollView = GetScrollHost();

    if (scrollView == NULL)
    {
        return false;
    }

    
    Float horizontalOffset = scrollView->GetHorizontalOffset();
    Float verticalOffset = scrollView->GetVerticalOffset();
    Float hOffset = horizontalOffset;
    Float vOffset = verticalOffset;

    int offset = 0;
    int indexSize = 0;

    bool flag = false;
    
    ComputeOffsetFromIndex(index, offset, indexSize);

    if (IsLogicalVertical())
    {
        // 滚动到最上面
        if (atTopOfViewport)
        {
            vOffset = offset;
        }
        else
        {
            vOffset = max(0, offset - scrollView->GetViewportHeight() + indexSize);
        }

        if ((int) verticalOffset != (int) vOffset)
        {
            scrollView->ScrollToVerticalOffset(vOffset);
            flag = true;
        }

        return flag;
    }

    if (IsLogicalHorizontal())
    {
        // 滚动到最左边
        if (atTopOfViewport)
        {
            hOffset = offset;
        }
        else
        {
            hOffset = max(0, offset - scrollView->GetViewportWidth() + indexSize);
        }

        if ((int) horizontalOffset != (int) hOffset)
        {
            scrollView->ScrollToHorizontalOffset(hOffset);
            flag = true;
        }

        return flag;
    }

    FrameworkElement* element = RTTICast<FrameworkElement>(GetItemContainerGenerator()->ContainerFromIndex(index));
    
    if (element != NULL)
    {
        element->BringIntoView();
        flag = !FloatUtil::AreClose(horizontalOffset, scrollView->GetHorizontalOffset()) || 
               !FloatUtil::AreClose(verticalOffset, scrollView->GetVerticalOffset());
    }

    return flag;
}

bool ItemsControl::MakeVisible(Object* item, int offset, int iItemSize, bool atTopOfViewport)
{
    if (offset < 0)
    {
        return false;
    }

    ScrollViewer* scrollView = GetScrollHost();

    if (scrollView == NULL)
    {
        return false;
    }

    if (IsLogicalVertical())
    {
        if (!atTopOfViewport && offset > scrollView->GetVerticalOffset())
        {
            offset = max(0, offset - scrollView->GetViewportHeight() + iItemSize);
        }
        scrollView->ScrollToVerticalOffset(offset);
        return true;
    }

    if (IsLogicalHorizontal())
    {
        if (!atTopOfViewport && offset > scrollView->GetHorizontalOffset())
        {
            offset = max(0, offset - scrollView->GetViewportWidth() + iItemSize);
        }
        scrollView->ScrollToHorizontalOffset(offset);
        return true;
    }

    return false;
}

void ItemsControl::NavigateToEnd(ItemNavigateArg* e)
{
    if (HasItems())
    {
        int index = GetItemsSource()->GetCount() - 1;
        NavigateToItem(GetItem(index), -1, 0, e, true);
    }
}

void ItemsControl::NavigateToItem(Object* item, int offset, int itemLen, eItemDirection id, bool alwaysAtTopOfViewport)
{
    int iLoop = 0;
    ScrollViewer* scrollHost = GetScrollHost();

    while (MakeVisible(item, offset, itemLen, alwaysAtTopOfViewport))
    {
        int extentWidth = scrollHost->GetExtentWidth();
        int extentHeight = scrollHost->GetExtentHeight();

        scrollHost->InvalidateMeasure();
        scrollHost->UpdateArrange();

        if (extentWidth == scrollHost->GetExtentWidth() && 
            extentHeight == scrollHost->GetExtentHeight())
        {
            break;
        }

        if (iLoop >= 3)
        {
            break;
        }

        ++iLoop;
        ComputeOffsetFromItem(item, id, offset, itemLen);
    }
}

void ItemsControl::NavigateToItem(Object* item, int offset, int itemLen, ItemNavigateArg* e, bool alwaysAtTopOfViewport)
{
    int index = 0;

    if (item != DpProperty::UnsetValue())
    {
        if (offset == -1)
        {
            index = ComputeOffsetFromItem(item, eItemDirection::idCurr, offset, itemLen);
            
            if (index < 0)
            {
                return;
            }
        }

        if (offset < 0)
        {
            return;
        }
    }
    else
    {
        return;
    }

    NavigateToItem(item, offset, itemLen, eItemDirection::idCurr, alwaysAtTopOfViewport);

    UpdateFocusItem(GetItemsSource()->GetItemEntry(index));
    UpdateLayout();
}

void ItemsControl::HandleSelectedItem(Object* container, MouseButton mouseButton)
{
}

void ItemsControl::UpdateFocusItem(suic::ItemEntry* item)
{
    if (IsKeyboardFocusWithin())
    {
        SetFocusItem(item->GetItem());
    }
    _focusedItem = item;
}

void ItemsControl::SetFocusItem(Object* item)
{
    if (item != NULL)
    {
        Element* elem = GetContainerFromItem(item);
        if (elem != NULL)
        {
            Keyboard::Focus(elem);
        }
    }
}

void ItemsControl::SetItemsHost(Panel* val)
{
    _itemsHost = val;
    if (NULL != _itemsHost)
    {
        _isVirtualizing = _itemsHost->GetRTTIType()->InheritFrom(VirtualizingPanel::RTTIType());
    }
    else
    {
        _isVirtualizing = false;
    }
}

void ItemsControl::CreateItemCollectionAndGenerator()
{
    if (NULL == _items)
    {
        SetItemsSource(new ObservableCollection());
    }
    //_groupStyle.CollectionChanged += new NotifyCollectionChangedEventHandler(this.OnGroupStyleChanged);
}

}
