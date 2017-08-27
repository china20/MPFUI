// 华勤科技版权所有 2008-2022
// 
// 文件名：Environment.h
// 功  能：定义核心库环境类，用于获取屏幕参数、坐标、操作系统等信息。
// 
// 作  者：MPF开发组
// 时  间：2010-07-02
// 
// ============================================================================

#ifndef _UIENVIRONMENT_H_
#define _UIENVIRONMENT_H_

#include <System/Types/Const.h>

namespace suic 
{

class SUICORE_API Environment
{
public:

    static OSType GetOSType();
    static Size GetScreenClient();
    static Size GetScreenBound();
    static Time_t GetSystemTick();
    static Time_t GetSystemTime();
};

}

#endif
