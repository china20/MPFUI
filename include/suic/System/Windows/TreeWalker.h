// 华勤科技版权所有 2010-2011
// 
// 文件名：TreeWalker.h
// 功  能：可视树层次管理
// 
// 作  者：Sharpui开发组
// 时  间：2010-08-31
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
