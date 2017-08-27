// 华勤科技版权所有 2008-2022
// 
// 文件名：AssessKeyManager.h
// 功  能：加速键。
// 
// 作  者：MPF开发组
// 时  间：2010-07-02
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
