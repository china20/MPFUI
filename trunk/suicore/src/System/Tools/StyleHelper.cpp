
#include <System/Tools/StyleHelper.h>
#include <System/Resources/Style.h>
#include <System/Windows/DependentList.h>
#include <System/Windows/EffectiveValue.h>
#include <System/Windows/MemberVisitor.h>

#include <Framework/Controls/Control.h>
#include <Framework/Presenter/ContentPresenter.h>

//#include <Framework/Controls/ListViewItem.h>

namespace suic
{

UncommonField<XArray>* StyleHelper::TemplatedBindField = NULL;

ImplementRTTIOfClass(IntAutoDicObj, suic::Object)

IntAutoDicObj::IntAutoDicObj()
{

}

IntAutoDicObj::~IntAutoDicObj()
{

}

void StyleHelper::StaticInit()
{
    if (NULL == TemplatedBindField)
    {
        TemplatedBindField = new UncommonField<XArray>(NULL);
    }
}

void StyleHelper::UpdateBindingSource(DpObject* source, DpProperty* sourceDp, DpObject* target, DpProperty* targetDp, Expression* expr, bool bAdd)
{
    Object* obj = DpObject::DependentDpField->GetValue(source);

    if (obj == NULL && bAdd)
    {
        obj = new IntAutoDicObj();
        DpObject::DependentDpField->SetValue(source, obj);
    }

    if (obj != NULL)
    {
        IntAutoDicObj* dpMap = RTTICast<IntAutoDicObj>(obj);
        if (NULL == dpMap)
        {
            throw InvalidValueException(_U("invalidate DependentDpField"), __FILELINE__);
        }

        Object* objBind = NULL;
        dpMap->TryGetValue(sourceDp->GetIndex(), objBind);

        DependentList* dpList = RTTICast<DependentList>(objBind);

        if (bAdd)
        {
            if (dpList == NULL)
            {
                dpList = new DependentList();
                dpMap->Add(sourceDp->GetIndex(), dpList);
            }

            dpList->Add(target, targetDp, expr);
        }
        else
        {
            if (dpList != NULL)
            {
                dpList->Remove(target, targetDp, expr);
            }
        }
    }
}

void StyleHelper::UpdateTemplateCache(FrameworkElement* fe, FrameworkTemplate* oldTemplate, FrameworkTemplate* newTemplate, DpProperty* prop)
{
    if (newTemplate != NULL && !newTemplate->IsSealed())
    {
        newTemplate->Seal();
    }

    fe->SetTemplateCache(newTemplate);
    DoTemplateInvalidations(fe, oldTemplate);
}

void StyleHelper::DoTemplateInvalidations(FrameworkElement* fe, FrameworkTemplate* oldTemplate)
{
    FrameworkTemplate* newTemplate = fe->GetTemplateInternal();

    if (newTemplate != oldTemplate)
    {
        //
        // 控件模版变化，清除其可视树
        //
        if (fe->ReadInnerFlag(InnerFlags::IsTemplateGenerated))
        {
            fe->SetTemplateChild(NULL);
            fe->WriteInnerFlag(InnerFlags::IsTemplateGenerated, false);
        }

        TemplatedBindField->ClearValue(fe);
        fe->OnTemplateChangedInternal(oldTemplate, newTemplate);
    }
}

void StyleHelper::UpdateStyleCache(FrameworkElement* fe, Style* oldStyle, Style* newStyle, Style*& styleCache)
{
    if (newStyle != NULL)
    {
        newStyle->CheckTargetType(fe);
        newStyle->Seal();
    }

    SETREFOBJ(styleCache, newStyle);
    DoStyleInvalidations(fe, oldStyle, newStyle);
}

void StyleHelper::UpdateTemplateInvalidations(FrameworkElement* fe)
{
    FrameworkTemplate* tfe = fe->GetTemplateInternal();

    if (NULL != tfe)
    {
        DicTriggerDependents exclusionDeps;
        DoTriggersInvalidations(fe, tfe, tfe->GetTriggers(), &exclusionDeps);
    }
}

void StyleHelper::DoTriggersInvalidations(FrameworkElement* fe, FrameworkTemplate* tfe, TriggerCollection* trgColl, DicTriggerDependents* exclusionDeps)
{
    XArray* childDep = StyleHelper::TemplatedBindField->GetValue(fe);

    if (NULL != trgColl)
    {
        for (int i = 0; i < trgColl->GetCount(); ++i)
        {
            TriggerBase* trgBase = trgColl->GetTrigger(i);
            if (trgBase->IsMatch(fe))
            {
                InvalidateTriggerOnProperty(childDep, exclusionDeps, fe, tfe, trgBase->GetSetters(), 0);
            }
        }
    }
}

void StyleHelper::DoStyleInvalidations(FrameworkElement* fe, Style* oldStyle, Style* newStyle)
{
    if (oldStyle != newStyle)
    {
        ContainerDeps* oldDeps = &EmptyArrayContainerDeps;
        ContainerDeps* newDeps = &EmptyArrayContainerDeps;
        DicTriggerDependents exclusionDeps;

        if (oldStyle)
        {
            oldDeps = oldStyle->GetElementDependents();
        }

        if (newStyle)
        {
            newDeps = newStyle->GetElementDependents();
        }

        InvalidateContainerDependents(fe, exclusionDeps, *oldDeps, *newDeps);
        DoStyleResourcesInvalidations(fe, oldStyle, newStyle);

        fe->OnStyleChanged(oldStyle, newStyle);
    }
}

void StyleHelper::InvalidateContainerDependents(FrameworkElement* fe, DicTriggerDependents& exclusionDeps, ContainerDeps& oldDeps, ContainerDeps& newDeps)
{
    int count = 0;
    FrameworkTemplate* tfe = fe->GetTemplateInternal();
    Style* newStyle = fe->GetStyle();
    DicTriggerDependents styleDeps;

    if (newStyle)
    {
        DoTriggersInvalidations(fe, NULL, newStyle->GetTriggers(), &exclusionDeps);
        if (newStyle->GetBasedOn())
        {
            DoTriggersInvalidations(fe, NULL, newStyle->GetBasedOn()->GetTriggers(), &exclusionDeps);
        }
    }

    if (NULL != tfe)
    {
        DoTriggersInvalidations(fe, tfe, tfe->GetTriggers(), &exclusionDeps);
    }

    if (NULL != newStyle)
    {
        SetterCollection* setters = newStyle->GetSetters();
        for (int i = 0; i < setters->GetCount(); ++i)
        {
            Setter* setter = setters->GetSetter(i);
            if (NULL != setter->GetProp() && 
                !IsSetOnTrigger(setter->GetProp(), 0, exclusionDeps))
            {
                TriggerDep elemDep(setter->GetProp(), 0);
                styleDeps.Add(elemDep.GetHashCode(), elemDep);
                InvalidateSetterOnProperty(fe, setter, BaseValueType::btStyle, OperationType::otUnknown);
            }
        }
    }

    /*DicTriggerDependents styleDeps;
    count = newDeps.Length();

    if (count > 0)
    {
        for (int j = 0; j < count; j++)
        {
            DpProperty* prop = newDeps.GetItem(j).prop;
            if (!IsSetOnTrigger(prop, 0, exclusionDeps))
            {
                ApplyStyleValue(fe, prop, true);
                TriggerDep elemDep(prop, 0);
                styleDeps.Add(elemDep.GetHashCode(), elemDep);
            }
        }
    }*/

    count = oldDeps.Length();
    for (int i = 0; i < count; i++)
    {
        DpProperty* dp = oldDeps.GetItem(i).prop;
        if (!IsSetOnTrigger(dp, 0, exclusionDeps) && 
            !IsSetOnTrigger(dp, 0, styleDeps))
        {
            //
            // 刷新之前Style中Trigger设置的值
            //
            fe->ClearValueFromStyle(dp);
        }
    }
}

void StyleHelper::
InvalidateTemplateTriggerDependents(FrameworkElement* fe, FrameworkTemplate* tfe, DpProperty* dp
                              , TriggerDepRecords* trgDeps, DpPropChangedEventArg* e, bool doOnlyContainer, int iSourceIndex)
{
    DicTriggerDependents exclusionDeps;
    // 刷新模版的TemplateBinding和ResourceDynamic资源（绑定和动态资源）
    InvalidateElementTemplateDependents(fe, dp, trgDeps, iSourceIndex, doOnlyContainer);
    InvalidateTriggerCollectionOnProperty(exclusionDeps, fe, tfe, dp, tfe->GetTriggers(), e, iSourceIndex);
}

void StyleHelper::
InvalidateStyleTriggerDependents(FrameworkElement* fe, Style* style, DpProperty* dp, TriggerDepRecords* trgDeps, DpPropChangedEventArg* e, bool doOnlyContainer)
{
    DicTriggerDependents exclusionDeps;
    InvalidateElementTemplateDependents(fe, dp, trgDeps, 0, doOnlyContainer);
    InvalidateTriggerCollectionOnProperty(exclusionDeps, fe, NULL, dp, style->GetTriggers(), e, 0);
    if (style->GetBasedOn())
    {
        InvalidateTriggerCollectionOnProperty(exclusionDeps, fe, NULL, dp, style->GetBasedOn()->GetTriggers(), e, 0);
    }
}

int StyleHelper::ChildIndexFromChildName(FrameworkTemplate* tfe, const String& childName)
{
    int index = 0;

    if (!childName.Empty() && tfe)
    {
        index = tfe->ChildIndexFromChildName(childName);
    }

    return index;
}

void StyleHelper::InvalidateSetterOnProperty(DpObject* fe, Setter* setter, BaseValueType bValueType, OperationType operType)
{
    DpProperty* feDp = setter->GetProp();
    EffectiveValue entry(feDp, bValueType);
    EntryIndex entryi = MemberVisitor::FindEntryIndex(fe, feDp->GetIndex());

    //
    // local值不允许设置（因为其优先级最高）
    //
    if (MemberVisitor::GetBaseValueSourceByEntryIndex(fe, entryi) != BaseValueType::btLocal)
    {
        MemberVisitor::SetValue(&entry, setter->GetValue());
        fe->UpdateEV(entryi, feDp, EffectiveValue(), entry, false, false, operType);
    }
    else if (bValueType == BaseValueType::btParentTemplateTrigger)
    {
        MemberVisitor::SetValue(&entry, setter->GetValue());
        fe->UpdateEV(entryi, feDp, EffectiveValue(), entry, false, true, OperationType::otForceUpdate);
    }
}

void StyleHelper::InvalidateTriggerOnProperty(XArray* childDep, DicTriggerDependents* exclusionDeps, FrameworkElement* fe, FrameworkTemplate* tfe, SetterCollection* setters, int iSourceIndex)
{
    if (setters != NULL)
    {
        for (int j = 0; j < setters->GetCount(); ++j)
        {
            BaseValueType bValueType = BaseValueType::btStyleTrigger;

            Setter* setter = setters->GetSetter(j);
            DpProperty* feDp = setter->GetProp();
            DpObject* child = fe;
            int index = 0;

            // 来自于模版
            if (tfe != NULL)
            {
                if (ShouldGetValueFromTemplate(feDp))
                {
                    String childName = setter->GetTargetName();

                    if (!childName.Empty())
                    {
                        index = tfe->ChildIndexFromChildName(childName);
                        if (index > 0 && NULL != childDep)
                        {
                            child = RTTICast<DpObject>(childDep->GetItem(index - 1));
                            bValueType = BaseValueType::btParentTemplateTrigger;
                        }
                        else
                        {
                            child = NULL;
                        }
                    }
                    else
                    {
                        bValueType = BaseValueType::btTemplateTrigger;
                    }
                }
                else
                {
                    continue;
                }
            }
            else if (!ShouldGetValueFromStyle(feDp))
            {
                continue;
            }

            if (NULL != child && feDp != NULL)
            {
                if (exclusionDeps != NULL)
                {
                    if (!IsSetOnTrigger(feDp, index, *exclusionDeps))
                    {
                        TriggerDep elemDep(feDp, index);
                        exclusionDeps->Add(elemDep.GetHashCode(), elemDep);
                    }
                    else
                    {
                        continue;
                    }
                }

                InvalidateSetterOnProperty(child, setter, bValueType, OperationType::otUnknown);
            }
        }
    }
}

void StyleHelper::InvalidateTriggerCollectionOnProperty(DicTriggerDependents& exclusionDeps, FrameworkElement* fe, FrameworkTemplate* tfe, DpProperty* dp, TriggerCollection* trgColl, DpPropChangedEventArg* e, int iSourceIndex)
{
    if (NULL == trgColl)
    {
        return;
    }

    int i = 0;
    bool bExistDp = false;

    for (i = 0; i < trgColl->GetCount(); ++i)
    {
        if (trgColl->GetTrigger(i)->CheckProperty(dp))
        {
            bExistDp = true;
            break;
        }
    }

    if (!bExistDp)
    {
        return;
    }

    TriggerDeps invDeps;

    //
    // 获取控件的模版子元素（一个数组，记录控件所有的子元素）
    //
    XArray* childDep = StyleHelper::TemplatedBindField->GetValue(fe);

    Object* oldValue = e->GetOldValue();
    Object* newValue = e->GetNewValue();

    //
    // trgColl为Trigger集合，可能是Style里定义的，也可能是ControlTemplate定义的
    //
    for (i = 0; i < trgColl->GetCount(); ++i)
    {
        TriggerBase* trigger = trgColl->GetTrigger(i);
        SetterCollection* setters = NULL;

        bool bOldEqual = false;
        bool bNewEqual = false;

        //
        // 根据dp匹配Trigger，如果dp为NULL，自动检测元素当前的值是否匹配
        //
        trigger->MatchValue(fe, dp, oldValue, newValue, bOldEqual, bNewEqual);
        setters = trigger->GetSetters();

        // 切换样式
        if (bNewEqual)
        {
            if (NULL != setters)
            {
                //
                // 刷新此Trigger下所有Setter值
                //
                InvalidateTriggerOnProperty(childDep, &exclusionDeps, fe, tfe, setters, iSourceIndex);
            }
        }

        // 恢复样式
        if (bOldEqual)
        {
            for (int j = 0; j < setters->GetCount(); ++j)
            {
                int index = 0;
                Setter* trgSetter = setters->GetSetter(j);
                DpProperty* feDp = trgSetter->GetProp();

                if (tfe != NULL)
                {
                    index = ChildIndexFromChildName(tfe, trgSetter->GetTargetName());
                }
                
                if (feDp != NULL && !IsSetOnTrigger(feDp, index, exclusionDeps))
                {
                    TriggerDep trgDep;

                    trgDep.prop = feDp;
                    trgDep.childIndex = index;
                    invDeps.Add(trgDep);
                }
            }
        }
        else
        {
            ;
        }
    }

    //---------------------------------------------------------

    for (i = 0; i < invDeps.Length(); ++i)
    {
        TriggerDep& trgDep = invDeps[i];

        if (!IsSetOnTrigger(trgDep.prop, trgDep.childIndex, exclusionDeps))
        {
            FrameworkElement* child = fe;

            if (trgDep.childIndex > 0 && NULL != childDep)
            {
                child = RTTICast<FrameworkElement>(childDep->GetItem(trgDep.childIndex - 1));
            }

            if (child != NULL)
            {
                Setter* setter = NULL;

                Style* style = child->GetStyle();
                SetterCollection* stySetters = NULL;

                if (NULL != style)
                {
                    stySetters = style->GetSetters();
                }

                if (NULL != stySetters)
                {
                    setter = stySetters->GetSetter(trgDep.prop);
                    if (setter == NULL && style->GetBasedOn() != NULL)
                    {
                        setter = style->GetBasedOn()->GetSetters()->GetSetter(trgDep.prop);
                    }
                }

                if (NULL != setter)
                {
                    InvalidateSetterOnProperty(child, setter, BaseValueType::btStyle, OperationType::otForceUpdate);
                }
                else
                {
                    child->InvalidateTrigger(trgDep.prop);
                }
            }
        }
    }
}

void StyleHelper::InvalidateElementTemplateDependents(FrameworkElement* fe, DpProperty* dp, TriggerDepRecords* trgDeps, int iSourceIndex, bool doOnlyContainer)
{
    if (NULL == trgDeps)
    {
        return;
    }

    //
    // 获取ControlTemplate的Trigger信息
    //
    TriggerDepRecord* trgDepRecord = trgDeps->GetTrgDepRecord(iSourceIndex);
    XArray* childDep = StyleHelper::TemplatedBindField->GetValue(fe);

    if (NULL != trgDepRecord && NULL != childDep)
    {
        //
        // 得到dp变化时需要改变的所有依赖属性值
        //
        DepRecord* depRecord = trgDepRecord->GetDepRecord(dp->GetIndex());

        if (NULL != depRecord)
        {
            for (int i = 0; i < depRecord->childPropDeps.GetCount(); i++)
            {
                DpObject* dpChild = NULL;
                ChildPropDep* cPropDep = depRecord->childPropDeps.GetItem(i);

                //
                // 如果Index小于等于0，则表示此依赖属性为控件本身值
                // 否则为其模版子元素
                //
                if (cPropDep->Index <= 0)
                {
                    dpChild = fe;
                }
                else if (!doOnlyContainer)
                {
                    dpChild = RTTICast<DpObject>(childDep->GetItem(cPropDep->Index - 1));
                }

                if (dpChild != NULL)
                {
                    dpChild->InvalidateProp(cPropDep->Prop);
                }
            }
        }
    }
}

void StyleHelper::InvalidateDependentResources(DpObject* d, DepRecord* depRecord)
{

}

void StyleHelper::DoStyleResourcesInvalidations(FrameworkElement* fe, Style* oldStyle, Style* newStyle)
{
    
}

bool StyleHelper::IsSetOnTrigger(DpProperty* dp, int index, DicTriggerDependents& elemDeps)
{
    Uint32 hashCode = ((index << 16) | (dp->GetIndex()));
    return elemDeps.Contains(hashCode);
}

bool StyleHelper::IsSetOnContainer(DpProperty* dp, ContainerDeps& elemDeps, bool fromTrigger)
{
    for (int i = 0; i < elemDeps.Length(); i++)
    {
        if (dp == elemDeps.GetItem(i).prop)
        {
            if (!fromTrigger)
            {
                return !elemDeps[i].fromTrigger;
            }
            return true;
        }
    }
    return false;
}

bool StyleHelper::ApplyStyleValue(FrameworkElement* fe, DpProperty* dp, bool bSelf)
{
    EffectiveValue entry(dp, 0);

    if (GetValueFromSelfStyle(fe, dp, entry))
    {
        EntryIndex entryi = fe->FindEntryIndex(dp->GetIndex());
        fe->UpdateEV(entryi, dp, EffectiveValue(), entry, false, false, OperationType::otUnknown);
        return true;
    }
    else
    {
        return false;
    }
}

bool StyleHelper::ApplySelfStyleValue(FrameworkElement* fe, DpProperty* dp)
{
    EffectiveValue entry(dp, DpProperty::UnsetValue(), 0);

    if (GetValueFromSelfStyle(fe, dp, entry))
    {
        EntryIndex entryi = fe->FindEntryIndex(dp->GetIndex());
        fe->UpdateEV(entryi, dp, EffectiveValue(), entry, false, false, OperationType::otUnknown);
        return true;
    }
    else
    {
        return false;
    }
}

bool StyleHelper::ShouldGetValueFromStyle(DpProperty* dp)
{
    return (dp != FrameworkElement::StyleProperty);
}

bool StyleHelper::ShouldGetValueFromTemplate(DpProperty* dp)
{
    /*return (dp != FrameworkElement::StyleProperty && 
        dp != Control::TemplateProperty && dp != ContentPresenter::TemplateProperty);*/
    return (dp != FrameworkElement::StyleProperty && 
        dp != Control::TemplateProperty/* && dp != ContentPresenter::ContentTemplateProperty*/);
}

bool StyleHelper::GetValueFromSelfStyle(FrameworkElement* fe, DpProperty* dp, EffectiveValue& entry)
{
    ValueLookupType simple = ValueLookupType::vltSimple;
    Object* unsetValue = DpProperty::UnsetValue();
    Style* style = fe->GetStyle();

    if ((style != NULL) && ShouldGetValueFromStyle(dp))
    {
        unsetValue = GetValueFromStyle(fe, style, dp, simple);
    }

    if (unsetValue != DpProperty::UnsetValue())
    {
        Extension* tempBind(RTTICast<Extension>(unsetValue));
        if (NULL != tempBind)
        {
            ObjectPtr bindVal;
            
            tempBind->ProvideValue(fe, dp, bindVal);
            MemberVisitor::SetValue(&entry, bindVal.get());
        }
        else
        {
            MemberVisitor::SetValue(&entry, unsetValue);
        }

        MemberVisitor::SetBaseValueType(&entry, BaseValueType::btStyle);

        return true;
    }
    else
    {
        return false;
    }
}

Object* StyleHelper::GetValueFromStyle(FrameworkElement* fe, Style* style, DpProperty* dp, ValueLookupType& vType)
{
    Object* dval = DpProperty::UnsetValue();
    PropValueArray* props = style->GetPropValues();

    for (int i = 0; i < props->Length(); ++i)
    {
        PropValue* propVal = (*props)[i];

        if (propVal->prop == dp)
        {
            vType = (ValueLookupType)propVal->valueType;
            return propVal->GetValue();
        }
    }

    if (NULL != style->GetBasedOn())
    {
        dval = GetValueFromStyle(fe, style->GetBasedOn(), dp, vType);
        if (dval != DpProperty::UnsetValue())
        {
            return dval;
        }
    }

    return dval;
}

}
