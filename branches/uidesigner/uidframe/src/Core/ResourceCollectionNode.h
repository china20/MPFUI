///
/// 文件名：ContentControlElement.h
///
/// 功  能：定义和Element对应的资源基类
///
/// 作  者：汪荣
///

#ifndef _RESOURCECOLLECTIONNODE_H
#define _RESOURCECOLLECTIONNODE_H

#include <Core/ResNode.h>
#include <Core/XamlSceneNode.h>

class ResourceCollectionNode : public ObservableCollection
{
public:

    ResourceCollectionNode();
    ~ResourceCollectionNode();

    String GetResXml(const String& offset);

    void Clear();
    void RemoveResItem(const String& key);

    ResNode* FindResItem(const String& key);

    void AddResItem(const String& key, Object* resItem);
    void AddResItem(ResNode* pResNode);

    void AddBaseResItem(ResNode* pResNode);

    int GetCount() const;
    ResNode* GetResItem(int index);

    ObservableCollection* GetResourceCollection() const
    {
        return _resItems;
    }

private:

    ObservableCollection* _resItems;
};

#endif
