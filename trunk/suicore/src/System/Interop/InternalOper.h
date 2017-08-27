// 华勤科技版权所有 2008-2022
// 
// 文件名：internaloper.h
// 功  能：封装内部的操作方法
// 
// 作  者：MPF开发组
// 时  间：2010-07-12
// 
// ============================================================================

#ifndef _UIINTERNALOPER_H_
#define _UIINTERNALOPER_H_

#include <System/Tools/Array.h>
#include <System/Windows/HwndObject.h>
#include <Framework/Controls/Window.h>

/*--------内部使用头文件--------*/
#include <System/Interop/InternalWindowOper.h>

namespace suic
{

void suiInternalClear();

void RecursiveLoaded(Element* sender);
void RecursiveUnloaded(Element* sender);
void RecursiveLoadedWithFocused(Element* sender, ElementPtr& focused);

}

#endif

