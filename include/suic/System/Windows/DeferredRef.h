// ���ڿƼ���Ȩ���� 2010-2012
// 
// �ļ�����DeferredRef.h
// ��  �ܣ��������ԵĻ��࣬�����������ʵ�ֵ��߼���
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2011-05-01
// 
// ============================================================================

#ifndef _UIDEFERREDREFERENCE_H_
#define _UIDEFERREDREFERENCE_H_

#include <System/Windows/EffectiveValue.h>

namespace suic
{

class DeferredRef : public Object
{
public:

    RTTIOfAbstractClass(DeferredRef)

    virtual ~DeferredRef() {}
    virtual Object* GetValue(BaseValueType valueSource) = 0;
};

}

#endif
