
#include "stdafx.h"

#include <Main/XamlRootItem.h>

#include <Core/StyleNode.h>
#include <Core/ResourceDictionaryNode.h>

ImplementRTTIOfClass(ApplicationNode, XamlNodeItem)

ImplementRTTIOfClass(ResourceDictionaryNode, XamlNodeItem)
ImplementRTTIOfClass(MergedDictionariesNode, ResNode)

MergedDictionariesNode::MergedDictionariesNode()
{
}

MergedDictionariesNode::~MergedDictionariesNode()
{
}

void MergedDictionariesNode::CloneNode(ResNodePtr& obj)
{
    MergedDictionariesNode* pMergedDic = new MergedDictionariesNode();
    obj = pMergedDic;
    for (int i = 0; i < GetCount(); ++i)
    {
        ResNodePtr resDic;
        ResourceDictionaryNode* resNode = RTTICast<ResourceDictionaryNode>(GetItem(i));
        resNode->CloneNode(resDic);
        resNode = RTTICast<ResourceDictionaryNode>(resDic.get());
        pMergedDic->AddMergedResource(resNode);
    }
}

bool MergedDictionariesNode::IsSingleValue()
{
    return false;
}

void MergedDictionariesNode::SetValue(suic::Object* val)
{

}

suic::Object* MergedDictionariesNode::GetValue()
{
    return NULL;
}

suic::String MergedDictionariesNode::GetSingleXml()
{
    return suic::String();
}

String MergedDictionariesNode::GetResXml(const String& offset)
{
    String strXml;
    String strMergedXml;

    for (int i = 0; i < GetCount(); ++i)
    {
        String strTmp;
        ResourceDictionaryNode* resNode = RTTICast<ResourceDictionaryNode>(GetItem(i));
        strTmp = resNode->GetResXml(offset + ResNode::OFFSET1);

        if (!strTmp.Empty())
        {
            strMergedXml += strTmp;
        }
    }

    if (!strMergedXml.Empty())
    {
        strXml += offset + _U("<ResourceDictionary.MergedDictionaries>\n");
        strXml += strMergedXml;
        strXml += offset + _U("</ResourceDictionary.MergedDictionaries>\n");
    }

    return strXml;
}

void MergedDictionariesNode::AddMergedResource(ResourceDictionaryNode* mergedRes)
{
    AddItem(mergedRes);
}

ResourceDictionaryNode* MergedDictionariesNode::GetResourceDictionaryNode(int index)
{
    ResourceDictionaryNode* pResDic = RTTICast<ResourceDictionaryNode>(GetItem(index));
    return pResDic;
}

ResNode* MergedDictionariesNode::SearchResItem(const String& key)
{
    ResNode* resNode = NULL;
    for (int i = 0; i < GetCount(); ++i)
    {
        ResourceDictionaryNode* pResDic = RTTICast<ResourceDictionaryNode>(GetItem(i));
        resNode = pResDic->SearchResItem(key);
        if (NULL != resNode)
        {
            break;
        }
    }
    return resNode;
}

void MergedDictionariesNode::FillResByResType(ItemCollection* itemColl, RTTIOfInfo* targetType, ResNode* breakNode)
{
    for (int i = 0; i < GetCount(); ++i)
    {
        ResourceDictionaryNode* pResDic = RTTICast<ResourceDictionaryNode>(GetItem(i));
        pResDic->FillResByResType(itemColl, targetType, breakNode);
    }
}

//==============================================================================
// ResourceCollectionNode

ResourceCollectionNode::ResourceCollectionNode()
{
    _resItems = new ObservableCollection();
    _resItems->ref();
}

ResourceCollectionNode::~ResourceCollectionNode()
{
    _resItems->unref();
}

String ResourceCollectionNode::GetResXml(const String& offset)
{
    String strXml;

    for (int i = 0; i < _resItems->GetCount(); ++i)
    {
        ResNodePtr resNode = _resItems->GetItem(i);
        strXml += resNode->GetResXml(offset);
    }

    return strXml;
}

void ResourceCollectionNode::Clear()
{
    _resItems->Clear();
}

