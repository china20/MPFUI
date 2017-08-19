// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����Template.h
// ��  �ܣ�ģ������װ��
// 
// ��  �ߣ�Sharpui������
// ʱ  �䣺2011-08-02
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
