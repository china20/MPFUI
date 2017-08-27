
#include "stdafx.h"

#include <System/Types/XamlNode.h>
#include <System/Tools/StyleHelper.h>
#include <System/Tools/TreeBuilder.h>
#include <System/Resources/StaticResource.h>

#include <Framework/Controls/TextBlock.h>

#include <Editor/XamlWalker.h>
#include <Main/Project.h>
#include <Core/TypeManager.h>
#include <Core/StyleNode.h>
#include <Core/XamlRedoUndo.h>
#include <Main/XamlRootItem.h>

#include <Main/MainWindow.h>

#include <Main/DesignPanel.h>
#include <Main/SlnTreeManager.h>
#include <Main/ElementTreeManager.h>

ImplementRTTIOfClass(DesignElement, ResNode)
ImplementRTTIOfClass(DesignContent, DesignElement)
ImplementRTTIOfClass(DesignGrid, DesignElement)
ImplementRTTIOfClass(DesignTrack, DesignElement)
ImplementRTTIOfClass(UserDesignElement, DesignElement)

//=======================================================

extern LockShowHandler* g_lockshow;

DesignContent::DesignContent()
{

}

DesignContent::~DesignContent()
{

}

Object* DesignContent::GetContent()
{
    if (GetCount() == 1)
    {
        return GetXamlElement(0);
    }
    else
    {
        SetterCollectionNode* pSetterColl = GetSetterCollection();
        return pSetterColl->FindSetter(ContentControl::ContentProperty->GetName());
    }
}

void DesignContent::UpdateElementProperty(DpProperty* dp, Object* newVal)
{
    if (dp == ContentControl::ContentProperty)
    {
        DesignElement* pElem = RTTICast<DesignElement>(newVal);
        if (NULL != pElem)
        {
            AddElement(pElem);
            return;
        }
        ClearChildren();
    }
    DesignElement::UpdateElementProperty(dp, newVal);
}

void DesignContent::AddXamlElement(DesignElement* elem)
{
    DpProperty* dp = ContentControl::ContentProperty;
    SetterCollectionNode* pSetterColl = GetSetterCollection();

    ClearChildren();
    pSetterColl->RemoveSetter(dp->GetName());
    DesignElement::AddXamlElement(elem);
}

bool DesignContent::RemoveElement(DesignElement* elem)
{
    DpProperty* dp = ContentControl::ContentProperty;
    SetterCollectionNode* pSetterColl = GetSetterCollection();

    if (GetCount() == 1 && GetItem(0) == elem)
    {
        ClearChildren();
        pSetterColl->RemoveSetter(dp->GetName());
        return true;
    }
    else
    {
        return false;
    }
}

DesignElement::DesignElement()
{
    _isModified = false;
    _isRoot = false;
    _isTemplate = false;

    _typeInfo = NULL;
    _element = NULL;
    _setterColl = new SetterCollectionNode();
    _resDic = new ResourceDictionaryNode();

    _resDic->ref();
    _setterColl->ref();
}

DesignElement::DesignElement(bool isTemplate)
{
    _isModified = false;
    _isRoot = false;

    _isTemplate = isTemplate;
    _element = NULL;
    _typeInfo = NULL;
    _setterColl = new SetterCollectionNode();
    _resDic = new ResourceDictionaryNode();

    _resDic->ref();
    _setterColl->ref();
}

DesignElement::~DesignElement()
{
    FREEREFOBJ(_resDic);
    FREEREFOBJ(_setterColl);
    FREEREFOBJ(_element);
}

void DesignElement::CloneNode(ResNodePtr& obj)
{
    DesignElement* pElemNode = RTTICast<DesignElement>(GetRTTIType()->Create());

    obj = pElemNode;

    pElemNode->_isRoot = _isRoot;
    pElemNode->_isTemplate = _isTemplate;

    pElemNode->_element = NULL;

    pElemNode->_typeInfo = _typeInfo;
    _setterColl->CloneSetterCollectionNode(pElemNode->_setterColl);
    _resDic->CloneResourceDic(pElemNode->_resDic);

    for (int i = 0; i < GetCount(); ++i)
    {
        DesignElement* pChild = GetXamlElement(i);
        ResNodePtr newChild;
        pChild->CloneNode(newChild);
        DesignElement* newElemNode = RTTICast<DesignElement>(newChild.get());
        pElemNode->AddXamlElement(newElemNode);
    }
}

suic::String DesignElement::GetName()
{
    Element* elem = CASTTOE(_element);
    if (elem)
    {
        return elem->GetName();
    }
    else
    {
        return suic::String();
    }
}

suic::String DesignElement::GetTypeName()
{
    if (_typeInfo != NULL)
    {
        return _typeInfo->typeName;
    }
    else
    {
        return _U("");
    }
}

suic::String DesignElement::GetNodeName()
{
    return _nodeName;
}

void DesignElement::SetNodeName(const suic::String& val)
{
    _nodeName = val;
}

void DesignElement::SetTypeInfo(suic::RTTIOfInfo* typeInfo)
{
    _typeInfo = typeInfo;
    GetSetterCollection()->SetRTTIOfInfo(typeInfo);
}

suic::RTTIOfInfo* DesignElement::GetTypeInfo() const
{
    return _typeInfo;
}

Object* DesignElement::GetElementIcon()
{
    String strName = GetTypeName();
    strName += _U("_s");
    Object* resItem = Application::Current()->FindRes(strName);
    if (DpProperty::UnsetValue() == resItem)
    {
        resItem = Application::Current()->FindRes(_U("Element_s"));
    }
    return resItem;
}

bool DesignElement::GetShowElement()
{
    DesignElement* pItem = this;
    while (pItem != NULL)
    {
        if (pItem->GetUIElement()->GetVisibility() != suic::Visibility::Visible)
        {
            return false;
        }

        pItem = RTTICast<DesignElement>(pItem->GetParent());
    }

    return true;
}

void DesignElement::SetShowElement(bool val)
{
    if (val)
    {
        GetUIElement()->SetVisibility(suic::Visibility::Visible);
    }
    else
    {
        GetUIElement()->SetVisibility(suic::Visibility::Hidden);
    }

    NotifyShowChanged();
}

void DesignElement::NotifyShowChanged()
{
    NotifyChanged(_U("ShowElement"));
    for (int i = 0; i < GetCount(); ++i)
    {
        GetXamlElement(i)->NotifyShowChanged();
    }
}

bool DesignElement::GetLockElement()
{
    return IsLocked();
}

void DesignElement::NotifyLockedChanged()
{
    NotifyChanged(_U("LockElement"));
    for (int i = 0; i < GetCount(); ++i)
    {
        GetXamlElement(i)->NotifyLockedChanged();
    }
}

void DesignElement::SetLockElement(bool val)
{
    SetLocked(val);
    NotifyLockedChanged();
}

String DesignElement::GetDisplayName()
{
    return ToString();
}

void DesignElement::SetDisplayName(String name)
{
    GetUIElement()->SetName(name);
    NotifyChanged(_U("DisplayName"));
}

//suic::Object* XamlElementNode::GetItemsSource()
//{
//    if (GetCount() > 0)
//    {
//        return XamlNodeItem::GetItemsSource();
//    }
//    else
//    {
//        return DpProperty::UnsetValue();
//    }
//}

bool DesignElement::IsModified() const
{
    return _isModified;
}

void DesignElement::SetModified(bool val)
{
    _isModified = val;
    UpdateDisplayName();
}

bool DesignElement::IsRoot() const
{
    return _isRoot;
}

void DesignElement::SetIsRoot(bool val)
{
    _isRoot = val;
}

bool DesignElement::IsTemplate() const
{
    return _isTemplate;
}

void DesignElement::SetIsTemplate(bool val)
{
    _isTemplate = val;
}

bool DesignElement::IsTemplateTaget() const
{
    return false;
}

String DesignElement::ToString()
{
    suic::String strName;
    Element* elem = CASTTOE(_element);

    if (elem)
    {
        if (elem->GetName().Empty())
        {
            strName = _U("[");
            strName += elem->GetRTTIType()->typeName;
            strName += _U("]");
        }
        else
        {
            strName = elem->GetName();
        }
    }
    else
    {
        strName = _U("Element");
    }

    if (!IsModified())
    {
        return strName;
    }
    else
    {
        return (strName + _U("*"));
    }
}

