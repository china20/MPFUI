// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����StyleSelector.h
// ��  �ܣ�Ԫ����ʽ�ķ���ʵ�֡�
// 
// ��  �ߣ�����
// ʱ  �䣺2011-08-02
// 
// ============================================================================

#ifndef _UISTYLESELECTOR_H_
#define _UISTYLESELECTOR_H_

#include <System/Windows/DpObject.h>
#include <System/Resources/Style.h>

namespace suic
{

class SUICORE_API StyleSelector : public DpObject
{
public:

    RTTIOfAbstractClass(StyleSelector)

    virtual ~StyleSelector() {}
    virtual Style* SelectStyle(Object* item, FrameworkElement* container) = 0;
};

}

#endif
