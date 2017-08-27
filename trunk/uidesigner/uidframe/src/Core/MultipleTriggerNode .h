///
/// 文件名：MultipleTriggerNode.h
///
/// 功  能：MultipleTrigger资源
///
/// 作  者：汪荣
///

#ifndef _MULTIPLETRIGGERNODE_H
#define _MULTIPLETRIGGERNODE_H

#include <Core/XamlSceneNode.h>

class MultipleTriggerNode : public XamlNodeItem
{
public:

    MultipleTriggerNode();

private:

    suic::MultiTrigger* _value;
};

#endif


