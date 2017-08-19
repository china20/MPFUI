// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����AcceleratorOp.h>
// ��  �ܣ���ݼ������ࡣ
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2010-07-02
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
