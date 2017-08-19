// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����TemplateContent.h
// ��  �ܣ�����ģ�����ݣ��������ɿ��ӻ�����
// 
// ��  �ߣ�Sharpui������
// ʱ  �䣺2011-08-02
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
