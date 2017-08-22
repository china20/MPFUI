
#include "stdafx.h"

#include <Core/StyleNode.h>
#include <main/Project.h>
#include <Editor/XamlWalker.h>

ImplementRTTIOfClass(StyleNode, ResNode)

StyleNode::StyleNode()
{
    _target = NULL;
    _setterCollNode = new SetterCollectionNode();
    _trgCollNode = new TriggerCollectionNode();

    _setterCollNode->ref();
    _trgCollNode->ref();
}

StyleNode::~StyleNode()
{
    _setterCollNode->unref();
    _trgCollNode->unref();
}

void StyleNode::ParseStyleValue(Project* pPrj, FrameworkElement* fe, ObjectPtr& resVal)
{
    suic::String strXml = GetResXml(_U(""));
    IXamlDoc* xamlDoc = XamlTools::CreateXamlDoc();
    if (NULL != xamlDoc)
    {
        XamlWalker walker(pPrj);
        IXamlNode* pRoot = NULL;
        suic::Mulstr strData(strXml.c_str());

        xamlDoc->LoadMemory((suic::Byte*)strData.c_str(), strData.Length());
        pRoot = xamlDoc->GetRootNode();
        if (NULL != pRoot)
        {
            ResourceItem resItem;
            walker.ReadStyleRes(fe, pRoot, resItem);
            resVal = RTTICast<suic::Style>(resItem.res.get());
        }
        xamlDoc->unref();
    }
}

SetterCollectionNode* StyleNode::GetSetterCollection()
{
    if (_setterCollNode->GetOwnerType() != _target)
    {
        _setterCollNode->SetRTTIOfInfo(_target);
    }
    return _setterCollNode;
}

TriggerCollectionNode* StyleNode::GetTriggerCollection()
{
    if (_trgCollNode->GetOwnerType() != _target)
    {
        _trgCollNode->SetRTTIOfInfo(_target);
    }
    return _trgCollNode;
}

void StyleNode::CloneNode(ResNodePtr& obj)
{
    StyleNode* pStyle = new StyleNode();

    obj = pStyle;

    pStyle->_target = _target;
    _setterCollNode->CloneSetterCollectionNode(pStyle->_setterCollNode);
    _trgCollNode->CloneTriggerCollectionNode(pStyle->_trgCollNode);
}

bool StyleNode::IsSingleValue()
{
    return false;
}

void StyleNode::SetValue(suic::Object* val)
{

}

suic::Object* StyleNode::GetValue()
{
    return NULL;
}

suic::String StyleNode::GetSingleXml()
{
    return String();
}

void StyleNode::SetTargetType(RTTIOfInfo* rttiInfo)
{
    _target = rttiInfo;
    CheckRTTIOfInfo();
}

void StyleNode::SetStyle(const suic::String& key, suic::Style* style)
{
    SetKey(key);

    if (NULL != style)
    {
        _target = style->GetTargetType();
    }

    CheckRTTIOfInfo();
}

bool StyleNode::HasContent() const
{
    return (_setterCollNode->GetCount() > 0 || _trgCollNode->GetCount() > 0);
}

void StyleNode::CheckRTTIOfInfo()
{
    if (_setterCollNode->GetOwnerType() != _target)
    {
        _setterCollNode->SetRTTIOfInfo(_target);
    }
    
    if (_trgCollNode->GetOwnerType() != _target)
    {
        _trgCollNode->SetRTTIOfInfo(_target);
    }
}

suic::String StyleNode::GetResXml(const String& offset)
{
    suic::String strXml;
    suic::String strSetters;
    suic::String strTriggers;
    suic::String name = _target != NULL ? _target->typeName : _targetType;

    if (name.Empty())
    {
        return strXml;
    }

    strXml = offset + _U("<Style TargetType=\"");
    strXml += name;
    strXml += _U("\"");
    if (!GetKey().Empty() && !GetKey().Equals(name))
    {
        strXml += _U(" x:Key=\"");
        strXml += GetKey();
        strXml += _U("\"");
    }
    strXml += _U(">\n");

    strSetters = GetSetterCollection()->GetResXml(offset + ResNode::OFFSET1);
    strTriggers = GetTriggerCollection()->GetResXml(offset + ResNode::OFFSET2);

    strXml += strSetters;

    if (!strTriggers.Empty())
    {
        strXml += offset + ResNode::OFFSET1 + _U("<Style.Triggers>\n");
        strXml += strTriggers;
        strXml += offset + ResNode::OFFSET1 + _U("</Style.Triggers>\n");
    }

    strXml += offset + _U("</Style>\n");

    return strXml;
}

void StyleNode::SetSetter(SetterNode* setter)
{
    _setterCollNode->AddSetter(setter);
}

void StyleNode::AddTrigger(TriggerBaseNode* trigger)
{
    _trgCollNode->AddTriggerNode(trigger);
    trigger->SetRTTIOfInfo(GetTargetType());
    AddItem(trigger);
}
