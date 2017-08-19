// ���ڿƼ���Ȩ���� 2010-2012
// 
// �ļ�����DynamicResource.h
// ��  �ܣ���̬��Դ���á�
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2011-05-01
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
