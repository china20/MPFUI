// 华勤科技版权所有 2010-2011
// 
// 文件名：Encry.h
// 功  能：加密算法文件。
// 
// 作  者：汪荣
// 时  间：2012-02-02
// 
// ============================================================================

#ifndef _UIENCRYPT_H_
#define _UIENCRYPT_H_

#include <System/Windows/Object.h>
#include <System/Types/String.h>

class UIEncrypt
{
public:

    static int Encrypt(const suic::String& pwd, suic::String path);

protected:

    UIEncrypt();
};

#endif