bool DesignElement::IsSingleValue()
{
    return false;
}

void DesignElement::SetValue(suic::Object* val)
{
    
}

suic::Object* DesignElement::GetValue()
{
    return _element;
}

suic::String DesignElement::GetSingleXml()
{
    return suic::String();
}

String DesignElement::GetElementName()
{
    return GetTypeName();
}

bool DesignElement::NeedNotifyChanged() const
{
    return true;
}

String DesignElement::GetChildXml(const String& offset)
{
    String strXml;

    for (int j = 0; j < GetCount(); ++j)
    {
        suic::String strElemXaml;
        DesignElement* xamlChild = GetXamlElement(j);

        strElemXaml = xamlChild->GetResXml(offset);
        strXml += strElemXaml; 
    }

    return strXml;
}

void DesignElement::GetElementSetterXml(const String& offset, int iCurrSerial, String& strProp, String& strChildProp)
{
    SetterCollectionNode* setterColl = GetSetterCollection();
    RTTIOfInfo* ownerType = setterColl->GetOwnerType();

    for (int i = 0; i < setterColl->GetCount(); ++i)
    {
        SetterNode* pSetterNode = setterColl->GetSetter(i);
        ResNode* resNode = pSetterNode->GetResNode();

        if (resNode->IsSingleValue())
        {
            if (resNode->GetValue() != NULL)
            {
                DpItem* dpItem = pSetterNode->GetDpItem();
                DpProperty* dp = pSetterNode->GetProperty();

                if (dp == suic::Element::NameProperty)
                {
                    suic::String tmpName = resNode->GetValue()->ToString();
                    tmpName.Trim();
                    if (tmpName.Empty())
                    {
                        continue;
                    }
                }

                if (resNode->GetValue() != DpProperty::UnsetValue())
                {
                    if (iCurrSerial >= 90)
                    {
                        iCurrSerial = 0;
                        strProp += _U("\n");
                        strProp += offset + ResNode::OFFSET1;
                    }

                    int iSerial = strProp.Length();

                    if (NULL == dp && dpItem == NULL)
                    {
                        strProp += pSetterNode->GetName();
                        strProp += _U("=\"");
                        strProp += pSetterNode->GetValue()->ToString();
                        strProp += _U("\" ");
                        iCurrSerial += strProp.Length() - iSerial;
                        continue;
                    }

                    if (NULL == dp)
                    {
                        dp = dpItem->dps.GetItem(0);
                    }

                    PropMetadata* propMeta = dp->GetMetadata(ownerType);

                    if (NULL == propMeta)
                    {
                        propMeta = dp->GetDefaultMetadata();
                    }

                    if (resNode->GetValue() != NULL && resNode->GetValue() != DpProperty::UnsetValue())
                        //if (!propMeta->GetDefaultValue()->Equals(resNode->GetValue()))
                    {
                        if (dpItem->name.Equals(_U("FontWeight")))
                        {
                            int ii = 0;
                        }
                        String strTmp = dpItem->ValueToString(resNode);
                        String strPropName = pSetterNode->GetName();
                        int iDotPos = strPropName.IndexOf(_U("."));

                        if (iDotPos == -1 && 
                            dp->IsAttached() && 
                            !ownerType->InheritFrom(dp->GetOwnerType()) && 
                            !ownerType->InheritFrom(propMeta->GetType()))
                        {
                            strProp += dp->GetOwnerType()->typeName;
                            strProp += _U(".");
                        }

                        strProp += strPropName;
                        strProp += _U("=\"");
                        strProp += strTmp;
                        strProp += _U("\" ");
                    }

                    iCurrSerial += strProp.Length() - iSerial;
                }
            }
        }
        else
        {
            String strChildPropName;
            String strTmp = resNode->GetResXml(offset + ResNode::OFFSET2);

            if (!strTmp.Empty())
            {
                strChildPropName = GetElementName();
                strChildPropName += _U(".");
                strChildPropName += pSetterNode->GetName();

                strChildProp += offset + ResNode::OFFSET1 + _U("<");
                strChildProp += strChildPropName;
                strChildProp += _U(">\n");
                strChildProp += strTmp;
                strChildProp += offset + ResNode::OFFSET1 + _U("</");
                strChildProp += strChildPropName;
                strChildProp += _U(">\n");
            }
        }
    }
}

String DesignElement::GetResXml(const String& offset)
{
    String strXml;
    String strChild;
    String strProp;
    String strRes;
    String strChildXml;
    String strChildProp;
    String strTypeName = GetElementName();

    int iCurrSerial = 0;

    if (strTypeName.Empty())
    {
        return strXml;
    }

    ResourceDictionaryNode* pResDic = GetResourceDictionary();
    SetterCollectionNode* setterColl = GetSetterCollection();
    RTTIOfInfo* ownerType = setterColl->GetOwnerType();

    strXml += offset + _U("<");
    strXml += strTypeName;

    if (!GetKey().Empty())
    {
        strXml += _U(" x:Key=\"");
        strXml += GetKey();
        strXml += _U("\"");
    }

    GetElementSetterXml(offset, iCurrSerial, strProp, strChildProp);

    if (pResDic->IsOnlyResItem())
    {
        strRes = pResDic->GetResItemsXml(offset + ResNode::OFFSET2);
    }
    else
    {
        strRes = pResDic->GetResXml(offset + ResNode::OFFSET2);
    }

    strChild += GetChildXml(offset + ResNode::OFFSET1);

    if (!strProp.Empty())
    {
        strXml += _U(" ");
        strXml += strProp;
    }

    if (strChildProp.Empty() && strChild.Empty() && strRes.Empty())
    {
        strXml += _U("/>\n");
    }
    else
    {
        strXml += _U(">\n");
        
        strXml += strChildProp;

        if (!strRes.Empty())
        {
            strXml += offset + ResNode::OFFSET1 + _U("<");
            strXml += GetElementName();
            strXml += _U(".Resources>\n");

            strXml += strRes;

            strXml += offset + ResNode::OFFSET1 + _U("</");
            strXml += GetElementName();
            strXml += _U(".Resources>\n");
        }

        strXml += strChild;

        strXml += offset + _U("</");
        strXml += GetElementName();
        strXml += _U(">\n");
    }

    return strXml;
}

DesignElement* DesignElement::GetXamlElement(int index)
{
    return RTTICast<DesignElement>(GetItem(index));
}

void DesignElement::SetNodeElement(suic::DpObject* elem)
{
    SETREFOBJ(_element, elem);
    DesignElement* pParent = GetParentXamlElement();
    if (pParent != NULL)
    {
        pParent->NotifyChildElementSet(this);
    }

    if (NULL != _element)
    {
        SetTypeInfo(_element->GetRTTIType());
    }
    else
    {
        SetTypeInfo(NULL);
    }
}

void DesignElement::NotifyChildElementSet(DesignElement* child)
{
}

suic::DpObject* DesignElement::GetNodeElement() const
{
    return _element;
}

suic::FrameworkElement* DesignElement::GetUIElement() const
{
    return CASTTOFE(_element);
}

DesignElement* DesignElement::GetParentXamlElement() const
{
    DesignElement* parent = suic::DynamicCast<DesignElement>(GetParent());
    return parent;
}

suic::FrameworkElement* DesignElement::GetParentUIElement() const
{
    DesignElement* parent = GetParentXamlElement();
    if (parent != NULL)
    {
        return parent->GetUIElement();
    }
    else
    {
        return NULL;
    }
}

void DesignElement::AddSetter(const String& name, DpProperty* dp, Object* val)
{
    _setterColl->AddSetter(new SetterNode(name, dp, val));
}

void DesignElement::AddResource(const String& key, Object* res)
{
    _resDic->AddResItem(key, res);
}

SetterCollectionNode* DesignElement::GetSetterCollection()
{
    return _setterColl;
}

