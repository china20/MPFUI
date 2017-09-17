/////////////////////////////////////////////////////////////////////////
// Panel.cpp


#include <Framework/Controls/Panel.h>
#include <Framework/Controls/ItemsControl.h>
#include <Framework/Controls/VisualizeStackPanel.h>
#include <Framework/Presenter/ItemContainerGenerator.h>
#include <System/Windows/EventManager.h>

/*--------内部使用头文件--------*/
#include <System/Interop/InternalOper.h>

#include <System/Graphics/SolidcolorBrush.h>

namespace suic
{

ImplementRTTIOfClass(Panel, FrameworkElement)

DpProperty* Panel::IsItemsHostProperty;
DpProperty* Panel::ZIndexProperty;
DpProperty* Panel::BackgroundProperty;

Panel::Panel()
    : _elemColl(NULL)
    , _icGenerator(NULL)
    , _flagField(0)
    , _background(NULL)
{
}

Panel::~Panel()
{
    _effects.Clear();

    if (_evthStore)
    {
        _evthStore->Clear();
    }

    if (NULL != _elemColl)
    {
        _elemColl->Clear();
        delete _elemColl;
        _elemColl = NULL;
    }
}

void Panel::OnZIndexPropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    FrameworkElement* elem = RTTICast<FrameworkElement>(d);
    if (NULL != elem)
    {
        Panel* pPanel = RTTICast<Panel>(elem->GetParent());
        if (pPanel)
        {
            pPanel->SetIsZStateDirty(true);
            pPanel->InvalidateArrange();
        }
    }
}

void Panel::OnBackgroundPropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    Panel* elem = RTTICast<Panel>(d);
    if (NULL != elem)
    {
        elem->_background = DynamicCast<Brush>(e->GetNewValue());
    }
}

void Panel::StaticInit()
{
    if (ZIndexProperty == NULL)
    {
        FrameworkElement::StaticInit();

        IsItemsHostProperty = DpProperty::Register(_U("IsItemsHost"), RTTIType(), Boolean::RTTIType()
            , DpPropMemory::GetPropMeta(Boolean::False, PropMetadataOptions::AffectsNone, &Panel::OnIsItemsHostPropChanged));
        ZIndexProperty = DpProperty::RegisterAttached(_U("ZIndex"), RTTIType(), Integer::RTTIType(), 
            DpPropMemory::GetPropMeta(Integer::GetZeroInt(), PropMetadataOptions::AffectsNone, &Panel::OnZIndexPropChanged));
        BackgroundProperty = DpProperty::Register(_U("Background"), RTTIType(), Brush::RTTIType(), 
            DpPropMemory::GetPropMeta(NULL, PropMetadataOptions::AffectsRender, &Panel::OnBackgroundPropChanged));

        EventControl::RegisterClassHandler(RTTIType(), InvalidateEvent, new RoutedEventHandler(&Panel::OnInvalidateThunk), false);
    }
}

void Panel::OnIsItemsHostPropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    Panel* panel = RTTICast<Panel>(d);
    bool oldIsItemsHost = false;
    bool newIsItemsHost = false;

    if (e->GetOldValue())
    {
        oldIsItemsHost = e->GetOldValue()->ToBool();
    }

    if (e->GetNewValue())
    {
        newIsItemsHost = e->GetNewValue()->ToBool();
    }

    panel->OnIsItemsHostChanged(oldIsItemsHost, newIsItemsHost);
}

void Panel::OnInvalidateThunk(DpObject* sender, RoutedEventArg* e)
{
    Panel* panel = RTTICast<Panel>(sender);
    InvalidateEventArg* iee = (InvalidateEventArg*)&e;
    
    if (!iee->GetProp())
    {
        DoInvalidate(panel, *iee);
        e->SetHandled(true);
    }
    else
    {
        int iWid = panel->GetWidth();
        int iHei = panel->GetHeight();
        int iHori = panel->GetHorizontalAlignment();
        int iVert = panel->GetVerticalAlignment();

        if ((iee->GetProp() == WidthProperty && iWid < 0 && HoriAlign::haStretch != iHori) || 
            (iee->GetProp() == HeightProperty && iHei < 0 && VertAlign::vaStretch != iVert))
        {
        }
        else
        {
            DoInvalidate(panel, *iee);
            e->SetHandled(true);
        }
    }
}

int Panel::GetVisibleStart() const
{
    return 0;
}

int Panel::GetZIndex(Element* element)
{
    if (element == NULL)
    {
        throw ArgumentNullException(_U("element"), __FILELINE__);
    }
    return element->GetValue(ZIndexProperty)->ToInt();
}

