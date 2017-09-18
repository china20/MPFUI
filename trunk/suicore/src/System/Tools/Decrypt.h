// 华勤科技版权所有 2010-2011
// 
// 文件名：Encry.h
// 功  能：加密算法文件。
// 
// 作  者：汪荣
// 时  间：2012-02-02
// 
// ============================================================================

#ifndef _UIDECRYPT_H_
#define _UIDECRYPT_H_

#include <System/Windows/Object.h>
#include <System/Types/String.h>
#include <System/Types/StreamDef.h>

class UIDecrypt
{
public:

    static int Decrypt(const suic::String& pwd, suic::String path, suic::String outFile);
    static int Decrypt(const suic::String& pwd, suic::Byte* data, int iSize, suic::ByteStream* outData);

protected:

    UIDecrypt();
};

#endif
