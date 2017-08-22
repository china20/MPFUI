// 华勤科技版权所有 2010-2011
// 
// 文件名：StyleHelper.h>
// 功  能：资源样式辅组类。
// 
// 作  者：MPF开发组
// 时  间：2010-07-02
// 
// ============================================================================

#ifndef _UISTYLEHELPER_H_
#define _UISTYLEHELPER_H_

#include <System/Windows/FrameworkElement.h>

namespace suic
{

class SUICORE_API StyleHelper
{
public:

    // FrameworkElement模板子元素个数
    static UncommonField<XArray>* TemplatedBindField;

    static bool StaticInit();

    static void UpdateTemplateInvalidations(FrameworkElement* fe);
    static void UpdateTemplateCache(FrameworkElement* fe, FrameworkTemplate* oldTemplate, FrameworkTemplate* newTemplate, DpProperty* prop);
    static void UpdateStyleCache(FrameworkElement* fe, Style* oldStyle, Style* newStyle, Style*& styleCache);
    static void InvalidateTriggerDependents(FrameworkElement* fe, DpProperty* dp, Object* val);
    static void InvalidateDependentResources(DpObject* d, DepRecord* depRecord);
    static void InvalidateElementTemplateDependents(FrameworkElement* fe, DpProperty* dp, TriggerDepRecords* trgDeps, int iSourceIndex, bool doOnlyContainer);

    static void InvalidateStyleTriggerDependents(FrameworkElement* fe, Style* style, DpProperty* dp, TriggerDepRecords* trgDeps, DpPropChangedEventArg* e, bool doOnlyContainer);
    static void InvalidateTemplateTriggerDependents(FrameworkElement* fe, FrameworkTemplate* tfe, DpProperty* dp, TriggerDepRecords* trgDeps, DpPropChangedEventArg* e, bool doOnlyContainer, int iSourceIndex);

    static void UpdateBindingSource(DpObject* source, DpProperty* sourceDp, DpObject* target, DpProperty* targetDp, Expression* expr, bool bAdd);

    static bool ApplyTemplateValue(FrameworkElement* fe, DpProperty* dp);
    static bool ApplyStyleValue(FrameworkElement* fe, DpProperty* dp, bool bSelf);
    static bool ApplySelfStyleValue(FrameworkElement* fe, DpProperty* dp);

protected:

    static void DoTemplateInvalidations(FrameworkElement* fe, FrameworkTemplate* oldTemplate);

    static bool ShouldGetValueFromStyle(DpProperty* dp);
    static bool ShouldGetValueFromTemplate(DpProperty* dp);

    static int ChildIndexFromChildName(FrameworkTemplate* tfe, const String& childName);

    static void InvalidateSetterOnProperty(DpObject* fe, Setter* setter, BaseValueType bValueType, OperationType operType);
    static void InvalidateTriggerOnProperty(XArray* childDep, DicTriggerDependents* exclusionDeps, FrameworkElement* fe, FrameworkTemplate* tfe, SetterCollection* setters, int iSourceIndex);
    static void InvalidateTriggerCollectionOnProperty(DicTriggerDependents& exclusionDeps, FrameworkElement* fe, FrameworkTemplate* tfe, DpProperty* dp, TriggerCollection* trgColl, DpPropChangedEventArg* e, int iSourceIndex);

    static bool IsSetOnTrigger(DpProperty* dp, int index, DicTriggerDependents& elemDeps);
    static bool IsSetOnContainer(DpProperty* dp, ContainerDeps& elemDeps, bool fromTrigger);
    static void DoStyleInvalidations(FrameworkElement* fe, Style* oldStyle, Style* newStyle);
    static void DoTriggersInvalidations(FrameworkElement* fe, FrameworkTemplate* tfe, TriggerCollection* trgColl, DicTriggerDependents* exclusionDeps);

    static bool GetValueFromSelfStyle(FrameworkElement* fe, DpProperty* dp, EffectiveValue& entry);

    static void InvalidateContainerDependents(FrameworkElement* fe, DicTriggerDependents& exclusionDeps, ContainerDeps& oldDeps, ContainerDeps& newDeps);
    static void DoStyleResourcesInvalidations(FrameworkElement* fe, Style* oldStyle, Style* newStyle);

    static Object* GetValueFromStyle(FrameworkElement* fe, Style* style, DpProperty* dp, ValueLookupType& vType);

    friend class MemberVisitor;

private:

    StyleHelper() {}
};

}

#endif
