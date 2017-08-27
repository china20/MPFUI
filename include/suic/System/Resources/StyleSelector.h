// 华勤科技版权所有 2008-2022
// 
// 文件名：StyleSelector.h
// 功  能：元素样式的访问实现。
// 
// 作  者：汪荣
// 时  间：2011-08-02
// 
// ============================================================================

#ifndef _UISTYLESELECTOR_H_
#define _UISTYLESELECTOR_H_

#include <System/Windows/DpObject.h>
#include <System/Resources/Style.h>

namespace suic
{

class SUICORE_API StyleSelector : public DpObject
{
public:

    RTTIOfAbstractClass(StyleSelector)

    virtual ~StyleSelector() {}
    virtual Style* SelectStyle(Object* item, FrameworkElement* container) = 0;
};

}

#endif
