///
/// 文件名：SingleUndefined.h
///
/// 功  能：实现不可编辑资源节点
///
/// 作  者：汪荣
///

#ifndef _XAMLSINGLEUNDEFINED_H
#define _XAMLSINGLEUNDEFINED_H

#include <Core/HoldItem.h>
#include <Core/ResNode.h>

class SingleUndefined : public suic::Object
{
public:

    RTTIOfClass(SingleUndefined)

    SingleUndefined()
    {
    }

    SingleUndefined(const suic::String& name, const suic::String& val)
    {
        _name = name;
        _value = val;
    }

    suic::String GetName()
    {
        return _name;
    }

    suic::String GetValue()
    {
        return _value;
    }

private:

    suic::String _name;
    suic::String _value;
};

class NodeUndefined : public suic::Object
{
public:

    RTTIOfClass(NodeUndefined)

    NodeUndefined()
    {
    }

    NodeUndefined(suic::IXamlNode* pNode)
    {
        _holdItem.InitNode(pNode);
    }

    void CloneTo(NodeUndefined* obj);

    suic::String GetName()
    {
        return _holdItem.GetName();
    }

    String GetResXml(const String& offset)
    {
        return _holdItem.GetResXml(offset);
    }

private:

    HoldItem _holdItem;
};

class SingleUndefinedResNode : public ResNode
{
public:

    RTTIOfClass(SingleUndefinedResNode)

    SingleUndefinedResNode();
    SingleUndefinedResNode(SingleUndefined* val);

    virtual ~SingleUndefinedResNode();

    virtual void CloneNode(ResNodePtr& obj);
    virtual bool IsSingleValue();
    virtual void SetValue(suic::Object* val);
    virtual suic::Object* GetValue();

    virtual suic::String GetSingleXml();
    virtual suic::String GetResXml(const String& offset);

private:

    SingleUndefined* _value;
};

class NodeUndefinedResNode : public ResNode
{
public:

    RTTIOfClass(NodeUndefinedResNode)

    NodeUndefinedResNode();
    NodeUndefinedResNode(NodeUndefined* val);

    virtual ~NodeUndefinedResNode();

    virtual void CloneNode(ResNodePtr& obj);
    virtual bool IsSingleValue();
    virtual void SetValue(suic::Object* val);
    virtual suic::Object* GetValue();

    virtual suic::String GetSingleXml();
    virtual suic::String GetResXml(const String& offset);

private:

    NodeUndefined* _value;
};


#endif
