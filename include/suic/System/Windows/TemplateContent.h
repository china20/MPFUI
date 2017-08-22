// 华勤科技版权所有 2010-2011
// 
// 文件名：TemplateContent.h
// 功  能：管理模板内容，解析生成可视化树。
// 
// 作  者：Sharpui开发组
// 时  间：2011-08-02
// 
// ============================================================================

#ifndef _UITEMPLATECONTENT_H_
#define _UITEMPLATECONTENT_H_

#include <System/Windows/Element.h>
#include <System/Resources/ResourceDictionary.h>

namespace suic 
{

class FrameworkTemplate;

class SUICORE_API TemplateContent
{
public:

    TemplateContent();
    virtual ~TemplateContent();

    void SetTemplateString(String temp);

    bool BuildVisualTree(FrameworkElement* parent);

protected:

    String _template;
    friend class FrameworkTemplate;
};

}

#endif
