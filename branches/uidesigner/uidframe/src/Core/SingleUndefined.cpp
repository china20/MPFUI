
#include "stdafx.h"

#include <Core/SingleUndefined.h>

ImplementRTTIOfClass(SingleUndefined, suic::Object)
ImplementRTTIOfClass(NodeUndefined, suic::Object)
ImplementRTTIOfClass(SingleUndefinedResNode, ResNode)
ImplementRTTIOfClass(NodeUndefinedResNode, ResNode)

void NodeUndefined::CloneTo(NodeUndefined* obj)
{
    obj->_holdItem.Clear();
    _holdItem.CloneTo(&(obj->_holdItem));
}

SingleUndefinedResNode::SingleUndefinedResNode()
{
    _value = NULL;
}

SingleUndefinedResNode::SingleUndefinedResNode(SingleUndefined* val)
{
    _value = val;
    if (NULL != _value)
    {
        _value->ref();
    }
}

SingleUndefinedResNode::~SingleUndefinedResNode()
{
    FREEREFOBJ(_value)
}

void SingleUndefinedResNode::CloneNode(ResNodePtr& obj)
{
}

bool SingleUndefinedResNode::IsSingleValue()
{
    return true;
}

void SingleUndefinedResNode::SetValue(suic::Object* val)
{
    if (NULL != val)
    {
        val->ref();
        SingleUndefined* pVal = suic::RTTICast<SingleUndefined>(val);
        SETREFOBJ(_value, pVal);
        val->unref();
    }
}

suic::Object* SingleUndefinedResNode::GetValue()
{
    return _value;
}

suic::String SingleUndefinedResNode::GetSingleXml()
{
    if (_value != NULL)
    {
        return _value->GetValue();
    }
    return suic::String();
}

suic::String SingleUndefinedResNode::GetResXml(const String& offset)
{
    suic::String strXml;
    return strXml;
}

//==================================================================
// NodeUndefinedResNode

NodeUndefinedResNode::NodeUndefinedResNode()
{
    _value = new NodeUndefined();
    _value->ref();
}

NodeUndefinedResNode::NodeUndefinedResNode(NodeUndefined* val)
{
    _value = val;
    _value->ref();
}

NodeUndefinedResNode::~NodeUndefinedResNode()
{
    FREEREFOBJ(_value)
}

void NodeUndefinedResNode::CloneNode(ResNodePtr& obj)
{
    if (NULL != _value)
    {
        NodeUndefinedResNode* pNode = new NodeUndefinedResNode();
        obj = pNode;
        _value->CloneTo(pNode->_value);
    }
}

bool NodeUndefinedResNode::IsSingleValue()
{
    return false;
}

void NodeUndefinedResNode::SetValue(suic::Object* val)
{
    if (NULL != val)
    {
        val->ref();
        NodeUndefined* pVal = suic::RTTICast<NodeUndefined>(val);
        SETREFOBJ(_value, pVal);
        val->unref();
    }
}

suic::Object* NodeUndefinedResNode::GetValue()
{
    return _value;
}

suic::String NodeUndefinedResNode::GetSingleXml()
{
    return suic::String();
}

suic::String NodeUndefinedResNode::GetResXml(const String& offset)
{
    if (NULL == _value)
    {
        return suic::String();
    }
    return _value->GetResXml(offset);
}