void DesignElement::SetResourceDictionary(ResourceDictionaryNode* val)
{
    if (NULL != val)
    {
        SETREFOBJ(_resDic, val);
    }
    else
    {
        ResourceDictionaryNode* resDic = new ResourceDictionaryNode();
        SETREFOBJ(_resDic, resDic);
    }

    suic::FrameworkElement* fe = GetUIElement();

    if (NULL != fe)
    {
        fe->SetResources(_resDic->GetResourceDictionary());
    }
}

ResourceDictionaryNode* DesignElement::GetResourceDictionary()
{
    return _resDic;
}

void DesignElement::AddXamlChild(DesignElement* elem)
{
    elem->SetParent(this);
    elem->SetIsTemplate(IsTemplate());
}

void DesignElement::RemoveXamlChild(DesignElement* elem)
{
    elem->SetParent(NULL);
}

void DesignElement::OnFinishReadChildElement(DesignElement* child)
{

}

void DesignElement::AddXamlElement(DesignElement* elem)
{
    AddXamlChild(elem);
    AddItem(elem);
}

bool DesignElement::AddElement(DesignElement* elem)
{
    FrameworkElement* feElem = elem->GetUIElement();
    DpObject* dParent = GetUIElement()->GetTemplatedParent();

    if (OnAddElement(feElem))
    {
        AddXamlElement(elem);
        if (NULL != dParent && IsTemplate())
        {
            DesignHelper::SetTemplateParent(dParent, feElem);
        }
        return true;
    }
    else
    {
        return false;
    }
}

bool DesignElement::AddUIElement(FrameworkElement* elem)
{
    ContentControl* contCtrl = RTTICast<ContentControl>(GetUIElement());
    if (NULL != contCtrl)
    {
        Clear();
    }

    if (OnAddElement(elem))
    {
        return true;
    }
    else
    {
        return false;
    }
}

void DesignElement::SetElementAsTemplate(DesignElement* elem)
{
    Clear();

    if (NULL != elem)
    {
        AddXamlChild(elem);
        AddItem(elem);
    }
}

bool DesignElement::CreateElement(DesignElementPtr& elem, const String& strPath, const String& clsName)
{
    ElementRootItem rootElem;
    XamlLoader xamlLoader;
    rootElem.setAutoDelete(false);
    
    if (!xamlLoader.LoadElementRootXaml(&rootElem, strPath))
    {
        RTTIOfInfo* rttiInfo = suic::BuildFactory::Ins()->Lookup(clsName);
        if (rttiInfo != NULL)
        {
            suic::FrameworkElement* fe = RTTICast<suic::FrameworkElement>(rttiInfo->Create());
            if (NULL != fe)
            {
                DpProperty* dpHori = FrameworkElement::HorizontalAlignmentProperty;
                DpProperty* dpVert = FrameworkElement::VerticalAlignmentProperty;

                DesignElement* xamlElem = XamlWalker::CreateDesignElement(rttiInfo);
                rootElem.UpdateRootElement(xamlElem);
                SetterNode* pSetter = new SetterNode(dpHori->GetName(), dpHori, HoriAlignBox::LeftBox);

                xamlElem->GetSetterCollection()->AddSetter(pSetter);

                pSetter = new SetterNode(dpVert->GetName(), dpVert, VertAlignBox::TopBox);
                xamlElem->GetSetterCollection()->AddSetter(pSetter);

                fe->SetHorizontalAlignment(HoriAlign::haLeft);
                fe->SetVerticalAlignment(VertAlign::vaTop);

                xamlElem->SetNodeElement(fe);
            }
        }
    }
    
    if (rootElem.GetRootElement() != NULL && NULL != rootElem.GetUIElement())
    {
        elem = rootElem.GetRootElement();
        if (elem.get() != NULL)
        {
            elem->SetIsTemplate(IsTemplate());
        }
        return true;
    }
    else
    {
        return false;
    }
}

bool DesignElement::InsertElement(int index, DesignElement* elem)
{
    if (OnInsertElement(index, elem->GetUIElement()))
    {
        AddXamlChild(elem);
        InsertItem(index, elem);
        return true;
    }
    else
    {
        return false;
    }
}

bool DesignElement::MoveElement(DesignElement* elem, DesignElement* destination)
{
    if (OnMoveElement(elem->GetUIElement(), destination->GetUIElement()))
    {
        int oriIndex = IndexOf(elem);
        int dstIndex = IndexOf(destination);
        this->GetItemColl()->MoveItem(oriIndex, dstIndex);
        return true;
    }
    else
    {
        return false;
    }
}

bool DesignElement::RemoveElement(DesignElement* elem)
{
    if (OnRemoveElement(elem->GetUIElement()))
    {
        RemoveXamlChild(elem);
        RemoveItem(elem);
        return true;
    }
    else
    {
        return false;
    }
}

