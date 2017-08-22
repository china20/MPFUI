// 华勤科技版权所有 2010-2011
// 
// 文件名：SystemResource.h
// 功  能：系统默认资源管理。
// 
// 作  者：MPF开发组
// 时  间：2011-08-02
// 
// ============================================================================

#ifndef _UISYSTEMRESOURCE_H_
#define _UISYSTEMRESOURCE_H_

#include <core/StyleNode.h>
#include <main/XamlRootItem.h>

class SystemResource
{
public:

    static SystemResource* Ins();

    ResourceDicRootItem* GetDefaultResDic();

    ResNode* FindResNode(const suic::String& strName);
    TemplateRootItem* FindControlTemplate(const suic::String& strType);
    StyleNode* FindStyle(const suic::String& strType);

    bool FindAndCloneStyle(const suic::String& strType, ResNodePtr& resNode);
    bool FindAndCloneControlTemplate(const suic::String& strType, ResNodePtr& resNode);

    bool InitDefaultResDic();

private:

    SystemResource();
    ~SystemResource();

    ResourceDicRootItem* _resRootItem;
};

#endif