void ResourceCollectionNode::RemoveResItem(const String& key)
{
    for (int i = 0; i < _resItems->GetCount(); ++i)
    {
        ResNodePtr resNode = _resItems->GetItem(i);
        if (resNode->GetKey().Equals(key))
        {
            _resItems->RemoveItemAt(i);
            break;
        }
    }
}

ResNode* ResourceCollectionNode::FindResItem(const String& key)
{
    for (int i = 0; i < _resItems->GetCount(); ++i)
    {
        ResNodePtr resNode = _resItems->GetItem(i);
        if (resNode->GetKey().Equals(key))
        {
            return resNode.get();
        }
    }

    return NULL;
}

void ResourceCollectionNode::AddResItem(const String& key, Object* resItem)
{
    ResNodePtr pNode;
    ResNode::CreateResNode(resItem, pNode);
    pNode->SetKey(key);

    _resItems->AddItem(pNode.get());
}

void ResourceCollectionNode::AddResItem(ResNode* pResNode)
{
    _resItems->AddItem(pResNode);
}

void ResourceCollectionNode::AddBaseResItem(ResNode* pResNode)
{
    int index = -1;

    for (int i = 0; i < _resItems->GetCount(); ++i)
    {
        ResNode* pNode = GetResItem(i);
        if (pNode->GetRTTIType() == StyleNode::RTTIType() || 
            pNode->GetRTTIType() == TemplateRootItem::RTTIType())
        {
            index = i;
            break;
        }
    }

    if (index >= 0)
    {
        _resItems->InsertItem(index, pResNode);
    }
    else
    {
        _resItems->AddItem(pResNode);
    }
}

int ResourceCollectionNode::GetCount() const
{
    return _resItems->GetCount();
}

ResNode* ResourceCollectionNode::GetResItem(int index)
{
    ResNodePtr resNode = _resItems->GetItem(index);
    return resNode.get();
}

ResourceDictionaryNode::ResourceDictionaryNode()
{
    _value = NULL;
    _resSource = NULL;
    _mergedDics = NULL;
    _resItems = new ResourceCollectionNode();
    _mergedDics = NULL;
    _resItems->ref();
}

ResourceDictionaryNode::~ResourceDictionaryNode()
{
    FREEREFOBJ(_value);
    FREEREFOBJ(_resSource);
    FREEREFOBJ(_resItems);
    FREEREFOBJ(_mergedDics);
}

ResourceCollectionNode* ResourceDictionaryNode::GetResourceCollectionNode() const
{
    return _resItems;
}

void ResourceDictionaryNode::ClearRes()
{
    _resItems->Clear();
    FREEREFOBJ(_value);
    FREEREFOBJ(_resSource);
}

void ResourceDictionaryNode::Clear()
{
    _resItems->Clear();
    FREEREFOBJ(_value);
    FREEREFOBJ(_mergedDics);
    if (NULL != _resSource)
    {
        _resSource->Reset();
    }
}

int ResourceDictionaryNode::GetCount() const
{
    return _resItems->GetCount();
}

ItemCollection* ResourceDictionaryNode::GetResourceCollection() const
{
    return _resItems->GetResourceCollection();
}

void ResourceDictionaryNode::SetResourceDictionary(ResourceDictionary* res)
{
    SETREFOBJ(_value, res);
}

//void ResourceDictionaryNode::SetResourceSourceUri(const String& strSource)
//{
//    _resSourceUri.Parse(strSource);
//}
//
//ResourceUri ResourceDictionaryNode::GetResourceSourceUri() const
//{
//    return _resSourceUri;
//}

ResourceDictionary* ResourceDictionaryNode::GetResourceDictionary() const
{
    return _value;
}

void ResourceDictionaryNode::SetSourceResource(ResourceDicRootItem* resSource)
{
    SETREFOBJ(_resSource, resSource);
}

ResourceDicRootItem* ResourceDictionaryNode::GetSourceResource() const
{
    return _resSource;
}

void ResourceDictionaryNode::AddResItem(const String& key, Object* resItem)
{
    _resItems->AddResItem(key, resItem);
}

