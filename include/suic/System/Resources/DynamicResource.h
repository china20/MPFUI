// 华勤科技版权所有 2010-2012
// 
// 文件名：DynamicResource.h
// 功  能：动态资源引用。
// 
// 作  者：MPF开发组
// 时  间：2011-05-01
// 
// ============================================================================

#ifndef _UIDYNAMICRESOURCEEXTENSION_H_
#define _UIDYNAMICRESOURCEEXTENSION_H_

#include <System/Windows/Extension.h>

namespace suic
{

class SUICORE_API DRExtension : public Extension
{
public:

    RTTIOfClass(DRExtension)

    DRExtension();
    DRExtension(String resourceKey);
    ~DRExtension();

    String GetResourceKey() const;
    void ProvideValue(DpObject* d, DpProperty* dp, ObjectPtr& obj);

private:

    String _resourceKey;
};

inline String DRExtension::GetResourceKey() const
{
    return _resourceKey;
}
}

#endif
