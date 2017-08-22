///
/// 文件名：SolidColorBrushNode.h
///
/// 功  能：SolidColorBrush资源
///
/// 作  者：汪荣
///

#ifndef _SOLIDCOLORBRUSHNODE_H
#define _SOLIDCOLORBRUSHNODE_H

#include <Core/XamlSceneNode.h>

class SolidColorBrushNode : public XamlNodeItem
{
public:

    SolidColorBrushNode();

private:

    suic::SolidColorBrush* _value;
};

#endif

