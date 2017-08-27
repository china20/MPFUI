
#include "stdafx.h"

#include <Core/XamlSceneNode.h>

ImplementRTTIOfClass(XamlSceneNode, suic::HierarchicalItem)
ImplementRTTIOfClass(XamlNodeItem, XamlSceneNode)

XamlSceneNode::XamlSceneNode()
{
    
}

XamlSceneNode::~XamlSceneNode()
{
    
}

XamlNodeItem::XamlNodeItem()
{
    _isLocked = false;
}

XamlNodeItem::~XamlNodeItem()
{
}

void XamlNodeItem::Dispose()
{
    _parent = NULL;
}

String XamlNodeItem::GetDisplayName()
{
    return ToString();
}

void XamlNodeItem::UpdateDisplayName()
{
    NotifyChanged(_U("DisplayName"));
}

ItemCollection* XamlNodeItem::GetItemColl()
{
    return GetChildren();
}

int XamlNodeItem::GetCount()
{
    return GetItemColl()->GetCount();
}

XamlNodeItem* XamlNodeItem::GetItem(int index)
{
    return RTTICast<XamlNodeItem>(GetItemColl()->GetItem(index));
}

void XamlNodeItem::AddItem(XamlNodeItem* val)
{
    OnAddItem(val);
}

void XamlNodeItem::OnAddItem(XamlNodeItem* val)
{
    GetItemColl()->AddItem(val);
    val->SetParent(this);
}

void XamlNodeItem::InsertItem(int index, XamlNodeItem* child)
{
    GetItemColl()->InsertItem(index, child);
}

void XamlNodeItem::RemoveItem(XamlNodeItem* val)
{
    val->SetParent(NULL);
    GetItemColl()->RemoveItem(val);
}

void XamlNodeItem::RemoveItemAt(int index)
{
    if (index >= 0 && index < GetCount())
    {
        Object* pObj = GetItemColl()->GetItem(index);
        XamlNodeItem* pItem = RTTICast<XamlNodeItem>(pObj);
        if (NULL != pItem)
        {
            pItem->SetParent(NULL);
        }
        GetItemColl()->RemoveItemAt(index);
    }
}

void XamlNodeItem::Clear()
{
    if (NULL != GetItemColl())
    {
        GetItemColl()->Clear();
    }
}

XamlNodeItem* XamlNodeItem::GetParent() const
{
    return _parent.GetElement<XamlNodeItem>();
}

void XamlNodeItem::SetParent(XamlNodeItem* val)
{
    _parent = val;
}

bool XamlNodeItem::GetLockedSelf() const
{
    return _isLocked;
}

bool XamlNodeItem::IsLocked() const
{
    const XamlNodeItem* pItem = this;
    while (pItem != NULL)
    {
        if (pItem->_isLocked)
        {
            return true;
        }

        pItem = RTTICast<XamlNodeItem>(pItem->GetParent());
    }

    return false;
}

void XamlNodeItem::SetLocked(bool val)
{
    _isLocked = val;
}
