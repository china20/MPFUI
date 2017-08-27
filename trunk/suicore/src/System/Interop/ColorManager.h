// 华勤科技版权所有 2008-2022
// 
// 文件名：colormanager.h
// 功  能：颜色管理
// 
// 作  者：MPF开发组
// 时  间：2010-07-12
// 
// ============================================================================

#ifndef _UICOLORMANAGER_H_
#define _UICOLORMANAGER_H_

#include <System/Windows/FrameworkElement.h>
#include <System/Types/Interface.h>
#include <System/Tools/Array.h>
#include <System/Tools/Collection.h>

namespace suic
{

class ColorManager
{
public:

    static ColorManager* Ins();

    bool GetColor(const String& strClr, Uint32& clr);
    String ColorToString(Color clr);

protected:

    ColorManager();
    ~ColorManager();

    StringDic<Uint32> _mapColor;
};

}

#endif
