// 华勤科技版权所有 2010-2011
// 
// 文件名：Template.h
// 功  能：模板基类封装。
// 
// 作  者：Sharpui开发组
// 时  间：2011-08-02
// 
// ============================================================================

#ifndef _UITEMPLATE_H_
#define _UITEMPLATE_H_

#include <System/Types/Const.h>
#include <System/Windows/Object.h>

namespace suic 
{

class FrameworkElement;

class SUICORE_API Template : public Object
{
public:

    RTTIOfClass(Template)

    Template();
    ~Template();

    FrameworkElement* LoadTemplate(Object* parent);

    Template* Clone();

    void SetTemplate(FrameworkElement* content);
    FrameworkElement* GetTemplate();

    void SetSource(const String& source);
    String GetSource();

    String ToString();

protected:

    String _source;
    FrameworkElement* _template;
};

typedef shared<Template> TemplatePtr;

}

#endif