void Panel::SetZIndex(Element* element, int value)
{
    if (element == NULL)
    {
        throw ArgumentNullException(_U("element"), __FILELINE__);
    }
    element->SetValue(ZIndexProperty, new Integer(value));
}

Panel* Panel::Clone()
{
    return NULL;
}

void Panel::Dispose()
{
    if (NULL != _elemColl)
    {
        _elemColl->Clear();
        delete _elemColl;
        _elemColl = NULL;
    }
}

bool Panel::IsVirtualizing()
{
    return (NULL != suic::DynamicCast<VirtualizingPanel>(this));
}

void Panel::OnRender(Drawing* drawing)
{
    Brush* background(GetBackground());

    if (background != NULL)
    {
        Size rSize = GetRenderSize();
        fRect rect(Point(), rSize);
        drawing->DrawRect(DrawCtx::DefDraw, background, NULL, rect);
    }
}

void Panel::AddChild(Object* value)
{
    if (value == NULL)
    {
        throw ArgumentNullException(_U("value"), __FILELINE__);
    }

    // 
    // 数据项宿主，不能加入子项，子项由ItemContainerGenerator产生
    // 由ItemCollection管理
    //
    if (IsItemsHost())
    {
        throw InvalidOperationException(_U("Panel_BoundPanel_NoChildren"), __FILELINE__);
    }

    Element* element(RTTICast<Element>(value));

    if (element == NULL)
    {
        throw ArgumentException(_U("UnexpectedParameterType"), __FILELINE__);
    }

    GetChildren()->Add(element);
}

void Panel::AddText(String text)
{
    AddChild(new OString(text));
}

void Panel::RemoveAt(int index)
{
    GetChildren()->RemoveAt(index);
}

void Panel::RemoveChild(Object* child)
{
    Element* pElem = RTTICast<Element>(child);
    GetChildren()->Remove(pElem);
}

int Panel::GetVisualChildrenCount()
{
    if (_elemColl == NULL)
    {
        return 0;
    }
    else
    {
        return _elemColl->GetCount();
    }
}

Element* Panel::GetVisualChild(int index)
{
    if (_elemColl == NULL)
    {
        return NULL;
    }

    if (IsZStateDirty() || _elemColl->GetCount() != _zIndexs.GetCount())
    {
        RecomputeZState();
    }

    int num = (_zIndexs.Length() > 0) ? _zIndexs[index] : index;
    return (*_elemColl)[num];
}

Element* Panel::GetLogicalChild(int index)
{
    return GetVisualChild(index);
}

int Panel::GetLogicalChildrenCount()
{
    if (GetTemplatedParent() != NULL || IsItemsHost())
    {
        return 0;
    }
    else
    {
        return GetVisualChildrenCount();
    }
}

int Panel::GetLogicalOrientation()
{
    return Orientation::Horizontal;
}

void Panel::NotifyItemChanged()
{

}

int Panel::ComputeOffsetFromItem(Object* item, eItemDirection id, int& offset, int& itemSize)
{
    return -1;
}

int Panel::ComputeOffsetFromIndex(int index, int& offset, int& itemSize)
{
    return -1;
}

void Panel::GenerateChildren()
{
    if (_icGenerator != NULL)
    {
        if (_icGenerator->StartAt(0, -1))
        {
            while (_icGenerator->HasNext())
            {
                Element* elem = _icGenerator->Generate();
                if (NULL != elem)
                {
                    _elemColl->AddInternal(elem);
                    elem->unref();
                    // 根据项容器和项生存容器内容
                    _icGenerator->PrepareItemContainer(elem);
                }
            }
        }
    }
}

void Panel::InvalidateZState()
{
    SetIsZStateDirty(true);
}

void Panel::EnsureGenerator()
{
    if (_icGenerator == NULL)
    {
        ItemsControl* itemsOwner = NULL;

        ConnectToGenerator();
        itemsOwner = ItemsControl::GetItemsOwner(this);
        EnsureEmptyChildren(itemsOwner);
        GenerateChildren();
    }
}

void Panel::ClearChildren()
{
    if (_icGenerator != NULL)
    {
        _icGenerator->RemoveAll();
    }

    if (_elemColl != NULL && _elemColl->GetCount() > 0)
    {
        _elemColl->ClearInternal();
        OnClearChildrenInternal();
    }
}

