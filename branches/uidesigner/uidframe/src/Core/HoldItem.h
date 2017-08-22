///
/// 文件名：HoldItem.h
///
/// 功  能：实现不可编辑资源节点
///
/// 作  者：汪荣
///

#ifndef _XAMLHOLDITEM_H
#define _XAMLHOLDITEM_H

#include <Core/SetterCollectionNode.h>
#include <Core/TriggerCollectionNode.h>
#include <Core/ResourceDictionaryNode.h>

typedef suic::KeyValuePair<suic::String, suic::String> StringPair;

class HoldItem
{
public:

    suic::String GetName()
    {
        return name;
    }

    void InitNode(suic::IXamlNode* pNode);
    void Clear();
    String GetResXml(const String& offset);
    String GetChildResXml(const String& offset);

    void CloneTo(HoldItem* Other);

private:

    void Clear(HoldItem* pHold);
    void InitNode(HoldItem* pHold, suic::IXamlNode* pNode);
    String GetResXml(HoldItem* pHold, const String& offset);

private:

    suic::String name;
    suic::Array<StringPair> attrs;
    suic::Array<HoldItem*> children;
};

#endif
