// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����HashHelper.h
// ��  �ܣ�Hash�㷨�����ࡣ
// 
// ��  �ߣ�MPF������(����)
// ʱ  �䣺2010-07-02
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
