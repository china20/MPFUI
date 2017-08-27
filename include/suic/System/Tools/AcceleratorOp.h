// 华勤科技版权所有 2008-2022
// 
// 文件名：AcceleratorOp.h>
// 功  能：快捷键帮助类。
// 
// 作  者：MPF开发组
// 时  间：2010-07-02
// 
// ============================================================================

#ifndef _UIACCELHELPER_H_
#define _UIACCELHELPER_H_

#include <System/Windows/Object.h>
#include <System/Windows/RenderTarget.h>
#include <System/Windows/Element.h>

namespace suic
{

class SUICORE_API AcceleratorOp
{
public:

    typedef Array<WeakRef> ArrayElement;

    static void Register(Uint16 modifier, const String& key, Element* elem);
    static void Unregister(Uint16 modifier, const String& key, Element* elem);

    static bool ProcessKeyDown(Element* rootElem, KeyboardEventArg* e);
    static bool ProcessTextInput(Element* rootElem, KeyboardEventArg* e);

protected:

    static String NormalizeKey(Uint16 modifier, const String& key);
    static bool PurgeDead(ArrayElement* elems, Element* elem);

protected:

    AcceleratorOp();
};

};

#endif
