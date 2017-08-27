
#include "stdafx.h"

#include <Core/TypeManager.h>
#include <Core/SetterCollectionNode.h>

ImplementRTTIOfClass(SetterSelf, XamlNodeItem)
ImplementRTTIOfClass(SetterCollectionNode, XamlNodeItem)

SetterSelf::SetterSelf()
{
    _setters = NULL;
}

SetterSelf::SetterSelf(SetterCollectionNode* setters)
{
    _setters = setters;
}

SetterCollectionNode::SetterCollectionNode()
{
    _rttiInfo = NULL;
}

SetterCollectionNode::~SetterCollectionNode()
{
    Clear();
}

void SetterCollectionNode::Clear()
{
    XamlNodeItem::Clear();
}

void SetterCollectionNode::RemoveSetter(SetterNode* pNode)
{
    for (int i = 0; i < GetCount(); ++i)
    {
        if (GetItem(i) == pNode)
        {
            RemoveItemAt(i);
            break;
        }
    }
}

void SetterCollectionNode::RemoveSetter(const String& strName)
{
    for (int i = 0; i < GetCount(); ++i)
    {
        if (GetSetter(i)->GetName().Equals(strName))
        {
            RemoveItemAt(i);
            break;
        }
    }
}

void SetterCollectionNode::CloneNode(ResNodePtr& obj)
{
    SetterCollectionNode* pSetterColl = new SetterCollectionNode();
    obj = pSetterColl;
    CloneSetterCollectionNode(pSetterColl);
}

void SetterCollectionNode::CloneSetterCollectionNode(SetterCollectionNode* pSetterColl)
{
    pSetterColl->_rttiInfo = _rttiInfo;

    for (int i = 0; i < GetCount(); ++i)
    {
        SetterNode* pSetter = GetSetter(i);
        SetterNode* pClone = new SetterNode();
        
        pSetterColl->AddSetter(pClone);
        pSetter->CloneSetterNode(pClone);        
    }
}

suic::String SetterCollectionNode::GetResXml(const String& offset)
{
    suic::String strXml;
    RTTIOfInfo* ownerType = GetOwnerType();

    for (int i = 0; i < GetCount(); ++i)
    {
        String strSetterXml;
        SetterNode* pSetter = GetSetter(i);
        String strSetter;
        String strPropName;
        DpProperty* dp = NULL;
        PropMetadata* propMeta = NULL;

        strSetter = offset + _U("<Setter Property=\"");

        strPropName = pSetter->GetName();
        dp = pSetter->GetProperty();

        if (dp == suic::Element::NameProperty)
        {
            suic::String tmpName = pSetter->GetValue()->ToString();
            tmpName.Trim();
            if (tmpName.Empty())
            {
                continue;
            }
        }

        ResNode* resNode = pSetter->GetResNode();

        if (resNode != NULL && NullResNode::Value != resNode)
        {
            if (dp != NULL)
            {
                propMeta = dp->GetMetadata(ownerType);

                if (NULL == propMeta)
                {
                    propMeta = dp->GetDefaultMetadata();
                }

                int iDotPos = strPropName.IndexOf(_U("."));

                if (iDotPos == -1 && 
                    dp->IsAttached() && 
                    !ownerType->InheritFrom(dp->GetOwnerType()) && 
                    !ownerType->InheritFrom(propMeta->GetType()))
                {
                    strSetter += dp->GetOwnerType()->typeName;
                    strSetter += _U(".");
                }
            }

            strSetter += strPropName;
            strSetter += _U("\"");

            if (!pSetter->GetTargetName().Empty())
            {
                strSetterXml += _U(" TargetName=\"");
                strSetterXml += pSetter->GetTargetName();
                strSetterXml += _U("\"");
            }

            if (resNode->IsSingleValue())
            {
                if (resNode->GetValue() != NULL && resNode->GetValue() != DpProperty::UnsetValue())
                {
                    DpItem* dpItem = pSetter->GetDpItem();
                    String strTmp;

                    if (NULL != dpItem)
                    {
                        strTmp = dpItem->ValueToString(pSetter->GetResNode());
                    }
                    else
                    {
                        // tbl
                        ResNode* sRes = pSetter->GetResNode();
                        if (NULL != sRes)
                        {
                            strTmp = sRes->GetSingleXml();
                        }
                    }

                    strSetterXml += _U(" Value=\"");
                    strSetterXml += strTmp;

                    strSetterXml += _U("\" />\n");
                }
                else
                {
                    strSetterXml = _U("");
                }
            }
            else
            {
                String strTmp = pSetter->GetResNode()->GetResXml(offset + ResNode::OFFSET2);

                if (!strTmp.Empty())
                {
                    strSetterXml += _U(">\n");
                    strSetterXml += offset + ResNode::OFFSET1 + _U("<Setter.Value>\n");
                    strSetterXml += strTmp;
                    strSetterXml += offset + ResNode::OFFSET1 + _U("</Setter.Value>\n");
                    strSetterXml += offset + _U("</Setter>\n");
                }
                else
                {
                    strSetterXml += _U("/>\n");
                }
            }

            if (!strSetterXml.Empty())
            {
                strXml += strSetter;
                strXml += strSetterXml;
            }
        }
    }

    return strXml;
}

void SetterCollectionNode::AddSetter(SetterNode* pNode)
{
    AddItem(pNode);
}

void SetterCollectionNode::AddSetterForAdd(SetterNode* pNode)
{
    int index = -1;
    for (int i = 0; i < GetCount(); ++i)
    {
        SetterNode* pSetter = GetSetter(i);
        DpProperty* dp = pSetter->GetProperty();

        if (dp != NULL && 
            (dp->GetPropType()->InheritFrom(suic::FrameworkTemplate::RTTIType())))
        {
            index = i;
            break;
        }
    }

    if (index == -1)
    {
        AddItem(pNode);
    }
    else
    {
        InsertItem(index, pNode);
    }
}

void SetterCollectionNode::SetRTTIOfInfo(RTTIOfInfo* rttiInfo)
{
    _rttiInfo = rttiInfo;
}

RTTIOfInfo* SetterCollectionNode::GetOwnerType() const
{
    return _rttiInfo;
}

SetterNode* SetterCollectionNode::GetSetter(int index)
{
    return RTTICast<SetterNode>(GetItem(index));
    //return _setters[index];
}

SetterNode* SetterCollectionNode::FindSetter(const String& name)
{
    for (int i = 0; i < GetCount(); ++i)
    {
        SetterNode* pNode = GetSetter(i);

        if (pNode->GetName().Equals(name))
        {
            return pNode;
        }
    }
    return NULL;
}