void Panel::ConnectToGenerator()
{
    ItemsControl* itemsOwner = ItemsControl::GetItemsOwner(this);
    if (itemsOwner == NULL)
    {
        return;
    }

    ItemContainerGenerator* itemContainerGenerator = itemsOwner->GetItemContainerGenerator();
    if (itemContainerGenerator != NULL)
    {
        _icGenerator = itemContainerGenerator->GetItemContainerGeneratorForPanel(this);
        if (NULL != _icGenerator)
        {
            ItemsChangedEventHandler e(this, &Panel::OnItemsChanged);
            _icGenerator->ItemsChanged += e;
            _icGenerator->NotifyItemHost(this);
            _icGenerator->RemoveAll();
        }
    }
}

void Panel::DisconnectFromGenerator()
{
    if (NULL != _icGenerator)
    {
        ItemsChangedEventHandler e(this, &Panel::OnItemsChanged);
        _icGenerator->ItemsChanged -= e;
        _icGenerator->RemoveAll();
        _icGenerator = NULL;

        ItemsPresenter* presenter = ItemsPresenter::FromPanel(this);
        if (NULL != presenter)
        {
            ;
        }
    }
}

void Panel::EnsureEmptyChildren(FrameworkElement* logicalParent)
{
    if ((_elemColl == NULL) || (_elemColl->GetLogicalParent() != logicalParent))
    {
        if (_elemColl)
        {
            _elemColl->Clear();
            delete _elemColl;
        }
        _elemColl = CreateUIElementCollection(logicalParent);
    }
    else
    {
        ClearChildren();
    }
}

ElementColl* Panel::GetInternalChildren()
{
    CheckItemsHost();

    if (IsItemsHost())
    {
        EnsureGenerator();
    }
    else if (_elemColl == NULL)
    {
        EnsureEmptyChildren(this);
    }
    return _elemColl;
}

Element* Panel::GetInternalChild(int index)
{
    return GetInternalChildren()->GetAt(index);
}

void Panel::RecomputeZState()
{
    Uint32 capacity = (_elemColl != NULL) ? _elemColl->GetCount() : 0;
    bool flag = false;
    bool flag2 = false;
    Uint32 defaultValue = 0;
    Uint32 num3 = defaultValue;

    if (capacity > 0 && capacity < 0x0000FFFF)
    {
        ElementColl& uiElems = *_elemColl;

        if ((*_elemColl)[0] != NULL)
        {
            num3 = Panel::GetZIndex(uiElems[0]);
        }

        if (capacity > 1)
        {
            _zIndexs.Clear();
            _zIndexs.Resize(capacity);

            _zIndexs.Add(num3 << 16);
            Uint32 num4 = num3;
            Uint32 num5 = 1;

            do
            {
                Uint32 num6 = (uiElems[num5] != NULL) ? Panel::GetZIndex(uiElems[num5]) : defaultValue;
                _zIndexs.Add((num6 << 16) + num5);
                flag2 |= num6 < num4;
                num4 = num6;
                flag |= num6 != num3;
            }
            while (++num5 < capacity);
        }

        if (flag2)
        {
            ValEqual<int> comparer;
            _zIndexs.Sort<ValEqual<int> >(&comparer, 0);

            for (int i = 0; i < _zIndexs.Length(); ++i)
            {
                _zIndexs[i] = _zIndexs[i] & 0x0000FFFF;
            }
        }
        else
        {
            _zIndexs.Clear();
        }
    }
    else
    {
        _zIndexs.Clear();
    }

    SetIsZStateDirty(false);
}

bool Panel::CheckItemsHost()
{
    if (IsItemsHost())
    {
        if (_icGenerator == NULL)
        {
            ClearChildren();
        }
        return (_icGenerator != NULL);
    }

    if (_icGenerator != NULL)
    {
        DisconnectFromGenerator();
        ClearChildren();
    }

    return false;
}

void Panel::AddChildren(int pos, int itemCount)
{
    if (_icGenerator != NULL)
    {
        int iEnd = pos + itemCount;
        // 根据用户数据项生成实际的UI元素
        // 移动到开始位置
        if (_icGenerator->StartAt(pos, iEnd - 1))
        {
            Element* container = NULL;
            while (_icGenerator->HasNext())
            {
                container = _icGenerator->Generate();
                if (NULL != container)
                {
                    _elemColl->AddInternal(container);
                    container->unref();
                    // 根据项容器和项生存容器内容
                    _icGenerator->PrepareItemContainer(container);
                }
            }
        }
    }
}

