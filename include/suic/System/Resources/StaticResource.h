// 华勤科技版权所有 2010-2012
// 
// 文件名：StaticResource.h
// 功  能：动态资源引用。
// 
// 作  者：MPF开发组
// 时  间：2011-05-01
// 
// ============================================================================

#ifndef _UISTATICRESOURCEEXTENSION_H_
#define _UISTATICRESOURCEEXTENSION_H_

#include <System/Windows/Extension.h>

namespace suic
{

class SUICORE_API SRExtension : public Extension
{
public:

    RTTIOfClass(SRExtension)

    SRExtension();
    SRExtension(String resourceKey);
    ~SRExtension();

    String GetResourceKey() const;

    void ProvideValue(DpObject* d, DpProperty* dp, ObjectPtr& obj);

private:

    String _resourceKey;
};

inline String SRExtension::GetResourceKey() const
{
    return _resourceKey;
}

}

#endif
