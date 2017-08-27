// 华勤科技版权所有 2008-2022
// 
// 文件名：HashHelper.h
// 功  能：Hash算法辅组类。
// 
// 作  者：MPF开发组(汪荣)
// 时  间：2010-07-02
// 
// ============================================================

#ifndef __UIHASHHELPER_H
#define __UIHASHHELPER_H

#include <Config/suicoreconfig.h>

namespace suic
{

class SUICORE_API HashHelper
{
public:

    static int GetPrime(int min);
    static int ExpandPrime(int oldSize);
};

}

#endif
