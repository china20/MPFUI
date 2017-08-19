// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����TreeWalker.h
// ��  �ܣ���������ι���
// 
// ��  �ߣ�Sharpui������
// ʱ  �䣺2010-08-31
// 
// =============================================================

#ifndef _TREEWALKER_H_
#define _TREEWALKER_H_

#include <System/Windows/FrameworkElement.h>

namespace suic
{

class SUICORE_API TreeWalker
{
public:

    TreeWalker();

    void InvalidateOnResourcesChange(FrameworkElement* fe, ResourcesChangeInfo info);
    void InvalidateOnTreeChange(FrameworkElement* fe, DpObject* parent, bool isAddOperation);

private:

    void InvalidateResourceReferences(FrameworkElement* d, ResourcesChangeInfo info);
    void InvalidateStyleAndReferences(FrameworkElement* d, ResourcesChangeInfo info, bool containsTypeOfKey);
 

private:

    ;
    
};

}

#endif
