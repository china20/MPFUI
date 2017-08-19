// ���ڿƼ���Ȩ���� 2010-2012
// 
// �ļ�����DeferredResourceReference.h
// ��  �ܣ���Դ����ֵ��
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2011-05-01
// 
// ============================================================================

#ifndef _UIDEFERRESOURCEREDREFERENCE_H_
#define _UIDEFERRESOURCEREDREFERENCE_H_

#include <System/Windows/DeferredRef.h>

namespace suic
{

class ResourceDictionary;
class DeferredResRef : public DeferredRef
{
public:

    DeferredResRef(String key, Object* val);
    DeferredResRef(ResourceDictionary* dictionary, String key);
    ~DeferredResRef();

    Object* GetValue(BaseValueType valueSource);

private:

    ResourceDictionary* _dictionary;
    String _key;
    Object* _value;

};

}

#endif