bool DesignElement::IsCanReplacePanel()
{
    suic::FrameworkElement* pParent = GetParentUIElement();
    suic::Panel* myPanel = RTTICast<suic::Panel>(GetUIElement());
    suic::ContentControl* pContent = RTTICast<suic::ContentControl>(pParent);
    suic::Panel* pPPanel = RTTICast<suic::Panel>(pParent);

    if (NULL != myPanel && (NULL != pContent || NULL != pPPanel))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool DesignElement::ReplacePanel(suic::Panel* pPanel)
{
    suic::FrameworkElement* pParent = GetParentUIElement();
    suic::Panel* myPanel = RTTICast<suic::Panel>(GetUIElement());
    suic::ContentControl* pContent = RTTICast<suic::ContentControl>(pParent);
    suic::Panel* pPPanel = RTTICast<suic::Panel>(pParent);

    DesignTemplateRoot* pTempRoot = dynamic_cast<DesignTemplateRoot*>(GetParent());

    if (NULL != myPanel && (NULL != pContent || NULL != pPPanel))
    {
        pPanel->GetChildren()->Clear();

        if (NULL != pTempRoot)
        {
            pTempRoot->ReplaceRootElement(pPanel);
        }
        else if (pContent != NULL)
        {
            pContent->SetContent(pPanel);
        }
        else if (pPPanel != NULL)
        {
            int index = pPPanel->GetChildren()->IndexOf(myPanel);
            pPPanel->GetChildren()->RemoveAt(index);
            pPPanel->GetChildren()->Insert(index, pPanel);
        }

        SetNodeElement(pPanel);
        NotifyChanged(_U("DisplayName"));

        for (int i = 0; i < myPanel->GetChildren()->GetCount(); ++i)
        {
            suic::Element* pElem = myPanel->GetChildren()->GetAt(i);
            if (myPanel->GetRTTIType()->InheritFrom(suic::HyperPanel::RTTIType()))
            {
                pElem->SetRenderTransform(NULL);
                pElem->SetOpacity(1.0f);
            }
            pPanel->AddChild(pElem);
        }

        for (int j = 0; j < _setterColl->GetCount(); ++j)
        {
            SetterNode* setterNode = _setterColl->GetSetter(j);
            if (NULL != setterNode && NULL != setterNode->GetProperty())
            {
                suic::DpProperty* dp = setterNode->GetProperty();
                if (dp->IsAttached() || 
                    pPanel->GetRTTIType()->InheritFrom(dp->GetOwnerType()))
                {
                    pPanel->SetValue(setterNode->GetProperty(), setterNode->GetValue());
                }
            }
        }

        pParent->UpdateLayout();
        return true;
    }
    else
    {
        return false;
    }
}

void DesignElement::CopyTo(DesignElement* Other)
{

}

void DesignElement::SwapElement(DesignElement* src, DesignElement* dest)
{
    int idxSrc = GetItemColl()->IndexOf(src);
    int idxDst = GetItemColl()->IndexOf(dest);
    suic::Panel* mvPanel = RTTICast<suic::Panel>(GetUIElement());

    GetItemColl()->SwapItem(idxSrc, idxDst);
    mvPanel->GetChildren()->Swap(src->GetUIElement(), dest->GetUIElement());
}

int DesignElement::IndexOf(DesignElement* pElem)
{
    int index = -1;

    for (int i = 0; i < GetCount(); ++i)
    {
        if (GetXamlElement(i) == pElem)
        {
            index = i;
            break;
        }
    }

    return index;
}

int DesignElement::IndexOf(suic::Point point, suic::Rect& rcElem)
{
    int index = -1;

    for (int i = 0; i < GetCount(); ++i)
    {
        DesignElement* pElem = GetXamlElement(i);
        suic::FrameworkElement* fe = pElem->GetUIElement();
        suic::Rect rect(fe->PointToScreen(suic::Point()), fe->GetRenderSize());

        if (rect.PointIn(point))
        {
            rcElem = rect;
            index = i;
            break;
        }
    }

    return index;
}

bool DesignElement::OnAddElement(suic::FrameworkElement* elem)
{
    IAddChild* pAddChild = dynamic_cast<IAddChild*>(GetUIElement());
    if (NULL != pAddChild)
    {
        pAddChild->AddChild(elem);
        return true;
    }
    else
    {
        return false;
    }
}

bool DesignElement::OnInsertElement(int index, suic::FrameworkElement* elem)
{
    suic::Panel* pPanel = RTTICast<suic::Panel>(GetUIElement());
    if (NULL != pPanel)
    {
        pPanel->GetChildren()->Insert(index, elem);
        return true;
    }
    return false;
}

bool DesignElement::OnMoveElement(suic::FrameworkElement* elem, suic::FrameworkElement* destination)
{
    suic::Panel* pPanel = RTTICast<suic::Panel>(GetUIElement());
    if (NULL != pPanel)
    {
        pPanel->GetChildren()->Move(elem, destination);
        return true;
    }
    return false;
}

bool DesignElement::OnRemoveElement(suic::FrameworkElement* elem)
{
    FrameworkElement* parent = GetUIElement();
    suic::IAddChild* pAddChild = dynamic_cast<suic::IAddChild*>(parent);
    if (NULL != pAddChild)
    {
        pAddChild->RemoveChild(elem);
        return true;
    }

    /*ContentControl* contCtrl = RTTICast<ContentControl>(parent);
    if (NULL != contCtrl)
    {
        contCtrl->SetContent(NULL);
        return true;
    }

    Panel* panelCtrl = RTTICast<Panel>(parent);
    if (NULL != panelCtrl)
    {
        panelCtrl->GetChildren()->Remove(elem);
        panelCtrl->NotifyItemChanged();
        return true;
    }

    ItemsControl* itemsCtrl = RTTICast<ItemsControl>(parent);
    if (NULL != itemsCtrl)
    {
        itemsCtrl->GetItemsSource()->RemoveItem(elem);
        return true;
    }
    
    suic::Decorator* pDecorator = RTTICast<suic::Decorator>(parent);
    if (NULL != pDecorator)
    {
        pDecorator->SetChild(NULL);
        return true;
    }

    suic::PlayBox* pPlayBox = RTTICast<suic::PlayBox>(parent);
    if (NULL != pPlayBox)
    {
        pPlayBox->GetChildCollection()->Remove(elem);
        return true;
    }*/
    
    return false;
}

void DesignElement::ClearChildren()
{
    for (int i = 0; i < GetCount(); ++i)
    {
        DesignElement* elem = GetXamlElement(i);
        if (OnRemoveElement(elem->GetUIElement()))
        {
            RemoveXamlChild(elem);
        }
    }
    XamlNodeItem::Clear();
}

void DesignElement::Clear()
{
    ClearChildren();

    _setterColl->Clear();
    _resDic->Clear();
}

void DesignElement::InitElementProps(bool bOnlyExt)
{
    bool hasStyleRef = false;
    FrameworkElement* fe = GetUIElement();
    SetterCollectionNode* pSetterColl = GetSetterCollection();

    for (int i = 0; i < pSetterColl->GetCount(); ++i)
    {
        SetterNode* pSetter = pSetterColl->GetSetter(i);
        suic::Object* objVal = pSetter->GetValue();

        if (pSetter->GetProperty() == FrameworkElement::StyleProperty)
        {
            ObjectPtr resVal;
            hasStyleRef = true;
            if (NULL == objVal)
            {
                StyleNode* pStyleNode = RTTICast<StyleNode>(pSetter->GetResNode());
                if (NULL != pStyleNode)
                {
                    pStyleNode->ParseStyleValue(NULL, fe, resVal);
                    if (NULL != resVal.get())
                    {
                        fe->SetValue(pSetter->GetProperty(), resVal.get());
                    }
                }
                return;
            }
        }
        else if (NULL == objVal)
        {
            ObjectPtr resVal;
            TemplateRootItem* tempRoot = RTTICast<TemplateRootItem>(pSetter->GetResNode());
            if (NULL != tempRoot)
            {
                tempRoot->ParseTemplateValue(NULL, fe, resVal);
                if (NULL != resVal.get())
                {
                    fe->SetValue(pSetter->GetProperty(), resVal.get());
                }
                return;
            }
        }

        if (NULL != objVal && DpProperty::UnsetValue() != objVal)
        {
            suic::Extension* pExt = RTTICast<suic::Extension>(objVal);
            if (NULL != pExt)
            {
                suic::ObjectPtr resVal;
                suic::SRExtension* pSrExt = RTTICast<suic::SRExtension>(pExt);
                if (NULL != pSrExt)
                {
                    resVal = fe->FindRes(pSrExt->GetResourceKey());
                }
                else
                {
                    pExt->ProvideValue(fe, pSetter->GetProperty(), resVal);
                }

                if (DpProperty::UnsetValue() != resVal.get())
                {
                    fe->SetValue(pSetter->GetProperty(), resVal.get());
                }
            }
            else if (!bOnlyExt)
            {
                if (pSetter->GetProperty() != NULL)
                {
                    fe->SetValue(pSetter->GetProperty(), objVal);
                }
                else
                {
                    suic::String strName = pSetter->GetName();
                    if (strName.Length() == 7 && strName.Equals(_U("Command")))
                    {
                        suic::ResourceParserOp::DoCommand(fe, objVal->ToString());
                    }
                    else
                    {
                        fe->SetAttribute(strName, objVal);
                    }
                }
            }
        }
    }

    if (!hasStyleRef)
    {
        //fe->SetStyle(NULL);
        fe->InvalidateStyle();
    }

    OnInstantiateDesignElement(fe);

    fe->UpdateLayout();
}

void DesignElement::RefreshDesignElement()
{
    // 初始化元素属性
    InitElementProps(true);

    // 创建子元素
    for (int i = 0; i < GetCount(); ++i)
    {
        DesignElement* pChild = GetXamlElement(i);
        pChild->RefreshDesignElement();
    }
}

void DesignElement::OnInstantiateDesignElement(suic::FrameworkElement* fe)
{

}

// 创建控件元素
void DesignElement::InstantiateDesignElement(DesignElement* parent, FrameworkElement* templatedParent, bool fromTempParent)
{
    FrameworkElement* fe = CASTTOFE(_typeInfo->Create());

    if (NULL == fe)
    {
        throw InvalidValueException(_U("cant create designelement"), __FILELINE__);
    }

    DesignHelper::SetInDesignMode(fe);
    SetNodeElement(fe);

    if (templatedParent != NULL)
    {
        DesignHelper::SetTemplateParent(templatedParent, fe);
    }

    // 初始化元素属性
    InitElementProps(false);

    if (NULL != parent)
    {
        if (!fromTempParent)
        {
            parent->OnAddElement(fe);
        }
        else
        {
            fe->RecursiveInit();
            templatedParent->SetTemplateChild(fe);
        }
    }

    // 创建子元素
    for (int i = 0; i < GetCount(); ++i)
    {
        DesignElement* pChild = GetXamlElement(i);
        pChild->InstantiateDesignElement(this, templatedParent, false);
    }
}

suic::Object* DesignElement::FindElemRes(const suic::String& resKey)
{
    suic::FrameworkElement* fe = CASTTOFE(GetUIElement());
    if (NULL == fe)
    {
        return NULL;
    }
    else
    {     
        return fe->FindRes(resKey);
    }
}

void DesignElement::UpdateElementProperty(DpProperty* dp, Object* newVal)
{
    if (IsLocked())
    {
        return;
    }

    bool bIsStyle = false;
    ObjectPtr unsetVal = newVal;
    suic::FrameworkElement* fe = GetUIElement();
    suic::Extension* expr = suic::RTTICast<suic::Extension>(newVal);

    // 表达式
    if (NULL != expr)
    {
        expr->ProvideValue(fe, dp, unsetVal);
    }
    else if (dp == suic::FrameworkElement::StyleProperty)
    {
        bIsStyle = true;
        if (NULL == newVal || suic::DpProperty::UnsetValue() == newVal)
        {
            fe->ClearValue(dp);
            fe->InvalidateStyle();
            return;
        }
    }

    if (newVal == NULL || newVal == DpProperty::UnsetValue())
    {
        if (!bIsStyle)
        {
            fe->ClearValue(dp);
            suic::StyleHelper::ApplyStyleValue(fe, dp, true);
            return;
        }
    }

    fe->SetEqualValue(dp, unsetVal.get());
}

//=====================================================
// XamlGridNode

DesignGrid::DesignGrid()
{
    _rowTmpDefs = NULL;
    _colTmpDefs = NULL;
}

DesignGrid::~DesignGrid()
{
}

suic::Grid* DesignGrid::GetOwner()
{
    suic::Grid* elem = RTTICast<suic::Grid>(GetUIElement());
    return elem;
}

void DesignGrid::SetRowMinHeight(int index, Float val)
{
    SetterCollectionNode* pRow = GetRow(index, true);
    if (NULL != pRow)
    {
        OFloat* oVal = new OFloat(val);
        SetterNode* setterNode = pRow->FindSetter(_U("MinHeight"));

        GetOwner()->GetRowDefs()->GetRowDef(index)->SetValue(RowDefinition::MinHeightProperty, oVal);

        if (NULL == setterNode)
        {
            setterNode = new SetterNode(_U("MinHeight"), suic::RowDefinition::MinHeightProperty, oVal);
            pRow->AddSetter(setterNode);
        }
        else
        {
            setterNode->SetValue(oVal);
        }
    }
}

void DesignGrid::SetRowMaxHeight(int index, Float val)
{
    SetterCollectionNode* pRow = GetRow(index, true);
    if (NULL != pRow)
    {
        OFloat* oVal = new OFloat(val);
        SetterNode* setterNode = pRow->FindSetter(_U("MaxHeight"));

        if (FloatUtil::GreaterThan(val, 0))
        {
            GetOwner()->GetRowDefs()->GetRowDef(index)->SetValue(RowDefinition::MaxHeightProperty, oVal);
        }
        else
        {
            GetOwner()->GetRowDefs()->GetRowDef(index)->ClearValue(RowDefinition::MaxHeightProperty);
        }

        if (NULL == setterNode)
        {
            setterNode = new SetterNode(_U("MaxHeight"), suic::RowDefinition::MaxHeightProperty, oVal);
            pRow->AddSetter(setterNode);
        }
        else
        {
            setterNode->SetValue(oVal);
        }
    }
}

void DesignGrid::SetRowHeight(int index, suic::Byte type, suic::Float val)
{
    SetterCollectionNode* pRow = GetRow(index, true);
    if (NULL != pRow)
    {
        suic::GridSize* oVal = new suic::GridSize(type, val);
        SetterNode* setterNode = pRow->FindSetter(_U("Height"));

        GetOwner()->GetRowDefs()->GetRowDef(index)->SetValue(RowDefinition::HeightProperty, oVal);

        if (NULL == setterNode)
        {
            setterNode = new SetterNode(_U("Height"), suic::RowDefinition::HeightProperty, oVal);
            pRow->AddSetter(setterNode);
        }
        else
        {
            setterNode->SetValue(oVal);
        }
    }
}

void DesignGrid::SetColumnMinWidth(int index, Float val)
{
    SetterCollectionNode* pCol = GetColumn(index, true);
    if (NULL != pCol)
    {
        suic::OFloat* oVal = new OFloat(val);
        SetterNode* setterNode = pCol->FindSetter(_U("MinWidth"));

        GetOwner()->GetColumnDefs()->GetColumnDef(index)->SetValue(ColumnDefinition::MinWidthProperty, oVal);

        if (NULL == setterNode)
        {
            setterNode = new SetterNode(_U("MinWidth"), suic::ColumnDefinition::MinWidthProperty, oVal);
            pCol->AddSetter(setterNode);
        }
        else
        {
            setterNode->SetValue(oVal);
        }
    }
}

void DesignGrid::SetColumnMaxWidth(int index, Float val)
{
    SetterCollectionNode* pCol = GetColumn(index, true);
    if (NULL != pCol)
    {
        suic::OFloat* oVal = new OFloat(val);
        SetterNode* setterNode = pCol->FindSetter(_U("MaxWidth"));

        if (FloatUtil::GreaterThan(val, 0))
        {
            GetOwner()->GetColumnDefs()->GetColumnDef(index)->SetValue(ColumnDefinition::MaxWidthProperty, oVal);
        }
        else
        {
            GetOwner()->GetColumnDefs()->GetColumnDef(index)->ClearValue(ColumnDefinition::MaxWidthProperty);
        }

        if (NULL == setterNode)
        {
            setterNode = new SetterNode(_U("MaxWidth"), suic::ColumnDefinition::MaxWidthProperty, oVal);
            pCol->AddSetter(setterNode);
        }
        else
        {
            setterNode->SetValue(oVal);
        }
    }
}

SetterCollectionNode* DesignGrid::GetRow(int index, bool bAdd)
{
    SetterCollectionNode* pRow = NULL;
    
    if (index >= 0 && index < _rowDefs.GetCount())
    {
        pRow = _rowDefs.GetItem(index);
    }
    else if (bAdd)
    {
        pRow = AddRow();
    }
    return pRow;
}

SetterCollectionNode* DesignGrid::GetColumn(int index, bool bAdd)
{
    SetterCollectionNode* pCol = NULL;
    
    if (index >= 0 && index < _colDefs.GetCount())
    {
        pCol = _colDefs.GetItem(index);
    }
    else if (bAdd)
    {
        pCol = AddColumn();
    }

    return pCol;
}

void DesignGrid::SetColumnWidth(int index, suic::Byte type, suic::Float val)
{
    SetterCollectionNode* pCol = GetColumn(index, true);
    if (NULL != pCol)
    {
        suic::GridSize* oVal = new suic::GridSize(type, val);
        SetterNode* setterNode = pCol->FindSetter(_U("Width"));

        GetOwner()->GetColumnDefs()->GetColumnDef(index)->SetValue(ColumnDefinition::WidthProperty, oVal);

        if (NULL == setterNode)
        {
            setterNode = new SetterNode(_U("Width"), suic::ColumnDefinition::WidthProperty, oVal);
            pCol->AddSetter(setterNode);
        }
        else
        {
            setterNode->SetValue(oVal);
        }
    }
}

SetterCollectionNode* DesignGrid::AddRow()
{
    SetterCollectionNode* elemNode = new SetterCollectionNode();
    if (_rowDefs.GetCount() == 0)
    {
        GetOwner()->GetRowDefs()->Clear();
    }

    _rowDefs.AddItem(elemNode);
    GetOwner()->GetRowDefs()->AddItem(new RowDefinition());
    return elemNode;
}

SetterCollectionNode* DesignGrid::AddColumn()
{
    SetterCollectionNode* elemNode = new SetterCollectionNode();
    if (_colDefs.GetCount() == 0)
    {
        GetOwner()->GetColumnDefs()->Clear();
    }

    _colDefs.AddItem(elemNode);
    GetOwner()->GetColumnDefs()->AddItem(new ColumnDefinition());
    return elemNode;
}

void DesignGrid::RemoveRowAt(int index)
{
    _rowDefs.RemoveItemAt(index);
    GetOwner()->GetRowDefs()->RemoveItemAt(index);
    if (_rowDefs.GetCount() == 0 && _colDefs.GetCount() == 0)
    {
        GetOwner()->GetColumnDefs()->Clear();
    }
}

void DesignGrid::RemoveColumnAt(int index)
{
    _colDefs.RemoveItemAt(index);
    GetOwner()->GetColumnDefs()->RemoveItemAt(index);
    if (_rowDefs.GetCount() == 0 && _colDefs.GetCount() == 0)
    {
        GetOwner()->GetRowDefs()->Clear();
    }
}

int DesignGrid::GetRowCount()
{
    return _rowDefs.GetCount();
}

SetterCollectionNode* DesignGrid::GetRow(int index)
{
    if (index >= 0 && index < _rowDefs.GetCount())
    {
        return _rowDefs.GetItem(index);
    }
    else
    {
        return NULL;
    }
}

int DesignGrid::GetColumnCount()
{
    return _colDefs.GetCount();
}

SetterCollectionNode* DesignGrid::GetColumn(int index)
{
    if (index >= 0 && index < _colDefs.GetCount())
    {
        return _colDefs.GetItem(index);
    }
    else
    {
        return NULL;
    }
}

void DesignGrid::AddXamlElement(DesignElement* elem)
{
    DesignElement* tempNode = RTTICast<DesignElement>(elem);
    if (NULL != tempNode && IsTemplate())
    {
        String name = elem->GetNodeName();
        if (name.IndexOf(_U(".RowDefinitions")) != -1)
        {
            _rowTmpDefs = tempNode;
            _rowTmpDefs->ref();
        }
        else if (name.IndexOf(_U(".ColumnDefinitions")) != -1)
        {
            _colTmpDefs = tempNode;
            _colTmpDefs->ref();
        }
        else
        {
            DesignElement::AddXamlElement(elem);
        }
    }
    else
    {
        DesignElement::AddXamlElement(elem);
    }
}

void DesignGrid::InitRowDefs()
{
    for (int i = 0; i < _rowDefs.GetCount(); ++i)
    {
        RowDefinition* pRowDef = new RowDefinition();
        SetterCollectionNode* pSetterColl = _rowDefs.GetItem(i);
        GetOwner()->GetRowDefs()->AddItem(pRowDef);
        for (int j = 0; j < pSetterColl->GetCount(); ++j)
        {
            SetterNode* pSetter = pSetterColl->GetSetter(j);
            if (NULL != pSetter->GetProperty())
            {
                pRowDef->SetValue(pSetter->GetProperty(), pSetter->GetValue());
            }
        }
    }
}

void DesignGrid::InitColumnDefs()
{
    for (int i = 0; i < _colDefs.GetCount(); ++i)
    {
        ColumnDefinition* pColDef = new ColumnDefinition();
        SetterCollectionNode* pSetterColl = _colDefs.GetItem(i);
        GetOwner()->GetColumnDefs()->AddItem(pColDef);
        for (int j = 0; j < pSetterColl->GetCount(); ++j)
        {
            SetterNode* pSetter = pSetterColl->GetSetter(j);
            if (NULL != pSetter->GetProperty())
            {
                pColDef->SetValue(pSetter->GetProperty(), pSetter->GetValue());
            }
        }
    }
}

void DesignGrid::OnInstantiateDesignElement(suic::FrameworkElement* fe)
{
    if (GetOwner() == fe)
    {
        GetOwner()->GetColumnDefs()->Clear();
        GetOwner()->GetRowDefs()->Clear();
        InitRowDefs();
        InitColumnDefs();
        
    }
}

void DesignGrid::OnFinishReadChildElement(DesignElement* child)
{
    if (NULL != _rowTmpDefs)
    {
        for (int i = 0; i < _rowTmpDefs->GetCount(); ++i)
        {
            _rowDefs.AddItem(_rowTmpDefs->GetXamlElement(i)->GetSetterCollection());
        }
        _rowTmpDefs->unref();
        _rowTmpDefs = NULL;
    }

    if (NULL != _colTmpDefs)
    {
        for (int i = 0; i < _colTmpDefs->GetCount(); ++i)
        {
            _colDefs.AddItem(_colTmpDefs->GetXamlElement(i)->GetSetterCollection());
        }
        _colTmpDefs->unref();
        _colTmpDefs = NULL;
    }
}

void DesignGrid::CloneNode(ResNodePtr& obj)
{
    DesignElement::CloneNode(obj);
    DesignGrid* pGrid = RTTICast<DesignGrid>(obj.get());

    for (int i = 0; i < _rowDefs.GetCount(); ++i)
    {
        SetterCollectionNode* pSetterColl = new SetterCollectionNode();
        pGrid->_rowDefs.AddItem(pSetterColl);
        _rowDefs.GetItem(i)->CloneSetterCollectionNode(pSetterColl);
    }

    for (int j = 0; j < _colDefs.GetCount(); ++j)
    {
        SetterCollectionNode* pSetterColl = new SetterCollectionNode();
        pGrid->_colDefs.AddItem(pSetterColl);
        _colDefs.GetItem(j)->CloneSetterCollectionNode(pSetterColl);
    }
}

String DesignGrid::GetGridSizeXml(SetterCollectionNode* setterColl)
{
    String strXml;
    for (int j = 0; j < setterColl->GetCount(); ++j)
    {
        String strProp;
        SetterNode* pSetter = setterColl->GetSetter(j);
        GridSize* gSize = RTTICast<GridSize>(pSetter->GetValue());

        if (NULL != gSize)
        {
            if (gSize->GetGridSizeType() == GridSizeType::Auto)
            {
                strProp.Format(_U(" %s=\"Auto\""), pSetter->GetName().c_str());
            }
            else if (gSize->GetGridSizeType() == GridSizeType::Star)
            {
                if (FloatUtil::GreaterThanOrClose(gSize->GetValue(), 1))
                {
                    strProp.Format(_U(" %s=\"%g*\""), pSetter->GetName().c_str(), gSize->GetValue());
                }
            }
            else if (FloatUtil::GreaterThanOrClose(gSize->GetValue(), 0))
            {
                strProp.Format(_U(" %s=\"%g\""), pSetter->GetName().c_str(), gSize->GetValue());
            }
        }
        else
        {
            Float iVal = pSetter->GetValue()->ToFloat();
            if (iVal >= 0 && !FloatUtil::IsPosInfinity(iVal))
            {
                strProp.Format(_U(" %s=\"%s\""), pSetter->GetName().c_str()
                    , pSetter->GetResNode()->GetSingleXml().c_str());
            }
        }

        strXml += strProp;
    }
    return strXml;
}

String DesignGrid::GetChildXml(const String& offset)
{
    int i = 0;
    String strXml;

    if (_colDefs.GetCount() > 0)
    {
        strXml += offset + _U("<Grid.ColumnDefinitions>\n");
        for (i = 0; i < _colDefs.GetCount(); ++i)
        {
            SetterCollectionNode* setterColl = _colDefs.GetItem(i);
            strXml += offset + ResNode::OFFSET1 + _U("<ColumnDefinition");
            strXml += GetGridSizeXml(setterColl);
            strXml += _U("/>\n");
        }
        strXml += offset + _U("</Grid.ColumnDefinitions>\n");
    }

    if (_rowDefs.GetCount() > 0)
    {
        strXml += offset + _U("<Grid.RowDefinitions>\n");
        for (i = 0; i < _rowDefs.GetCount(); ++i)
        {
            SetterCollectionNode* setterColl = _rowDefs.GetItem(i);
            strXml += offset + ResNode::OFFSET1 + _U("<RowDefinition");
            strXml += GetGridSizeXml(setterColl);
            strXml += _U("/>\n");
        }
        strXml += offset + _U("</Grid.RowDefinitions>\n");
    }

    strXml += DesignElement::GetChildXml(offset);
    return strXml;
}

//=====================================================
// XamlTrackNode

DesignTrack::DesignTrack()
{
    _drb = NULL;
    _irb = NULL;
    _thumb = NULL;
}

DesignTrack::~DesignTrack()
{
    FREEREFOBJ(_drb);
    FREEREFOBJ(_irb);
    FREEREFOBJ(_thumb);
}

void DesignTrack::CloneNode(ResNodePtr& obj)
{
    DesignElement::CloneNode(obj);
    DesignTrack* pTrack = RTTICast<DesignTrack>(obj.get());

    if (NULL != _drb)
    {
        ResNodePtr drbNode;
        _drb->CloneNode(drbNode);
        pTrack->SetDecreaseButton(RTTICast<DesignElement>(drbNode.get()));
    }

    if (NULL != _irb)
    {
        ResNodePtr irbNode;
        _irb->CloneNode(irbNode);
        pTrack->SetIncreaseButton(RTTICast<DesignElement>(irbNode.get()));
    }

    if (NULL != _thumb)
    {
        ResNodePtr thumbNode;
        _thumb->CloneNode(thumbNode);
        pTrack->SetThumb(RTTICast<DesignElement>(thumbNode.get()));
    }
}

void DesignTrack::SetThumb(DesignElement* elem)
{
    SETREFOBJ(_thumb, elem);
}

void DesignTrack::SetDecreaseButton(DesignElement* elem)
{
    SETREFOBJ(_drb, elem);
}

void DesignTrack::SetIncreaseButton(DesignElement* elem)
{
    SETREFOBJ(_irb, elem);
}

void DesignTrack::AddXamlElement(DesignElement* elem)
{
    DesignElement* tempNode = RTTICast<DesignElement>(elem);
    if (NULL != tempNode)
    {
        String name = tempNode->GetNodeName();
        if (name.IndexOf(_U(".DecreaseRepeatButton")) != -1)
        {
            _drb = tempNode;
            _drb->ref();
        }
        else if (name.IndexOf(_U(".IncreaseRepeatButton")) != -1)
        {
            _irb = tempNode;
            _irb->ref();
        }
        else if (name.Equals(_U("Track.Thumb")))
        {
            _thumb = tempNode;
            _thumb->ref();
        }
    }
}

void DesignTrack::OnFinishReadChildElement(DesignElement* child)
{
    if (child == _drb)
    {
        _drb = AddTrackChild(_drb);
    }
    if (_irb == child)
    {
        _irb = AddTrackChild(_irb);
    }
    if (_thumb == child)
    {
        _thumb = AddTrackChild(_thumb);
    }

    ClearChildren();
}

DesignElement* DesignTrack::AddTrackChild(DesignElement* pNode)
{
    DesignElement* elemNode = NULL;
    ItemCollection* itemColl = GetItemColl();
    if (pNode != NULL)
    {
        if (pNode->GetCount() == 1)
        {
            elemNode = RTTICast<DesignElement>(pNode->GetItem(0));
            itemColl->AddItem(elemNode);
            elemNode->ref();
        }

        pNode->unref();
    }
    
    return elemNode;
}

String DesignTrack::GetChildXml(const String& offset)
{
    String strXml;
    suic::String strElemXaml;

    if (NULL != _drb)
    {
        strXml += offset + _U("<Track.DecreaseRepeatButton>\n");
        strElemXaml = _drb->GetResXml(offset + ResNode::OFFSET1);
        strXml += strElemXaml; 
        strXml += offset + _U("</Track.DecreaseRepeatButton>\n");
    }

    if (NULL != _irb)
    {
        strXml += offset + _U("<Track.IncreaseRepeatButton>\n");
        strElemXaml = _irb->GetResXml(offset + ResNode::OFFSET1);
        strXml += strElemXaml; 
        strXml += offset + _U("</Track.IncreaseRepeatButton>\n");
    }

    if (NULL != _thumb)
    {
        strXml += offset + _U("<Track.Thumb>\n");
        strElemXaml = _thumb->GetResXml(offset + ResNode::OFFSET1);
        strXml += strElemXaml; 
        strXml += offset + _U("</Track.Thumb>\n");
    }
    
    return strXml;
}

//=========================================================
// XamlContentControl

XamlContentControl::XamlContentControl()
{
}

XamlContentControl::~XamlContentControl()
{
}

bool XamlContentControl::OnAddElement(suic::FrameworkElement* elem)
{
    suic::ContentControl* content = suic::RTTICast<suic::ContentControl>(elem);
    content->SetContent(elem);
    return true;
}

bool XamlContentControl::OnInsertElement(int index, suic::FrameworkElement* elem)
{
    OnAddElement(elem);
    return true;
}

bool XamlContentControl::OnRemoveElement(suic::FrameworkElement* elem)
{
    suic::ContentControl* content = suic::RTTICast<suic::ContentControl>(elem);
    if (content->GetContent() == elem)
    {
        content->SetContent(NULL);
    }
    return true;
}

XamlPanel::XamlPanel()
{
}

XamlPanel::~XamlPanel()
{
}

bool XamlPanel::OnAddElement(suic::FrameworkElement* elem)
{
    suic::Panel* panel = suic::RTTICast<suic::Panel>(elem);
    panel->AddChild(elem);
    return true;
}

bool XamlPanel::OnInsertElement(int index, suic::FrameworkElement* elem)
{
    suic::Panel* panel = suic::RTTICast<suic::Panel>(elem);
    return true;
}

bool XamlPanel::OnRemoveElement(suic::FrameworkElement* elem)
{
    suic::Panel* panel = suic::RTTICast<suic::Panel>(elem);
    return true;
}

UserDesignElement::UserDesignElement()
{

}

UserDesignElement::UserDesignElement(suic::IXamlNode* pNode)
{
    _holdItem.InitNode(pNode);
}

UserDesignElement::~UserDesignElement()
{
    Clear();
}

bool UserDesignElement::IsLocked() const
{
    return DesignElement::IsLocked();
}

suic::String UserDesignElement::GetTypeName()
{
    return _holdItem.GetName();
}

bool UserDesignElement::GetLockElement()
{
    return DesignElement::GetLockElement();
}

void UserDesignElement::SetLockElement(bool val)
{
    DesignElement::SetLockElement(val);
}

String UserDesignElement::ToString()
{
    suic::String strName;
    FrameworkElement* elem = GetUIElement();

    if (elem)
    {
        if (elem->GetName().Empty())
        {
            strName = _U("[");
            strName += _holdItem.GetName();
            strName += _U("]");
        }
        else
        {
            strName = elem->GetName();
        }
    }
    else
    {
        strName = _U("Element");
    }

    if (!IsModified())
    {
        return strName;
    }
    else
    {
        return (strName + _U("*"));
    }
}

void UserDesignElement::Clear()
{
    _holdItem.Clear();
}

String UserDesignElement::GetResXml(const String& offset)
{
    int iCurrSerial = 0;
    String strProp;
    String strChildProp;
    String strXml = "";
    String strItems = "";

    strXml += offset + _U("<") + _holdItem.GetName();
    GetElementSetterXml(offset, iCurrSerial, strProp, strChildProp);
    strXml += _U(" ") + strProp;

    strItems = _holdItem.GetChildResXml(offset + ResNode::OFFSET1);

    if (!strItems.Empty())
    {
        strXml += _U(">\n");
        strXml += strItems;
        strXml += offset + _U("</") + _holdItem.GetName() + _U(">");
    }
    else
    {
        strXml += _U("/>");
    }

    strXml += _U("\n");

    return strXml;
}


//====================================================
// BaseRedoUndo

BaseRedoUndo::BaseRedoUndo(RootItem* rootElem)
    : _rootElem(rootElem)
{

}

BaseRedoUndo::~BaseRedoUndo()
{

}

void BaseRedoUndo::UpdateLayout()
{
    SlnTreeManager* slnTree = GetSlnTreeManager();
    ObjTreeManager* objMana = _rootElem->GetObjTreeManager();

    if (NULL == objMana)
    {
        objMana = slnTree->GetObjTreeManager();
    }

    objMana->GetElementTree()->UpdateLayout();
    objMana->GetDesignPanel()->UpdateDElement();
    objMana->GetDesignPanel()->UpdateModified();
}

void BaseRedoUndo::SetFocusedElement(DesignElement* elem)
{
    SlnTreeManager* slnTree = GetSlnTreeManager();
    ObjTreeManager* objMana = _rootElem->GetObjTreeManager();

    if (NULL == objMana)
    {
        objMana = slnTree->GetObjTreeManager();
    }

    objMana->GetDesignPanel()->SetFocusedElement(elem, true);
}

SlnTreeManager* BaseRedoUndo::GetSlnTreeManager() const
{
    return _rootElem->GetProject()->GetSlnTreeManager();
}

RootItemData* BaseRedoUndo::GetRootItemData()
{
    return RTTICast<ElementRootItem>(_rootElem)->GetRootItemData();
}

AddElementRedoUndo::AddElementRedoUndo(RootItem* rootElem, DesignElement* pParent, DesignElement* pChild)
    : BaseRedoUndo(rootElem)
{
    _parent = pParent;
    _parent->ref();

    _child = pChild;
    _child->ref();
}

AddElementRedoUndo::~AddElementRedoUndo()
{
    _parent->unref();

    if (NULL != _child)
    {
        _child->unref();
    }
}

bool AddElementRedoUndo::Redo()
{
    bool bSucc = _parent->AddElement(_child);
    if (bSucc)
    {
        SetFocusedElement(_child);
        UpdateLayout();
    }
    return bSucc;
}

void AddElementRedoUndo::Undo()
{
    _parent->RemoveElement(_child);
    SetFocusedElement(_parent);
    UpdateLayout();    
}

AddContentRedoUndo::AddContentRedoUndo(RootItem* rootElem, DesignContent* pParent, DesignElement* pChild)
    : BaseRedoUndo(rootElem)
{
    _parent = pParent;
    _parent->ref();

    _child = pChild;
    _child->ref();

    _oldChild = pParent->GetContent();
    if (_oldChild != NULL)
    {
        _oldChild->ref();
    }
}

AddContentRedoUndo::~AddContentRedoUndo()
{
    _parent->unref();

    if (NULL != _child)
    {
        _child->unref();
    }

    if (NULL != _oldChild)
    {
        _oldChild->unref();
    }
}

bool AddContentRedoUndo::Redo()
{
    bool bSucc = _parent->AddElement(_child);
    if (bSucc)
    {
        SetFocusedElement(_child);
        UpdateLayout();
    }
    return bSucc;
}

void AddContentRedoUndo::Undo()
{
    DesignElement* fucused = _parent;

    _parent->RemoveElement(_child);

    if (NULL != _oldChild)
    {
        DesignElement* pChild = RTTICast<DesignElement>(_oldChild);
        if (NULL != pChild)
        {
            _parent->AddElement(pChild);
            fucused = pChild;
        }
        else
        {
            SetterNode* pSetterNode = RTTICast<SetterNode>(_oldChild);
            if (NULL != pSetterNode)
            {
                _parent->GetSetterCollection()->AddSetter(pSetterNode);
                _parent->UpdateElementProperty(pSetterNode->GetProperty(), pSetterNode->GetValue());
            }
        }
    }

    SetFocusedElement(fucused);
    UpdateLayout();    
}

DeleteElementRedoUndo::DeleteElementRedoUndo(RootItem* rootElem, DesignElement* pParent, DesignElement* pChild)
    : AddElementRedoUndo(rootElem, pParent, pChild)
{
}

DeleteElementRedoUndo::~DeleteElementRedoUndo()
{
}

void DeleteElementRedoUndo::HandleContentControl()
{
}

bool DeleteElementRedoUndo::Redo()
{
    bool bSucc = false;
    SlnTreeManager* slnTree = GetSlnTreeManager();
    ObjTreeManager* objMana = _rootElem->GetObjTreeManager();

    if (NULL == objMana)
    {
        objMana = slnTree->GetObjTreeManager();
    }

    DesignPanel* designPanel = objMana->GetDesignPanel();
    RootItemData* rootData = GetRootItemData();

    if (NULL != _child && _parent->RemoveElement(_child))
    {
        if (rootData->hot == _child)
        {
            designPanel->SetMouseOverElement(NULL, false);
        }

        if (rootData->ContainsSelected(_child))
        {
            designPanel->SetFocusedElement(_parent, true);
        }

        UpdateLayout();
        bSucc = true;
    }
    else if (NULL != _child)
    {
        _child->unref();
        _child = NULL;
    }
    return bSucc;
}

void DeleteElementRedoUndo::Undo()
{
    if (NULL != _child && _parent->AddElement(_child))
    {
        SetFocusedElement(_child);
        UpdateLayout();
    }
}

ReplacePanelRedoUndo::ReplacePanelRedoUndo(RootItem* rootElem, DesignElement* pElem, suic::Panel* pPanel)
    : BaseRedoUndo(rootElem)
{
    _elem = pElem;
    _oldPanel = RTTICast<suic::Panel>(pElem->GetUIElement());
    _newPanel = pPanel;

    _newPanel->ref();
    _oldPanel->ref();
    _elem->ref();
}

ReplacePanelRedoUndo::~ReplacePanelRedoUndo()
{
    _newPanel->unref();
    _oldPanel->unref();
    _elem->unref();
}

bool ReplacePanelRedoUndo::Redo()
{
    return _elem->ReplacePanel(_newPanel);
}

void ReplacePanelRedoUndo::Undo()
{
    _elem->ReplacePanel(_oldPanel);
}

SetElementContentRedoUndo::
SetElementContentRedoUndo(RootItem* rootElem, DesignElement* pElem, SetterNode* pSetter)
    : BaseRedoUndo(rootElem)
{
    _elem = pElem;
    _elem->ref();

    _setterNode = pSetter;
    _setterNode->ref();

    _content = NULL;

    _dp = pSetter->GetProperty();
    _oldVal = pSetter->GetOldValue();
    _newVal = pSetter->GetValue();

    if (pElem->GetCount() == 1)
    {
        _content = RTTICast<DesignElement>(pElem->GetXamlElement(0));
        _content->ref();
    }
}

SetElementContentRedoUndo::~SetElementContentRedoUndo()
{
    if (_content != NULL)
    {
        _content->ref();
    }

    if (NULL != _setterNode)
    {
        _setterNode->unref();
    }

    if (NULL != _rootElem)
    {
        _rootElem->ref();
    }

    if (NULL != _elem)
    {
        _elem->ref();
    }
}

bool SetElementContentRedoUndo::Redo()
{
    _elem->UpdateElementProperty(_dp, _newVal.get());
    _setterNode->SetValue(_newVal.get());
    _setterNode->UpdateOldValue(_oldVal.get());
    _elem->ClearChildren();

    RootItemData* rootData = GetRootItemData();
    DesignPanel* designPanel = GetSlnTreeManager()->GetObjTreeManager()->GetDesignPanel();

    if (rootData->hot == _content)
    {
        designPanel->SetMouseOverElement(NULL, false);
    }

    if (rootData->ContainsSelected(_content))
    {
        designPanel->SetFocusedElement(_elem, true);
    }

    UpdatePropertyWindow();
    UpdateLayout();

    return true;
}

void SetElementContentRedoUndo::Undo()
{
    if (_content != NULL)
    {
        _elem->AddElement(_content);
    }
    else
    {
        _elem->UpdateElementProperty(_dp, _oldVal.get());
    }

    _setterNode->SetValue(_oldVal.get());
    _setterNode->UpdateOldValue(_newVal.get());
    
    UpdatePropertyWindow();
    UpdateLayout();
}

void SetElementContentRedoUndo::UpdatePropertyWindow()
{
    DesignPanel* designPanel = GetSlnTreeManager()->GetObjTreeManager()->GetDesignPanel();
    designPanel->GetPropWindow()->UpdateSetter(_setterNode, true);
}

/*
bool AppRootElement::Save()
{
    String strXml;
    strXml += _U("<?xml version=\"1.0\" encoding=\"utf-8\"?>\n");

    if (NULL != _appNode)
    {
        strXml += _appNode->GetResXml(GetProject()->GetProjectName(), _U(""));

        FileStream fs;
        String strFile = GetFullPath();

        if (fs.Open(strFile))
        {
            fs.Write(strXml);
            fs.Close();
            SetModified(false);
        }
    }

    return false;
}


bool ResourceRootItem::Save()
{
    String strXml;
    strXml += _U("<?xml version=\"1.0\" encoding=\"utf-8\"?>\n");

    if (NULL != _resNode)
    {
        strXml += _resNode->GetResXml(_U(""));

        FileStream fs;
        String strFile = GetFullPath();

        suic::FileDir::DupCreateDir(strFile);

        if (fs.Open(strFile))
        {
            fs.Write(strXml);
            fs.Close();
            SetModified(false);
        }
    }

    return false;
}*/

