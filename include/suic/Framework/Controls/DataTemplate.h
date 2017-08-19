// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����DataTemplate.h
// ��  �ܣ�ʵ������ģ��
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2010-08-31
// 
// =============================================================

#ifndef _UIDATATEMPLATE_
#define _UIDATATEMPLATE_

#include <System/Windows/FrameworkTemplate.h>

namespace suic
{

/// <summary>
/// DataTemplate,����ģ�档
/// </summary>
class SUICORE_API DataTemplate : public FrameworkTemplate
{
public:

    RTTIOfClass(DataTemplate)

    DataTemplate();
    DataTemplate(RTTIOfInfo* targetType);
    virtual ~DataTemplate();

    RTTIOfInfo* GetTargetType();
    void SetTargetType(RTTIOfInfo* targetType);

    TriggerCollection* GetTriggers();

protected:

    void ValidateTemplatedParent(FrameworkElement* templatedParent);
    void ReadDataTemplateTriggers(const TemplateReadInfo* info, IXamlNode* node);

    friend class FrameworkTemplate;

private:

    RTTIOfInfo* _dataType;
    TriggerCollection* _triggers;
};

inline RTTIOfInfo* DataTemplate::GetTargetType()
{
    return _dataType;
}

inline void DataTemplate::SetTargetType(RTTIOfInfo* targetType)
{
    _dataType = targetType;
}

class SUICORE_API DataTemplateSelector : public Object
{
public:

    RTTIOfAbstractClass(DataTemplateSelector)

    virtual ~DataTemplateSelector() {}
    virtual DataTemplate* SelectTemplate(Object* item, FrameworkElement* container) = 0;
};

class SUICORE_API DefaultTemplateSelector
{
public:

    static DataTemplate* SelectTemplate(Object* item, FrameworkElement* container);
};

}

#endif
