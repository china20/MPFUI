// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����ControlTemplate.h
// ��  �ܣ�ʵ�ֿؼ�ģ�塣
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2011-08-02
// 
// ============================================================================

#ifndef _UICONTROLTEMPLATE_H_
#define _UICONTROLTEMPLATE_H_

#include <System/Windows/FrameworkTemplate.h>
#include <System/Resources/TriggerCollection.h>
#include <System/Resources/ResourceDictionary.h>

namespace suic 
{

class FrameworkElement;

class SUICORE_API ControlTemplate : public FrameworkTemplate
{
public:

    ControlTemplate();
    ControlTemplate(RTTIOfInfo* targetType);
    ~ControlTemplate();

    RTTIOfClass(ControlTemplate)

    void Seal();
    void ProcessVisualTriggers();

public:

    RTTIOfInfo* GetTargetType();
    void SetTargetType(RTTIOfInfo* targetType);
    TriggerCollection* GetTriggers();
    TriggerCollection* GetTriggersAndCreate();
    ResourceDictionary* GetResources();

protected:

    void ValidateTargetType(RTTIOfInfo* targetType, const String& argName);
    void AddContainerDependent(DpProperty* dp, bool fromTrigger, Array<ContainerDep>& deps);

    friend class FrameworkTemplate;

protected:

    RTTIOfInfo* _targetType; 
    TriggerCollection* _triggers;
    ResourceDictionary* _resources;

};

inline RTTIOfInfo* ControlTemplate::GetTargetType()
{
    return _targetType;
}

inline void ControlTemplate::SetTargetType(RTTIOfInfo* targetType)
{
    _targetType = targetType;
}

}

#endif
