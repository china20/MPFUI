///
/// 文件名：ResourceDictionaryNode.h
///
/// 功  能：定义ResourceDictionary资源节点
///
/// 作  者：汪荣
///

#ifndef _RESOURCEDICTIONARYNODE_H
#define _RESOURCEDICTIONARYNODE_H

#include <Core/ResourceCollectionNode.h>
#include <Core/SetterCollectionNode.h>

class ElementRootItem;
class ResourceDicRootItem;
class MergedDictionariesNode;

class ResourceDictionaryNode : public XamlNodeItem
{
public:

    RTTIOfClass(ResourceDictionaryNode)

    ResourceDictionaryNode();
    ~ResourceDictionaryNode();

    void CloneNode(ResNodePtr& obj);
    void CloneResourceDic(ResourceDictionaryNode* pDicNode);

    String GetResXml(const String& offset);

    bool IsOnlyResItem();
    String GetResItemsXml(const String& offset);

    //ResourceUri* GetResPath();

    void ClearRes();
    void Clear();
    int GetCount() const;

    ItemCollection* GetResourceCollection() const;
    ResourceCollectionNode* GetResourceCollectionNode() const;

    void SetResourceDictionary(ResourceDictionary* res);
    ResourceDictionary* GetResourceDictionary() const;

    void SetSourceResource(ResourceDicRootItem* resSource);
    ResourceDicRootItem* GetSourceResource() const;

    void AddResItem(const String& key, Object* resItem);
    void AddMergedDictionary(ResourceDictionaryNode* resDic);

    ResNode* FindResItem(const String& key);
    void AddInternalresItem(ResNode* pResNode);
    ResNode* SearchResItem(const String& key);

    MergedDictionariesNode* GetMergedDictionariesNode();

    void FillResByResType(ItemCollection* itemColl, RTTIOfInfo* targetType, ResNode* breakNode);

    static bool ResInItemCollection(ItemCollection* itemColl, ResNode* resItem);

private:

    ResourceDictionary* _value;

    MergedDictionariesNode* _mergedDics;

    ResourceDicRootItem* _resSource;
    ResourceCollectionNode* _resItems;
};

class MergedDictionariesNode : public ResNode
{
public:

    RTTIOfClass(MergedDictionariesNode)

    MergedDictionariesNode();
    ~MergedDictionariesNode();

    virtual void CloneNode(ResNodePtr& obj);

    virtual bool IsSingleValue();
    virtual void SetValue(suic::Object* val);
    virtual suic::Object* GetValue();
    virtual suic::String GetSingleXml();

    String GetResXml(const String& offset);
    void AddMergedResource(ResourceDictionaryNode* mergedRes);
    ResNode* SearchResItem(const String& key);

    ResourceDictionaryNode* GetResourceDictionaryNode(int index);

    void FillResByResType(ItemCollection* itemColl, RTTIOfInfo* targetType, ResNode* breakNode);
};

class ApplicationNode : public XamlNodeItem
{
public:

    RTTIOfClass(ApplicationNode)

    ApplicationNode();
    ~ApplicationNode();

    String GetResXml(const String& resName, const String& offset);
    
    void SetApplication(Application* pApp);
    Application* GetApplication() const;

    ResourceUri GetStartupUri() const;

    void SetMainRootItem(ElementRootItem* rootItem);
    ElementRootItem* GetMainRootItem() const;

    SetterCollectionNode* GetSetterCollection();
    ResourceDictionaryNode* GetResourceDictionary();
    void SetResourceDictionary(ResourceDictionaryNode* val);

    void Close();
    void Clear();

private:

    Application* _value;
    ElementRootItem* _mainWnd;
    // 元素本身的属性（local值）
    SetterCollectionNode* _setterColl;
    // 元素资源
    ResourceDictionaryNode* _resDic;
};

#endif