void ResourceDictionaryNode::AddMergedDictionary(ResourceDictionaryNode* resDic)
{
    if (_mergedDics == NULL)
    {
        _mergedDics = new MergedDictionariesNode();
        _mergedDics->ref();
        AddResItem(_U(""), _mergedDics);
    }
    _mergedDics->AddMergedResource(resDic);
}

ResNode* ResourceDictionaryNode::FindResItem(const String& key)
{
    return _resItems->FindResItem(key);
}

ResNode* ResourceDictionaryNode::SearchResItem(const String& key)
{
    ResNode* resNode = NULL;
    resNode = _resItems->FindResItem(key);
    if (NULL == resNode)
    {
        if (NULL != _resSource && NULL != _resSource->GetResourceDicNode())
        {
            resNode = _resSource->GetResourceDicNode()->SearchResItem(key);
        }
        if (NULL == resNode && NULL != _mergedDics)
        {
            resNode = _mergedDics->SearchResItem(key);
        }
    }
    return resNode;
}

void ResourceDictionaryNode::AddInternalresItem(ResNode* pResNode)
{
    if (pResNode->GetRTTIType() == StyleNode::RTTIType() || 
        pResNode->GetRTTIType() == TemplateRootItem::RTTIType())
    {
        _resItems->AddResItem(pResNode);
    }
    else
    {
        _resItems->AddBaseResItem(pResNode);
    }
}

bool ResourceDictionaryNode::ResInItemCollection(ItemCollection* itemColl, ResNode* resItem)
{
    bool bExist = false;
    for (int i = 0; i < itemColl->GetCount(); ++i)
    {
        ResNode* pItem = RTTICast<ResNode>(itemColl->GetItem(i));
        if (pItem != NULL && pItem->GetKey().Equals(resItem->GetKey()))
        {
            bExist = true;
            break;
        }
    }
    return bExist;
}

MergedDictionariesNode* ResourceDictionaryNode::GetMergedDictionariesNode()
{
    return _mergedDics;
}

void ResourceDictionaryNode::FillResByResType(ItemCollection* itemColl, RTTIOfInfo* targetType, ResNode* breakNode)
{
    if (_mergedDics != NULL)
    {
        _mergedDics->FillResByResType(itemColl, targetType, breakNode);
    }

    if (NULL != _resSource)
    {
        _resSource->GetResourceDicNode()->FillResByResType(itemColl, targetType, breakNode);
    }

    for (int j = 0; j < _resItems->GetCount(); ++j)
    {
        ResNode* pResItem = _resItems->GetResItem(j);

        if (pResItem == breakNode)
        {
            break;
        }

        RTTIOfInfo* rttiInfo = pResItem->GetValueRTTIType();

        if (NULL != rttiInfo)
        {
            if (rttiInfo->InheritFrom(targetType) && 
                !ResourceDictionaryNode::ResInItemCollection(itemColl, pResItem))
            {
                itemColl->AddItem(pResItem);
            }
        }
    }
}

bool ResourceDictionaryNode::IsOnlyResItem()
{
    if (_resSource == NULL && (_mergedDics == NULL || _mergedDics->GetCount() == 0))
    {
        return true;
    }
    else
    {
        return false;
    }
}

//ResourceUri* ResourceDictionaryNode::GetResPath()
//{
//    return (&_resSourceUri);
//}

String ResourceDictionaryNode::GetResItemsXml(const String& offset)
{
    String strXml;
    strXml = _resItems->GetResXml(offset);
    return strXml;
}

void ResourceDictionaryNode::CloneNode(ResNodePtr& obj)
{
    ResourceDictionaryNode* pDicNode = new ResourceDictionaryNode();
    obj = pDicNode;

    CloneResourceDic(pDicNode);
}

