///
/// 文件名：DictionaryNode.h
///
/// 功  能：Dictionary资源
///
/// 作  者：汪荣
///


#ifndef _DICTIONARYNODE_H
#define _DICTIONARYNODE_H

#include <Core/XamlSceneNode.h>

class DictionaryNode : public XamlNodeItem
{
public:

    DictionaryNode();

private:

    suic::XDictionary* _value;
};

#endif
