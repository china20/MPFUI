// 华勤科技版权所有 2008-2022
// 
// 文件名：ViewBase.h
// 功  能：列表视图基类。
// 
// 作  者：MPF开发组
// 时  间：2010-11-02
// 
// ============================================================================

#ifndef _UIVIEWBASE_H_
#define _UIVIEWBASE_H_

#include <Framework/Controls/Control.h>

namespace suic
{

class SUICORE_API ViewBase : public DpObject
{
public:

    RTTIOfClass(ViewBase)

    static bool StaticInit();

    ViewBase();
    ~ViewBase();

public:

    virtual void LinkViewOwner(DpObject* owner);
    virtual void UnlinkViewOwner(DpObject* owner);
    virtual void PrepareItem(DpObject* item);
};

}

#endif
