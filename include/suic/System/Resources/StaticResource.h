// ���ڿƼ���Ȩ���� 2010-2012
// 
// �ļ�����StaticResource.h
// ��  �ܣ���̬��Դ���á�
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2011-05-01
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
