#include "stdafx.h"

#include <Core/HoldItem.h>
#include <Core/ResNode.h>

void HoldItem::InitNode(suic::IXamlNode* pNode)
{
    pNode->Reset();
    InitNode(this, pNode);
}

void HoldItem::Clear()
{
    for (int i = 0; i < children.GetCount(); ++i)
    {
        Clear(children.GetItem(i));
    }

    attrs.Clear();
    children.Clear();
}

String HoldItem::GetResXml(const String& offset)
{
    String strChild = GetResXml(this, offset);
    return strChild;
}

String HoldItem::GetChildResXml(const String& offset)
{
    String strChild;
    for (int j = 0; j < children.GetCount(); ++j)
    {
        HoldItem* pChild = children.GetItem(j);
        strChild += GetResXml(pChild, offset);
    }
    return strChild;
}

void HoldItem::CloneTo(HoldItem* Other)
{
    Other->Clear();
    Other->name = name;

    for (int i = 0; i < attrs.GetCount(); ++i)
    {
        Other->attrs.Add(attrs.GetItem(i));
    }

    for (int j = 0; j < children.GetCount(); ++j)
    {
        HoldItem* pChild = children.GetItem(j);
        HoldItem* pOtherChild = new HoldItem();

        Other->children.Add(pOtherChild);
        pChild->CloneTo(pOtherChild);
    }
}

void HoldItem::Clear(HoldItem* pHold)
{
    for (int i = 0; i < pHold->children.GetCount(); ++i)
    {
        Clear(pHold->children.GetItem(i));
    }
    delete pHold;
}

void HoldItem::InitNode(HoldItem* pHold, suic::IXamlNode* pNode)
{
    pHold->name = pNode->GetName();
    suic::IXamlAttris* pAttris = pNode->GetAttris();
    if (NULL != pAttris)
    {
        while (pAttris->HasNext())
        {
            StringPair pair(pAttris->GetName(), pAttris->GetValue());
            pHold->attrs.Add(pair);
        }
    }

    while (pNode->HasNext())
    {
        suic::IXamlNode* childNode = pNode->Current();
        if (NULL != childNode)
        {
            HoldItem* pChildItem = new HoldItem();

            pHold->children.Add(pChildItem);
            InitNode(pChildItem, childNode);
        }
    }
}

String HoldItem::GetResXml(HoldItem* pHold, const String& offset)
{
    String strXml;

    strXml += offset + _U("<") + pHold->name;
    for (int i = 0; i < pHold->attrs.GetCount(); ++i)
    {
        StringPair& pair = pHold->attrs.GetItem(i);
        strXml += _U(" ") + pair.GetKey() + _U("=\"") + pair.GetValue() + _U("\"");
    }

    if (pHold->children.GetCount() > 0)
    {
        strXml += _U(">\n");

        String strChild;
        for (int j = 0; j < pHold->children.GetCount(); ++j)
        {
            HoldItem* pChildHold = pHold->children.GetItem(j);
            strChild += GetResXml(pChildHold, offset + ResNode::OFFSET1);
        }

        strXml += strChild;
        strXml += offset + _U("</") + pHold->name + _U(">\n");
    }
    else
    {
        strXml += _U(" />\n");
    }
    return strXml;
}