void ResourceDictionaryNode::CloneResourceDic(ResourceDictionaryNode* pDicNode)
{
    //pDicNode->_resSourceUri = _resSourceUri;
    SETREFOBJ(pDicNode->_value, _value);

    pDicNode->_mergedDics = NULL;

    SETREFOBJ(pDicNode->_resSource, _resSource);

    for (int i = 0; i < _resItems->GetCount(); ++i)
    {
        ResNodePtr resNode = _resItems->GetItem(i);
        ResNodePtr newNode;;

        resNode->CloneNode(newNode);

        MergedDictionariesNode* pMerged = RTTICast<MergedDictionariesNode>(newNode.get());
        if (pMerged != NULL)
        {
            pDicNode->_mergedDics = pMerged;
        }

        pDicNode->_resItems->AddResItem(newNode.get());
    }
}

String ResourceDictionaryNode::GetResXml(const String& offset)
{
    String strXml;
    String strChild;

    //strChild += _mergedDics->GetResXml(offset + ResNode::OFFSET1);
    strChild += _resItems->GetResXml(offset + ResNode::OFFSET1);

    if (_resSource != NULL)
    {
        ResourceUri resUri = _resSource->GetResourceUri();

        strXml += offset + _U("<ResourceDictionary Source=\"");
        strXml += resUri.ToString();

        if (strChild.Empty())
        {
            strXml += _U("\" />\n");
        }
        else
        {
            strXml += _U("\">\n");
            strXml += strChild;
            strXml += offset + _U("</ResourceDictionary>\n");
        }
    }
    else
    {
        if (!strChild.Empty())
        {
            strXml += offset + _U("<ResourceDictionary>\n");
            strXml += strChild;
            strXml += offset + _U("</ResourceDictionary>\n");
        }
    }

    return strXml;
}

ApplicationNode::ApplicationNode()
{
    _mainWnd = NULL;
    _value = NULL;
    _setterColl = new SetterCollectionNode();
    _resDic = new ResourceDictionaryNode();

    _resDic->ref();
    _setterColl->ref();
}

ApplicationNode::~ApplicationNode()
{
    FREEREFOBJ(_setterColl);
    FREEREFOBJ(_resDic);
    FREEREFOBJ(_value);
    FREEREFOBJ(_mainWnd);
}

String ApplicationNode::GetResXml(const String& resName, const String& offset)
{
    String strXml;
    String strRes;
    String strSetter;
    ResourceDictionaryNode* pResDic = GetResourceDictionary();

    strXml = _U("<Application StartupUri=\"");
    strXml += GetStartupUri().ToString();
    strXml += _U("\">\n");

    if (pResDic->IsOnlyResItem())
    {
        strRes = pResDic->GetResItemsXml(offset + ResNode::OFFSET2);
    }
    else
    {
        strRes = pResDic->GetResXml(offset + ResNode::OFFSET2);
    }

    if (!strRes.Empty())
    {
        strXml += offset + ResNode::OFFSET1 + _U("<Application.Resources>\n");
        strXml += strRes;
        strXml += offset + ResNode::OFFSET1;
        strXml += _U("</Application.Resources>\n");
    }

    strXml += _U("</Application>");

    return strXml;
}

void ApplicationNode::SetApplication(Application* pApp)
{
    SETREFOBJ(_value, pApp);
}

Application* ApplicationNode::GetApplication() const
{
    return _value;
}

ResourceUri ApplicationNode::GetStartupUri() const
{
    if (NULL != _mainWnd)
    {
        return _mainWnd->GetResourceUri();
    }
    return ResourceUri();
}

void ApplicationNode::SetMainRootItem(ElementRootItem* rootItem)
{
    SETREFOBJ(_mainWnd, rootItem);
}

ElementRootItem* ApplicationNode::GetMainRootItem() const
{
    return _mainWnd;
}

SetterCollectionNode* ApplicationNode::GetSetterCollection()
{
    return _setterColl;
}

ResourceDictionaryNode* ApplicationNode::GetResourceDictionary()
{
    return _resDic;
}

void ApplicationNode::SetResourceDictionary(ResourceDictionaryNode* val)
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

    if (NULL != _value)
    {
        _value->SetResources(_resDic->GetResourceDictionary());
    }
}

void ApplicationNode::Close()
{
}

void ApplicationNode::Clear()
{
    _setterColl->Clear();
    _resDic->Clear();
}
