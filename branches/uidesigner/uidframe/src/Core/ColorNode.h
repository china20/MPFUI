///
/// 文件名：ColorNode.h
///
/// 功  能：Color资源
///
/// 作  者：汪荣
///

#ifndef _COLORNODE_H
#define _COLORNODE_H

#include <Core/XamlSceneNode.h>

class ColorNode : public XamlEditNode
{
public:

    ColorNode();

    Object* GetHostElem()
    {
        return _value;
    }

private:

    suic::OColor* _value;
};

#endif
