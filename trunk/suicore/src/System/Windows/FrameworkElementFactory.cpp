
#include <System/Windows/FrameworkElementFactory.h>
#include <System/Windows/FrameworkTemplate.h>
#include <System/Windows/BuildFactory.h>
#include <System/Windows/TemplateBindingExpression.h>
#include <System/Windows/ResourceReferenceExpression.h>

#include <System/Tools/StyleHelper.h>
#include <System/Tools/DesignHelper.h>
#include <System/Tools/ResourceParser.h>

#include <System/Interop/System.h>

#include <System/Resources/XamlNodeImp.h>
#include <System/Resources/DynamicResource.h>
#include <System/Resources/StaticResource.h>

#include <Framework/Controls/RangeBase.h>
#include <Framework/Controls/Grid.h>
#include <Framework/Controls/Track.h>
#include <Framework/Controls/GridView.h>
#include <Framework/Controls/Menu.h>
#include <Framework/Controls/GridViewRowPresenter.h>

#include <Framework/Presenter/ContentPresenter.h>

namespace suic 
{

FEFactory::FEFactory()
    : _type(NULL)
    , _firstChild(NULL)
    , _nextSibling(NULL)
    , _lastChild(NULL)
    , _parent(NULL)
    , _frameworkTemplate(NULL)
    , _childIndex(0)
    , _sealed(false)
{
}

FEFactory::FEFactory(RTTIOfInfo* type)
    : _type(type)
    , _firstChild(NULL)
    , _nextSibling(NULL)
    , _lastChild(NULL)
    , _parent(NULL)
    , _frameworkTemplate(NULL)
    , _childIndex(0)
    , _sealed(false)
{
}

FEFactory::FEFactory(String text)
    : _type(NULL)
    , _text(text)
    , _firstChild(NULL)
    , _nextSibling(NULL)
    , _lastChild(NULL)
    , _parent(NULL)
    , _frameworkTemplate(NULL)
    , _childIndex(0)
    , _sealed(false)
{
}

FEFactory::FEFactory(RTTIOfInfo* type, String name)
    : _type(type)
    , _text(name)
    , _firstChild(NULL)
    , _nextSibling(NULL)
    , _lastChild(NULL)
    , _parent(NULL)
    , _frameworkTemplate(NULL)
    , _childIndex(0)
    , _sealed(false)
{ 
}

FEFactory::~FEFactory()
{
    for (int i = 0; i < _propValues.Length(); ++i)
    {
        delete _propValues[i];
    }
    _propValues.Clear();
}

FEFactory* FEFactory::Create()
{
    return new FEFactory();
}

bool FEFactory::IsSealed() const
{
    return _sealed;
}

void FEFactory::SetType(RTTIOfInfo* type)
{ 
    if (IsSealed())
    { 
        throw;; 
    }

    _type = type;
}

void FEFactory::SetText(String text)
{ 
    if (IsSealed())
    {
        InvalidOperationException e(_T("FEFactory::SetText"), _T("CannotChangeAfterSealed"), __FILELINE__);
        throw e;
    }

    if (_firstChild != NULL)
    {
        InvalidOperationException e(_T("FEFactory::SetText"), _T("FEFactoryCannotAddText"), __FILELINE__);
        throw e;
    } 

    _text = text;
}

void FEFactory::SetName(String name)
{ 
    if (IsSealed())
    { 
        throw; 
    }

    _childName = name;
}

int FEFactory::GetPropCount() const
{
    return _propValues.GetCount();
}

PropValue* FEFactory::GetPropValue(int index) const
{
    return _propValues[index];
}

void FEFactory::AddChild(FEFactory* child)
{
    AppendChild(child);

    if (child->_type != NULL)
    {
        _frameworkTemplate->_templateChildCount++;
        child->_childIndex = _frameworkTemplate->_templateChildCount;
    }
    else
    {
        child->_childIndex = 0;
    }
}

void FEFactory::AppendChild(FEFactory* child)
{ 
    if (IsSealed()) 
    {
        throw; 
    }

    if (child == NULL)
    { 
        throw;
    } 

    if (child->_parent != NULL)
    { 
        throw;
    }

    if (_firstChild == NULL)
    {
        _firstChild = child;
        _lastChild = child; 
    }
    else 
    { 
        _lastChild->_nextSibling = child;
        _lastChild = child;
    }

    child->_parent = this;
    child->_frameworkTemplate = _frameworkTemplate;
} 

void FEFactory::UpdatePropertyValueList(DpProperty* dp, PropValType valueType, Object* value)
{
    int num = -1;

    if (dp != NULL)
    {
        for (int i = 0; i < _propValues.Length(); i++)
        {
            if (_propValues[i]->prop == dp)
            {
                num = i;
                break;
            }
        }

        if (num >= 0)
        {
            PropValue* propVal = _propValues[num];
            propVal->valueType = valueType;
            propVal->SetValue(value);
        }
        else
        {
            PropValue* propVal = new PropValue();

            propVal->valueType = valueType;
            propVal->childName = _U("~Self");
            propVal->prop = dp;
            propVal->SetValue(value);
            _propValues.Add(propVal);
        }
    }
    else
    {
        PropValue* propVal = new PropValue();
        propVal->valueType = valueType;
        propVal->childName = _U("~Command");
        propVal->prop = dp;
        propVal->SetValue(value);
        _propValues.Add(propVal);
    }
}

int FEFactory::SetValue(DpProperty* dp, Object* value)
{
#ifdef _DEBUG
    if (value == DpProperty::UnsetValue() || NULL == value)
    {
        throw InvalidValueException(_U("value is invalid for FEFactor SetValue"), __FILELINE__);
    }
#endif
    ObjectPtr obj = value;
    int iRet = PropValType::pvtSet;
    DRExtension* dyrExt = RTTIEqualCast<DRExtension>(value);
    SRExtension* strExt = RTTIEqualCast<SRExtension>(value);

    if (dyrExt != NULL)
    {
        iRet = PropValType::pvtResource;
    }
    else if (strExt != NULL)
    {
        iRet = PropValType::pvtTemplateBinding;
    }

    UpdatePropertyValueList(dp, (PropValType)iRet, value);

    return iRet;
}

bool FEFactory::IsDefaultValue(DpProperty* dp) const
{
    for (int i = 0; i < _propValues.Length(); i++)
    {
        if (_propValues[i]->prop == dp)
        {
            return true;
        }
    }

    return false;
}

void FEFactory::SetBinding(DpProperty* dp, Binding* binding) 
{
    SetValue(dp, binding);
}

void FEFactory::SetResourceReference(DpProperty* dp, Object* name)
{
    if (IsSealed())
    {
        throw InvalidOperationException(_U("CannotChangeAfterSealed"), __FILELINE__);
    }
    if (dp == NULL)
    {
        throw ArgumentNullException(_U("dp"), __FILELINE__);
    }

    UpdatePropertyValueList(dp, PropValType::pvtResource, name);
}

Object* FEFactory::GetValue(DpProperty* dp)
{ 
    Object* val = NULL;

    for (int i = 0; i < _propValues.Length(); i++)
    {
        PropValue* propVal = _propValues[i];
        if ((propVal->valueType == PropValType::pvtSet) && (propVal->prop == dp))
        {
            val = propVal->GetValue();
            break;
        }
    }

    if (val == NULL)
    {
        val = DpProperty::UnsetValue();
    }

    return val;
}

int FEFactory::GetPropValueCount() const
{
    return _propValues.GetCount();
}

PropValue* FEFactory::GetPropValue(int index)
{
    return _propValues.GetItem(index);
}

void FEFactory::ReadElementProperties(FrameworkElement* elem)
{
    for (int i = 0; i < _propValues.Length(); ++i)
    {
        DpObject* tfe = elem->GetTemplatedParent();
        PropValue* propVal = _propValues[i];
        ObjectPtr val = propVal->GetValue();
        Extension* expr(RTTICast<Extension>(val.get()));
        CommandProp* cmdProp = RTTIEqualCast<CommandProp>(val.get());

        if (cmdProp)
        {
            ResourceParserOp::DoCommand(elem, cmdProp->val);
        }
        else if (expr)
        {
            try
            {
                if (expr != NULL)
                {
                    ObjectPtr exprVal;
                    expr->ProvideValue(elem, propVal->prop, exprVal);
                    if (DpProperty::UnsetValue() != exprVal.get())
                    {
                        elem->SetValue(propVal->prop, exprVal.get());;
                    }
                }
            }
            catch (Exception& e)
            {
                e; // remove compiling warning
                elem->ClearValue(propVal->prop);
            }
        }
        else if (propVal->prop != NULL)
        {
            elem->SetValue(propVal->prop, val.get());
        }
    }
}

void FEFactory::BindCountTemplateElement(XArray* bindColl, DpObject* elem)
{
    if (NULL != bindColl)
    {
        bindColl->SetItem(_childIndex - 1, elem);
    }
}

void FEFactory::BeginInit(DpObject* container, DpObject* parent, FrameworkElement* elem)
{
    if (container != NULL)
    {
        if (container->InDesignMode())
        {
            DesignHelper::SetInDesignMode(elem);
        }
        elem->BeginInit();
    }

    elem->_templatedParent = container;

    if (container == parent)
    {
        FrameworkElement* templatedParent = RTTICast<FrameworkElement>(parent);
        if (NULL != templatedParent)
        {
            templatedParent->SetTemplateChild(elem);
        }
    }
    else if (NULL != parent)
    {
        IAddChild* pAddChild = DynamicCast<IAddChild>(parent);
        if (NULL == pAddChild)
        {
            throw InvalidOperationException(String().Format(_U("'%s' no implement interface IAddChild"), 
                parent->GetRTTIType()->typeName), __FILELINE__);
        }

        pAddChild->AddChild(elem);
    }
}

void FEFactory::EndInit(DpObject* container, FrameworkElement* elem)
{
    if (container != NULL)
    {
        elem->EndInit();
    }
}

void FEFactory::AutoDoProperties()
{
    if (_type->InheritFrom(ContentPresenter::RTTIType()))
    {
        Object* osr = GetValue(ContentPresenter::ContentSourceProperty);
        String prefix = (osr == DpProperty::UnsetValue()) ? _U("Content") : osr->ToString();

        if (prefix.Equals("ContentSource"))
            {
                int i = 0;
            }

        if (!prefix.Empty() && NULL != _frameworkTemplate)
        {
            RTTIOfInfo* targetType = _frameworkTemplate->GetTargetType();

            DpProperty* dpc = DpProperty::Lookup(prefix, targetType);
            DpProperty* dpct = DpProperty::Lookup(prefix + _U("Template"), targetType); 
            DpProperty* dpcts = DpProperty::Lookup(prefix + _U("TemplateSelector"), targetType);

            if (dpc != NULL && !IsDefaultValue(ContentPresenter::ContentProperty))
            {
                SetValue(ContentPresenter::ContentProperty, new TBExpression(new TemplateBindingExtension(dpc)));
                //SetValue(Element::DataContextProperty, new TemplateBindingExpression(new TemplateBindingExtension(Element::DataContextProperty)));
            }

            if (dpct != NULL && !IsDefaultValue(ContentPresenter::ContentTemplateProperty))
            {
                SetValue(ContentPresenter::ContentTemplateProperty, new TBExpression(new TemplateBindingExtension(dpct)));
            }

            if (dpcts != NULL && !IsDefaultValue(ContentPresenter::ContentTemplateSelectorProperty))
            {
                SetValue(ContentPresenter::ContentTemplateSelectorProperty, new TBExpression(new TemplateBindingExtension(dpcts)));
            }
        }
    }
    else if (_type == GridViewRowPresenter::RTTIType())
    {
        RTTIOfInfo* targetType = _frameworkTemplate->GetTargetType();
        DpProperty* dpc = DpProperty::Lookup(_U("Content"), targetType);
        DpProperty* dpcol = GridView::ColumnsProperty;//DpProperty::Lookup(_U("Columns"), targetType); 

        if (dpc != NULL && !IsDefaultValue(GridViewRowPresenter::ContentProperty))
        {
            SetValue(GridViewRowPresenter::ContentProperty, new TBExpression(new TemplateBindingExtension(dpc)));
        }

        if (dpcol != NULL && !IsDefaultValue(GridViewRowPresenterBase::ColumnsProperty))
        {
            SetValue(GridViewRowPresenterBase::ColumnsProperty, new TBExpression(new TemplateBindingExtension(dpcol)));
        }
    }
}

void FEFactory::MappingChildName(DpProperty* dp, const String& val)
{
    if (Element::NameProperty == dp && !IsSealed())
    {
        if (!val.Empty() && _frameworkTemplate)
        {
            ChildNameIndex nameIndex;
            nameIndex.index = _childIndex;
            nameIndex.typecls = _type;
            _frameworkTemplate->_childNameMappingIndex.Add(val, nameIndex);
        }
    }
}

void FEFactory::Seal()
{
    if (IsSealed())
    {
        return;
    }

    _sealed = true;

    if (_propValues.Length() > 0)
    {
        if (NULL == _frameworkTemplate->_trgDepRecords)
        {
            _frameworkTemplate->_trgDepRecords = new TriggerDepRecords();
        }

        TriggerDepRecord* trgDeps = _frameworkTemplate->GetTrgDepRecord(0);
        RTTIOfInfo* targetType = _frameworkTemplate->GetTargetType();

        if (NULL != trgDeps)
        {
            for (int i = 0; i < _propValues.Length(); ++i)
            {
                PropValue* propVal = _propValues[i];
                TBExpression* tempExpr = NULL;
                TemplateBindingExtension* temp = NULL;
                DRExtension* rrExpr = NULL;
                BindingBase* bind = NULL;

                temp = RTTIEqualCast<TemplateBindingExtension>(propVal->GetValue());

                if (NULL != temp)
                {
                    DepRecord* depRecords = trgDeps->AddDepRecord(temp->GetProp()->GetIndex());
                    ChildPropDep* propDep = new ChildPropDep();

                    propDep->Index = _childIndex;
                    propDep->Prop = propVal->prop;
                    depRecords->Add(propDep);

                    continue;
                }

                tempExpr = RTTIEqualCast<TBExpression>(propVal->GetValue());
                if (NULL != tempExpr)
                {
                    DepRecord* depRecords = trgDeps->AddDepRecord(tempExpr->GetProp()->GetIndex());
                    ChildPropDep* propDep = new ChildPropDep();

                    propDep->Index = _childIndex;
                    propDep->Prop = propVal->prop;
                    depRecords->Add(propDep);

                    continue;
                }

                rrExpr = RTTIEqualCast<DRExtension>(propVal->GetValue());

                if (NULL != rrExpr)
                {
                    ChildPropDep* propDep = new ChildPropDep();

                    propDep->Index = _childIndex;
                    propDep->Prop = propVal->prop;
                    propDep->Name = rrExpr->GetResourceKey();

                    _frameworkTemplate->GetResourceRecord()->Add(propDep);

                    continue;
                }

                bind = RTTICast<BindingBase>(propVal->GetValue());

                if (NULL != bind)
                {
                    continue;
                }
            }
        }
    }

    FEFactory* child = GetFirstChild();

    while (NULL != child)
    {
        child->Seal();
        child = child->GetNextSibling();
    }
}

void FEFactory::ClearChildren()
{
    FEFactory* child = GetFirstChild();
    FEFactory* delChild = NULL;

    while (NULL != child)
    {
        delChild = child;
        child->ClearChildren();
        child = child->GetNextSibling();

        delete delChild;
    }

    _firstChild = NULL;
    _nextSibling = NULL;
    _lastChild = NULL;
}

}