void Panel::MoveChildren(int fromPos, int toPos, int containerCount)
{
    if (fromPos != toPos)
    {
        Array<Element*, false> elemArr;
        ItemContainerGenerator* generator = GetGenerator();

        for (int i = 0; i < containerCount; i++)
        {
            Element* child = _elemColl->GetAt(fromPos + i);
            elemArr.Add(child);
            child->ref();
        }

        _elemColl->RemoveRange(fromPos, containerCount);

        for (int j = 0; j < containerCount; j++)
        {
            Element* child = elemArr[j];
            _elemColl->Insert(toPos + j, child);
            child->unref();
        }

        elemArr.Clear();
    }
}

void Panel::SwapChildren(int pos1, int pos2)
{
    Element* src = _elemColl->GetAt(pos1);
    Element* dest = _elemColl->GetAt(pos2);
    _elemColl->Swap(src, dest);
}

void Panel::RemoveChildren(int pos, int containerCount)
{
    _elemColl->RemoveRange(pos, containerCount);
}

void Panel::ReplaceChildren(int pos, int itemCount, int containerCount)
{
    if (_icGenerator != NULL)
    {
        int iEnd = pos + itemCount;
        // 根据用户数据项生成实际的UI元素
        // 移动到开始位置
        if (_icGenerator->StartAt(pos, iEnd - 1))
        {
            Element* container = NULL;
            while (_icGenerator->HasNext())
            {
                container = _icGenerator->Generate();
                if (NULL != container)
                {
                    _elemColl->SetInternal(_icGenerator->GetPos(), container);
                    container->unref();
                    // 根据项容器和项生存容器内容
                    _icGenerator->PrepareItemContainer(container);
                }
            }
        }
    }
}

void Panel::ResetChildren()
{
    if (_elemColl == NULL)
    {
        EnsureEmptyChildren(NULL);
    }
    else
    {
        EnsureEmptyChildren(_elemColl->GetLogicalParent());
    }
    GenerateChildren();
}

ElementColl* Panel::CreateUIElementCollection(FrameworkElement* logicalParent)
{
    return new ElementColl(this, logicalParent);
}

void Panel::SetIsZStateDirty(bool val)
{
    UISETFLAG(_flagField, BoolField::IsZStateDirtyField, val);
}

void Panel::OnItemsChanged(Object* sender, ItemsChangedEventArg* e)
{
    if (CheckItemsHost())
    {
        OnItemsChangedInternal(sender, e);
        UpdateLayout();
    }
}

void Panel::OnClearChildrenInternal()
{
}

void Panel::OnVisualChildrenChanged(DpObject* visualAdded, DpObject* visualRemoved)
{
}

void Panel::OnItemsChangedInternal(Object* sender, ItemsChangedEventArg* e)
{
    InvalidateZState();

    switch (e->GetAction())
    {
    case NotifyCollectionChangedAction::Add:
        AddChildren(e->GetNewPos(), e->GetCount());
        break;

    case NotifyCollectionChangedAction::Remove:
        RemoveChildren(e->GetOldPos(), e->GetCount());
        break;

    case NotifyCollectionChangedAction::Replace:
        ReplaceChildren(e->GetNewPos(), e->GetCount(), e->GetCount());
        break;

    case NotifyCollectionChangedAction::Move:
        MoveChildren(e->GetOldPos(), e->GetNewPos(), e->GetCount());
        return;

    case NotifyCollectionChangedAction::Swap:
        SwapChildren(e->GetOldPos(), e->GetNewPos());
        break;

    case NotifyCollectionChangedAction::Reset:
        ResetChildren();
        break;
    }
}

void Panel::OnIsItemsHostChanged(bool oldIsItemsHost, bool newIsItemsHost)
{
    ItemsControl* itemsOwner = ItemsControl::GetItemsOwner(this);
    if (itemsOwner != NULL)
    {
        ItemContainerGenerator* itemContainerGenerator = itemsOwner->GetItemContainerGenerator();
        if (itemContainerGenerator == itemContainerGenerator->GetItemContainerGeneratorForPanel(this))
        {
            itemsOwner->SetItemsHost(this);
        }
    }
    
    // 刷新容器
    CheckItemsHost();
}

void Panel::OnRequestBringIntoView(RequestBringIntoViewEventArg* e)
{
    suic::Element* elem = (suic::Element*)e->GetOriginalSource();
    suic::ItemsControl* itemsOwner = NULL;

    itemsOwner = suic::ItemsControl::GetItemsOwner(this);

    if (itemsOwner != NULL)
    {
        suic::Object* item = elem->GetDataContext().GetTarget();
        e->SetHandled(true);

        if (NULL != item)
        {
            itemsOwner->ScrollIntoView(item, false);
        }
        
    }
}

}
