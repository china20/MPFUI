// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����AssessKeyManager.h
// ��  �ܣ����ټ���
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2010-07-02
// 
// ============================================================================

#ifndef _UIACCESSKEYMANAGER_H_
#define _UIACCESSKEYMANAGER_H_

#include <System/Types/Const.h>
#include <System/Windows/Object.h>

namespace suic 
{

class SUICORE_API AssessKeyManager
{
public:

    static void Register(String key, Element* element);
    static void Unregister(String key, Element* element);

protected:

    AssessKeyManager() {}
  
};

}

#endif
