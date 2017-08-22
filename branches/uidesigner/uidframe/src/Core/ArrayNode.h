///
/// 文件名：ArrayNode.h
///
/// 功  能：定义和Element对应的资源基类
///
/// 作  者：汪荣
///

#ifndef _ARRAYNODE_H
#define _ARRAYNODE_H

#include <Core/XamlSceneNode.h>

class ArrayNode : public XamlNodeItem
{
public:

    ArrayNode();

private:

    suic::XArray* _value;
};

#endif
