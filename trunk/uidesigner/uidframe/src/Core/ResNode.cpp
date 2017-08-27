
#include "stdafx.h"

#include <Core/ResNode.h>
#include <Core/TypeManager.h>
#include <Core/XamlElementNode.h>
#include <Core/SingleUndefined.h>

#include <System/Resources/StaticResource.h>
#include <System/Resources/DynamicResource.h>

ImplementRTTIOfClass(ResNode, XamlNodeItem)

ImplementRTTIOfClass(GridViewResNode, ResNode)
ImplementRTTIOfClass(NullResNode, ResNode)
ImplementRTTIOfClass(ResValueNode, ResNode)
ImplementRTTIOfClass(SingleResNode, ResNode)
ImplementRTTIOfClass(StringResNode, SingleResNode)

ImplementRTTIOfClass(IntegerResNode, SingleResNode)
ImplementRTTIOfClass(WHIntegerResNode, SingleResNode)
ImplementRTTIOfClass(DoubleResNode, SingleResNode)
ImplementRTTIOfClass(BooleanResNode, SingleResNode)
ImplementRTTIOfClass(RectResNode, SingleResNode)
ImplementRTTIOfClass(SizeResNode, SingleResNode)
ImplementRTTIOfClass(PointResNode, SingleResNode)
ImplementRTTIOfClass(fRectResNode, SingleResNode)
ImplementRTTIOfClass(fSizeResNode, SingleResNode)
ImplementRTTIOfClass(fPointResNode, SingleResNode)

ImplementRTTIOfClass(CursorResNode, ResNode)
ImplementRTTIOfClass(ExtensionResNode, ResNode)
ImplementRTTIOfClass(BrushResNode, ResNode)
ImplementRTTIOfClass(ImageSourceResNode, ResNode)

ImplementRTTIOfClass(TransformResNode, ResNode)
ImplementRTTIOfClass(SolidColorBrushResNode, BrushResNode)
ImplementRTTIOfClass(LinearGradientBrushResNode, BrushResNode)
ImplementRTTIOfClass(RadialGradientBrushResNode, BrushResNode)
ImplementRTTIOfClass(ImageBrushResNode, BrushResNode)

String ResNode::OFFSET1 = _U("    ");
String ResNode::OFFSET2 = _U("        ");
String ResNode::OFFSET3 = _U("            ");

NullResNode* NullResNode::Value = new NullResNode();

ResNodePool* ResNodePool::Ins()
{
    static ResNodePool _ins;
    return &_ins;
}

ResNodePool::ResNodePool()
{
    _resNodes.Add(suic::GridView::RTTIType(), GridViewResNode::RTTIType());

    _resNodes.Add(suic::OString::RTTIType(), StringResNode::RTTIType());
    _resNodes.Add(suic::Integer::RTTIType(), IntegerResNode::RTTIType());
    _resNodes.Add(suic::OFloat::RTTIType(), DoubleResNode::RTTIType());
    _resNodes.Add(suic::ORect::RTTIType(), RectResNode::RTTIType());
    _resNodes.Add(suic::OSize::RTTIType(), SizeResNode::RTTIType());
    _resNodes.Add(suic::OPoint::RTTIType(), PointResNode::RTTIType());
    _resNodes.Add(suic::OfRect::RTTIType(), fRectResNode::RTTIType());
    _resNodes.Add(suic::OfSize::RTTIType(), fSizeResNode::RTTIType());
    _resNodes.Add(suic::OfPoint::RTTIType(), fPointResNode::RTTIType());
    _resNodes.Add(suic::Boolean::RTTIType(), BooleanResNode::RTTIType());
    _resNodes.Add(suic::OCursor::RTTIType(), CursorResNode::RTTIType());

    _resNodes.Add(suic::ImageBrush::RTTIType(), ImageBrushResNode::RTTIType());
    _resNodes.Add(suic::SolidColorBrush::RTTIType(), SolidColorBrushResNode::RTTIType());
    _resNodes.Add(suic::LinearGradientBrush::RTTIType(), LinearGradientBrushResNode::RTTIType());
    _resNodes.Add(suic::RadialGradientBrush::RTTIType(), RadialGradientBrushResNode::RTTIType());

    _resNodes.Add(suic::Extension::RTTIType(), ExtensionResNode::RTTIType());
    _resNodes.Add(suic::ImageSource::RTTIType(), ImageSourceResNode::RTTIType());
    _resNodes.Add(suic::Transform::RTTIType(), CursorResNode::RTTIType());

    _resNodes.Add(NodeUndefined::RTTIType(), NodeUndefinedResNode::RTTIType());
    _resNodes.Add(SingleUndefined::RTTIType(), SingleUndefinedResNode::RTTIType());
}

ResNodePool::~ResNodePool()
{
}

bool ResNodePool::CreateResNode(suic::DpProperty* dp, suic::Object* val, ResNodePtr& obj)
{
    if (val == NULL)
    {
        obj = NullResNode::Value;
        return true;
    }
    else
    {
        obj = val;
        if (NULL != obj.get())
        {
            return true;
        }
    }

    suic::RTTIOfInfo* rttiInfo = NULL;
    suic::RTTIOfInfo* rttiTmp = val->GetRTTIType();

    while (rttiTmp != NULL)
    {
        if (_resNodes.TryGetValue(rttiTmp, rttiInfo))
        {
            obj = rttiInfo->Create();
            obj->SetValue(val);
            return true;
        }
        rttiTmp = rttiTmp->BaseType();
    }
    obj = new SingleResNode(val); 
    return true;
}

ResNode* ResNode::CreateResNode(suic::Object* val, ResNodePtr& obj)
{
    ResNodePool::Ins()->CreateResNode(NULL, val, obj);
    return obj.get();
}

/*ResNode* ResNode::CreateResNode(suic::Object* val, ResNodePtr& obj)
{
    if (suic::RTTICast<suic::ImageBrush>(val))
    {
        obj = new ImageBrushResNode((suic::ImageBrush*)(val));
    }
    else if (suic::RTTICast<suic::SolidColorBrush>(val))
    {
        obj = new SolidColorBrushResNode((suic::SolidColorBrush*)(val));
    }
    else if (suic::RTTICast<suic::LinearGradientBrush>(val))
    {
        obj = new LinearGradientBrushResNode((suic::LinearGradientBrush*)(val));
    }
    else if (suic::RTTICast<suic::RadialGradientBrush>(val))
    {
        obj = new RadialGradientBrushResNode((suic::RadialGradientBrush*)(val));
    }
    else if (suic::RTTICast<Extension>(val))
    {
        obj = new ExtensionResNode((Extension*)(val));
    }
    else if (suic::RTTICast<suic::GridView>(val))
    {
        obj = new GridViewResNode(suic::RTTICast<suic::GridView>(val));
    }
    else if (suic::RTTICast<suic::OCursor>(val))
    {
        obj = new CursorResNode((suic::OCursor*)(val));
    }
    else if (suic::RTTICast<ImageSource>(val))
    {
        obj = new ImageSourceResNode((suic::ImageSource*)(val));
    }
    else if (suic::RTTICast<OString>(val))
    {
        obj = new StringResNode((suic::OString*)(val));
    }
    else if (suic::RTTICast<suic::Transform>(val))
    {
        obj = new TransformResNode((suic::Transform*)(val));
    }
    else if (suic::RTTICast<NodeUndefined>(val))
    {
        obj = new NodeUndefinedResNode((NodeUndefined*)(val));
    }
    else if (suic::RTTICast<SingleUndefined>(val))
    {
        obj = new SingleUndefinedResNode((SingleUndefined*)(val));
    }
    else if (suic::RTTICast<ResNode>(val))
    {
        obj = suic::RTTICast<ResNode>(val);
    }
    else
    {
        obj = new SingleResNode(val);
    }

    return obj.get();
}*/

void ResNode::CloneNode(ResNodePtr& obj)
{
    CreateResNode(GetValue(), obj);
}

ResValueNode::ResValueNode()
{
    _value = NULL;
}

ResValueNode::ResValueNode(suic::Object* val)
{
    _value = val;
    if (_value != NULL)
    {
        _value->ref();
    }
}

ResValueNode::~ResValueNode()
{
    FREEREFOBJ(_value);
}

bool ResValueNode::IsSingleValue()
{
    return true;
}

void ResValueNode::SetValue(suic::Object* val)
{
    SETREFOBJ(_value, val);
}

suic::Object* ResValueNode::GetValue()
{
    return _value;
}

void ResValueNode::Dispose()
{
    FREEREFOBJ(_value);
}

//====================================================
// SingleResNode

SingleResNode::SingleResNode()
{
}

SingleResNode::SingleResNode(suic::Object* val)
    : ResValueNode(val)
{
}

SingleResNode::~SingleResNode()
{
}

suic::String SingleResNode::GetFormatValue()
{
    suic::OfRect* fObj = RTTICast<suic::OfRect>(GetValue());
    if (NULL != fObj)
    {
        suic::fRect rect = fObj->TofRect();
        return suic::String().Format(_U("%g,%g,%g,%g"), 
            rect.left, rect.top, rect.right, rect.bottom);
    }
    suic::OfPoint* pObj = RTTICast<suic::OfPoint>(GetValue());
    if (NULL != pObj)
    {
        suic::fPoint point = pObj->TofPoint();
        return suic::String().Format(_U("%g,%g"), point.x, point.y);
    }
    suic::OFloat* flObj = RTTICast<suic::OFloat>(GetValue());
    if (NULL != flObj)
    {
        return suic::String().Format(_U("%g"), flObj->ToFloat());
    }
    return suic::String();
}

suic::String SingleResNode::GetNodeName()
{
    suic::String strNodeName;

    if (NULL != GetValue())
    {
        strNodeName = _U("sys:");
        strNodeName += GetValue()->GetRTTIType()->typeName;
    }
    return strNodeName;
}

suic::String SingleResNode::GetSingleXml()
{
    suic::String strVal;

    if (NULL == GetValue())
    {
        return strVal;
    }

    strVal = GetFormatValue();

    if (strVal.Empty())
    {
        strVal = GetValue()->ToString();
    }

    return strVal;
}

suic::String SingleResNode::GetResXml(const String& offset)
{
    suic::String strXml;
    suic::String strTypeName;
    
    if (NULL != GetValue())
    {
        strTypeName = GetNodeName();

        strXml = offset + _U("<");
        strXml += strTypeName;
        strXml += _U(" ");

        if (!GetKey().Empty())
        {
            strXml += _U("x:Key=\"");
            strXml += GetKey();
            strXml += _U("\">");
        }

        strXml += GetSingleXml();
        strXml += _U("</");
        strXml += strTypeName;
        strXml += _U(">\n");
    }   

    return strXml;
}


//====================================================
// StringResNode

StringResNode::StringResNode()
{
}

StringResNode::StringResNode(suic::OString* val)
    : SingleResNode(val)
{

}

StringResNode::~StringResNode()
{

}

suic::String StringResNode::GetFormatValue()
{
    suic::String strVal;
    if (NULL != GetValue())
    {
        strVal = GetValue()->ToString();
    }
    return strVal;
}

suic::String StringResNode::GetNodeName()
{
    return _U("sys:String");
}

//====================================================
// IntegerResNode

IntegerResNode::IntegerResNode()
{
}

IntegerResNode::IntegerResNode(suic::Integer* val)
    : SingleResNode(val)
{

}

IntegerResNode::~IntegerResNode()
{

}

suic::String IntegerResNode::GetFormatValue()
{
    suic::String strVal;
    if (NULL != GetValue())
    {
        strVal = GetValue()->ToString();
    }
    return strVal;
}

suic::String IntegerResNode::GetNodeName()
{
    return _U("sys:Int32");
}

//====================================================
// WHIntegerResNode

WHIntegerResNode::WHIntegerResNode()
{
}

WHIntegerResNode::WHIntegerResNode(suic::Integer* val)
    : SingleResNode(val)
{

}

WHIntegerResNode::~WHIntegerResNode()
{

}

suic::String WHIntegerResNode::GetFormatValue()
{
    suic::String strVal;
    if (NULL != GetValue())
    {
        int iVal = GetValue()->ToInt();
        if (iVal < 0)
        {
            strVal = _U("Auto");
        }
        else
        {
            strVal = GetValue()->ToString();
        }
    }
    return strVal;
}

suic::String WHIntegerResNode::GetNodeName()
{
    return _U("sys:Int32");
}

//====================================================
// DoubleResNode

DoubleResNode::DoubleResNode()
{
}

DoubleResNode::DoubleResNode(suic::OFloat* val)
    : SingleResNode(val)
{

}

DoubleResNode::~DoubleResNode()
{

}

suic::String DoubleResNode::GetFormatValue()
{
    suic::String strVal;
    if (NULL != GetValue())
    {
        strVal.Format(_U("%g"), GetValue()->ToFloat());
    }
    return strVal;
}

suic::String DoubleResNode::GetNodeName()
{
    return _U("sys:Float");
}

//====================================================
// BooleanResNode

BooleanResNode::BooleanResNode()
{
}

BooleanResNode::BooleanResNode(suic::Boolean* val)
    : SingleResNode(val)
{

}

BooleanResNode::~BooleanResNode()
{

}

suic::String BooleanResNode::GetFormatValue()
{
    suic::String strVal;
    if (suic::Boolean::True == GetValue())
    {
        strVal = _U("True");
    }
    else if (suic::Boolean::False == GetValue())
    {
        strVal = _U("False");
    }
    else
    {
        strVal = _U("x:Null");
    }
    return strVal;
}

suic::String BooleanResNode::GetNodeName()
{
    return _U("sys:Boolean");
}

//====================================================
// RectResNode

RectResNode::RectResNode()
{
}

RectResNode::RectResNode(suic::ORect* val)
    : SingleResNode(val)
{

}

RectResNode::~RectResNode()
{

}

suic::String RectResNode::GetFormatValue()
{
    suic::String strVal;
    suic::ORect* oRect = dynamic_cast<suic::ORect*>(GetValue());
    if (NULL != oRect)
    {
        strVal = oRect->ToString();
    }
    return strVal;
}

suic::String RectResNode::GetNodeName()
{
    return _U("sys:Rect");
}

//====================================================
// SizeResNode

SizeResNode::SizeResNode()
{
}

SizeResNode::SizeResNode(suic::OSize* val)
    : SingleResNode(val)
{

}

SizeResNode::~SizeResNode()
{

}

suic::String SizeResNode::GetFormatValue()
{
    suic::String strVal;
    suic::OSize* oSize = dynamic_cast<suic::OSize*>(GetValue());
    if (NULL != oSize)
    {
        strVal = oSize->ToString();
    }
    return strVal;
}

suic::String SizeResNode::GetNodeName()
{
    return _U("sys:Size");
}

//====================================================
// PointResNode

PointResNode::PointResNode()
{
}

PointResNode::PointResNode(suic::OPoint* val)
    : SingleResNode(val)
{

}

PointResNode::~PointResNode()
{

}

suic::String PointResNode::GetFormatValue()
{
    suic::String strVal;
    suic::OPoint* oPoint = dynamic_cast<suic::OPoint*>(GetValue());
    if (NULL != oPoint)
    {
        strVal = oPoint->ToString();
    }
    return strVal;
}

suic::String PointResNode::GetNodeName()
{
    return _U("sys:Point");
}

//====================================================
// fRectResNode

fRectResNode::fRectResNode()
{
}

fRectResNode::fRectResNode(suic::OfRect* val)
    : SingleResNode(val)
{

}

fRectResNode::~fRectResNode()
{

}

suic::String fRectResNode::GetFormatValue()
{
    suic::String strVal;
    suic::OfRect* oRect = dynamic_cast<suic::OfRect*>(GetValue());
    if (NULL != oRect)
    {
        strVal = oRect->ToString();
    }
    return strVal;
}

suic::String fRectResNode::GetNodeName()
{
    return _U("sys:fRect");
}

//====================================================
// fSizeResNode

fSizeResNode::fSizeResNode()
{
}

fSizeResNode::fSizeResNode(suic::OfSize* val)
    : SingleResNode(val)
{

}

fSizeResNode::~fSizeResNode()
{

}

suic::String fSizeResNode::GetFormatValue()
{
    suic::String strVal;
    suic::OfSize* oSize = dynamic_cast<suic::OfSize*>(GetValue());
    if (NULL != oSize)
    {
        strVal = oSize->ToString();
    }
    return strVal;
}

suic::String fSizeResNode::GetNodeName()
{
    return _U("sys:fSize");
}

//====================================================
// fPointResNode

fPointResNode::fPointResNode()
{
}

fPointResNode::fPointResNode(suic::OfPoint* val)
    : SingleResNode(val)
{

}

fPointResNode::~fPointResNode()
{

}

suic::String fPointResNode::GetFormatValue()
{
    suic::String strVal;
    suic::OfPoint* oPoint = dynamic_cast<suic::OfPoint*>(GetValue());
    if (NULL != oPoint)
    {
        strVal = oPoint->ToString();
    }
    return strVal;
}

suic::String fPointResNode::GetNodeName()
{
    return _U("sys:fPoint");
}

//====================================================
// BaseResNode

/*BaseResNode::BaseResNode()
{
}

BaseResNode::BaseResNode(suic::Object* val)
    : SingleResNode(val)
{
}

BaseResNode::~BaseResNode()
{
}

suic::String BaseResNode::GetSingleXml()
{
    return suic::String();
}

suic::String BaseResNode::GetResXml(const String& offset)
{
    suic::String strXml;
    suic::String strTypeName;

    strTypeName = ResTypePool::GetResTypeName(GetValue()->GetRTTIType());

    strXml = offset + _U("<");
    strXml += strTypeName;
    strXml += _U(" ");

    if (!GetKey().Empty())
    {
        strXml += _U("x:Key=\"");
        strXml += GetKey();
        strXml += _U("\" ");
    }

    strXml += GetValue()->ToString();
    strXml += _U(" />\n");

    return strXml;
}*/

//================================================
// CursorResNode

CursorResNode::CursorResNode()
{
    _value = NULL;
}

CursorResNode::CursorResNode(suic::OCursor* val)
{
    _value = val;
    if (_value != NULL)
    {
        _value->ref();
    }
}

CursorResNode::~CursorResNode()
{
    if (_value != NULL)
    {
        _value->unref();
    }
}

String CursorResNode::CursorToString(suic::OCursor* pCursor)
{
    if (pCursor == suic::OCursor::Arrow)
    {
        return _U("Arrow");
    }
    else if (pCursor == suic::OCursor::Hand)
    {
        return _U("Hand");
    }
    else if (pCursor == suic::OCursor::Wait)
    {
        return _U("Wait");
    }
    else if (pCursor == suic::OCursor::Cross)
    {
        return _U("Cross");
    }
    else if (pCursor == suic::OCursor::Resize)
    {
        return _U("Resize");
    }
    else if (pCursor == suic::OCursor::Splith)
    {
        return _U("Splith");
    }
    else if (pCursor == suic::OCursor::Ibeam)
    {
        return _U("Ibeam");
    }
    else if (pCursor == suic::OCursor::Splitv)
    {
        return _U("Splitv");
    }
    else if (pCursor == suic::OCursor::SizeNwse)
    {
        return _U("SizeNwse");
    }
    else if (pCursor == suic::OCursor::Splitv)
    {
        return _U("Splitv");
    }
    else if (pCursor == suic::OCursor::SizeNesw)
    {
        return _U("SizeNesw");
    }
    else if (pCursor == suic::OCursor::SizeWe)
    {
        return _U("SizeWe");
    }
    else if (pCursor == suic::OCursor::SizeNs)
    {
        return _U("SizeNs");
    }
    else if (pCursor == suic::OCursor::SizeAll)
    {
        return _U("SizeAll");
    }
    else
    {
        return _U("");
    }
}

bool CursorResNode::IsSingleValue()
{
    return true;
}

suic::String CursorResNode::GetSingleXml()
{
    return CursorToString(_value);
}

suic::String CursorResNode::GetResXml(const String& offset)
{
    suic::String strXml;
    return strXml;
}

void CursorResNode::SetValue(suic::Object* val)
{
    if (_value != NULL)
    {
        _value->unref();
        _value = NULL;
    }

    _value = RTTICast<suic::OCursor>(val);
    if (NULL != _value)
    {
        _value->ref();
    }
}

suic::Object* CursorResNode::GetValue()
{
    return _value;
}

//================================================
// ExpressionResNode

ExtensionResNode::ExtensionResNode()
{
    _value = NULL;
}

ExtensionResNode::ExtensionResNode(suic::Extension* val)
{
    _value = val;
    ADDREFOBJ(_value);
}

ExtensionResNode::~ExtensionResNode()
{
    FREEREFOBJ(_value);
}

bool ExtensionResNode::IsSingleValue()
{
    return true;
}

void ExtensionResNode::SetValue(suic::Object* val)
{
    if (NULL != val)
    {
        val->ref();
    }
    SETREFOBJ(_value, suic::RTTICast<suic::Extension>(val));
    if (NULL != val)
    {
        val->unref();
    }
}

suic::Object* ExtensionResNode::GetValue()
{
    return _value;
}

suic::String ExtensionResNode::GetBindingModeXml(suic::Binding* binding)
{
    suic::String strXml;

    switch (binding->GetBindMode())
    {
        case suic::BindingMode::OneTime:
            strXml += _U("OneTime");
            break;

        case suic::BindingMode::OneWay:
            strXml += _U("OneWay");
            break;

        case suic::BindingMode::TwoWay:
            strXml += _U("TwoWay");
            break;

        case suic::BindingMode::OneWayToSource:
            strXml += _U("OneWayToSource");
            break;
    }

    if (!strXml.Empty())
    {
        strXml = _U("Mode=") + strXml;
    }

    return strXml;
}

suic::String ExtensionResNode::GetUpdateSourceTriggerXml(suic::Binding* binding)
{
    suic::String strXml;

    if (binding->GetUpdateSourceTrigger() != suic::UpdateSourceTrigger::ustPropertyChanged)
    {
        switch (binding->GetUpdateSourceTrigger())
        {
        case suic::UpdateSourceTrigger::ustLostFocus:
            strXml += _U("LostFocus");
            break;

        case suic::UpdateSourceTrigger::ustExplicit:
            strXml += _U("Explicit");
            break;
        }
    }

    if (!strXml.Empty())
    {
        strXml = _U("UpdateSourceTrigger=") + strXml;
    }

    return strXml;
}

suic::String ExtensionResNode::GetSingleXml()
{
    suic::Binding* binding = suic::RTTICast<suic::Binding>(GetValue());
    if (NULL != binding)
    {
        suic::String strXml;
        suic::SourceRef* srcRef = binding->GetSourceRef();

        strXml = _U("{Binding ");

        if (!binding->GetPath().Path.Empty())
        {
            strXml += _U("Path=");
            strXml += binding->GetPath().Path;
            strXml += _U(",");
        }

        if (!binding->GetConverterName().Empty())
        {
            strXml += _U("Converter=");
            strXml += binding->GetConverterName();
            strXml += _U(",");
        }

        if (!binding->GetConverterParam().Empty())
        {
            strXml += _U("ConverterParam=");
            strXml += binding->GetConverterParam();
            strXml += _U(",");
        }

        if (srcRef != NULL)
        {
            suic::RelativeSource* relativeRef = dynamic_cast<suic::RelativeSource*>(srcRef);
            suic::String strMode = GetBindingModeXml(binding);
            suic::String strTrigger = GetUpdateSourceTriggerXml(binding);

            if (!strMode.Empty())
            {
                strXml += strMode;
                strXml += _U(",");
            }

            if (!strTrigger.Empty())
            {
                strXml += strTrigger;
                strXml += _U(",");
            }

            if (NULL != relativeRef)
            {
                strXml += _U("RelativeSource=");
                if (relativeRef->Mode == RelativeSourceMode::Self)
                {
                    strXml += _U("{RelativeSource Self}");
                }
                else if (relativeRef->Mode == RelativeSourceMode::TemplatedParent)
                {
                    strXml += _U("{RelativeSource TemplatedParent}");
                }
                else if (relativeRef->Mode == RelativeSourceMode::FindAncestor)
                {
                    strXml += _U("{RelativeSource FindAncestor,AncestorType={x:Type ");
                    strXml += relativeRef->AncestorType->typeName;
                    strXml += _U("}, AncestorLevel=");
                    strXml += String().FromInt(relativeRef->AncestorLevel);
                    strXml += _U("}");
                }
            }
            else
            {
                suic::ElementNameSource* elementRef = dynamic_cast<suic::ElementNameSource*>(srcRef);
                if (NULL != elementRef)
                {
                    strXml += _U("ElementName=");
                    strXml += elementRef->Name;
                }
            }
        }
        else
        {
            if (!binding->GetPath().Path.Empty())
            {
                strXml = _U("{Binding ");
                strXml += _U("Path=");
                strXml += binding->GetPath().Path;
                
                suic::String strMode = GetBindingModeXml(binding);
                suic::String strTrigger = GetUpdateSourceTriggerXml(binding);

                if (!strMode.Empty())
                {
                    strXml += _U(",");
                    strXml += strMode;
                }

                if (!strTrigger.Empty())
                {
                    strXml += _U(",");
                    strXml += strTrigger;
                }
            }
        }

        strXml += _U("}");
        return strXml;
    }

    suic::SRExtension* staticRes = suic::RTTICast<suic::SRExtension>(GetValue());
    if (NULL != staticRes)
    {
        String strXml = _U("{StaticResource ");

        strXml += staticRes->GetResourceKey();
        strXml += _U("}");
        return strXml;
    }

    suic::DRExtension* dynamicRes = suic::RTTICast<suic::DRExtension>(GetValue());
    if (NULL != dynamicRes)
    {
        String strXml = _U("{DynamicResource ");

        strXml += dynamicRes->GetResourceKey();
        strXml += _U("}");
        return strXml;
    }

    suic::TemplateBindingExtension* tempRes = suic::RTTICast<suic::TemplateBindingExtension>(GetValue());
    if (NULL != tempRes && NULL != tempRes->GetProp())
    {
        String strXml = _U("{TemplateBinding ");

        if (tempRes->GetProp()->IsAttached())
        {
            strXml += tempRes->GetProp()->GetOwnerType()->typeName;
            strXml +=  _U(".") + tempRes->GetProp()->GetName();
        }
        else
        {
            strXml += tempRes->GetProp()->GetName();
        }
        strXml += _U("}");
        return strXml;
    }

    return String();
}

suic::String ExtensionResNode::GetResXml(const String& offset)
{
    return suic::String();
}

suic::String ExtensionResNode::GetResourceKey()
{
    suic::SRExtension* staticRes = suic::RTTICast<suic::SRExtension>(GetValue());
    if (NULL != staticRes)
    {
        return staticRes->GetResourceKey();
    }

    suic::DRExtension* dynamicRes = suic::RTTICast<suic::DRExtension>(GetValue());
    if (NULL != dynamicRes)
    {
        return dynamicRes->GetResourceKey();
    }

    return _U("");
}

//================================================
// GridViewResNode

GridViewResNode::GridViewResNode()
{
}

GridViewResNode::GridViewResNode(suic::GridView* val)
{
}

GridViewResNode::~GridViewResNode()
{
}

bool GridViewResNode::IsSingleValue()
{
    return false;
}

void GridViewResNode::SetValue(suic::Object* val)
{
}

suic::Object* GridViewResNode::GetValue()
{
    return _value;
}

suic::String GridViewResNode::GetSingleXml()
{
    suic::String strXml;
    return strXml;
}

suic::String GridViewResNode::GetResXml(const String& offset)
{
    suic::String strXml;
    return strXml;
}

//================================================
// BrushResNode

BrushResNode::BrushResNode()
{
    _value = NULL;
}

BrushResNode::BrushResNode(suic::Brush* val)
{
    _value = val;
    ADDREFOBJ(_value);
}

BrushResNode::~BrushResNode()
{
    FREEREFOBJ(_value);
}

bool BrushResNode::IsSingleValue()
{
    return false;
}

void BrushResNode::SetValue(suic::Object* val)
{
    if (NULL != val)
    {
        val->ref();
        SETREFOBJ(_value, suic::RTTICast<suic::Brush>(val));
        val->unref();
    }
    else
    {
        SETREFOBJ(_value, NULL);
    }
}

suic::Object* BrushResNode::GetValue()
{
    return _value;
}

suic::String BrushResNode::GetSingleXml()
{
    return suic::String();
}

suic::String BrushResNode::GetResXml(const String& offset)
{
    return suic::String();
}

bool BrushResNode::GetSolidColorBrushResXml(const String& offset, String& strXml)
{
    SolidColorBrush* brush = RTTICast<SolidColorBrush>(GetValue());

    if (NULL == brush)
    {
        return false;
    }

    strXml = offset + _U("<SolidColorBrush ");

    if (!GetKey().Empty())
    {
        strXml += _U("x:Key=\"");
        strXml += GetKey();
        strXml += _U("\" ");
    }

    strXml += _U("Color=\"");
    strXml += GetValue()->ToString();
    strXml += _U("\"/>\n");

    return true;
}

//================================================
// TransformResNode

TransformResNode::TransformResNode()
{
    _transform = NULL;
}

TransformResNode::TransformResNode(Transform* val)
{
    _transform = val;
    if (NULL != _transform)
    {
        _transform->ref();
    }
}

TransformResNode::~TransformResNode()
{
    FREEREFOBJ(_transform);
}

void TransformResNode::SetValue(suic::Object* val)
{
    if (NULL != val)
    {
        val->ref();
        SETREFOBJ(_transform, suic::RTTICast<Transform>(val));
        val->unref();
    }
    else
    {
        SETREFOBJ(_transform, NULL);
    }
}

bool TransformResNode::IsSingleValue()
{
    return false;
}

suic::String TransformResNode::GetSingleXml()
{
    return suic::String();
}

suic::String TransformResNode::GetResXml(const String& offset)
{
    suic::String strXml = offset + _U("<");

    suic::ScaleTransform* sTrans = RTTICast<suic::ScaleTransform>(_transform);
    if (NULL != sTrans)
    {
        strXml += _U("ScaleTransform");

        strXml += _U(" CenterX=\"") + String().FromFloat(sTrans->GetCenterX()) + _U("\"");
        strXml += _U(" CenterY=\"") + String().FromFloat(sTrans->GetCenterY()) + _U("\"");

        strXml += _U(" ScaleX=\"") + String().FromFloat(sTrans->GetScaleX()) + _U("\"");
        strXml += _U(" ScaleY=\"") + String().FromFloat(sTrans->GetScaleY()) + _U("\"");

        strXml += _U("/>\n");

        return strXml;
    }

    suic::RotateTransform* rTrans = RTTICast<suic::RotateTransform>(_transform);
    if (NULL != rTrans)
    {
        strXml += _U("RotateTransform");

        strXml += _U(" CenterX=\"") + String().FromFloat(rTrans->GetCenterX()) + _U("\"");
        strXml += _U(" CenterY=\"") + String().FromFloat(rTrans->GetCenterY()) + _U("\"");
        strXml += _U(" Angle=\"") + String().FromFloat(rTrans->GetAngle()) + _U("\"");

        strXml += _U("/>\n");

        return strXml;
    }

    suic::TranslateTransform* tTrans = RTTICast<suic::TranslateTransform>(_transform);
    if (NULL != tTrans)
    {
        strXml += _U("TranslateTransform");
        strXml += _U(" X=\"") + String().FromFloat(tTrans->GetX()) + _U("\"");
        strXml += _U(" Y=\"") + String().FromFloat(tTrans->GetY()) + _U("\"");
        strXml += _U("/>\n");
        return strXml;
    }

    suic::Rotate3DTransform* r3dTrans = RTTICast<suic::Rotate3DTransform>(_transform);
    if (NULL != r3dTrans)
    {
        strXml += _U("Rotate3DTransform");
        strXml += _U(" AngleX=\"") + String().FromFloat(r3dTrans->GetAngleX()) + _U("\"");
        strXml += _U(" AngleY=\"") + String().FromFloat(r3dTrans->GetAngleY()) + _U("\"");
        strXml += _U(" AngleZ=\"") + String().FromFloat(r3dTrans->GetAngleZ()) + _U("\"");
        strXml += _U("/>\n");
        return strXml;
    }

    return strXml;
}

//================================================
// SolidColorBrushResNode

SolidColorBrushResNode::SolidColorBrushResNode()
{
}

SolidColorBrushResNode::SolidColorBrushResNode(SolidColorBrush* val)
    : BrushResNode(val)
{
}

SolidColorBrushResNode::~SolidColorBrushResNode()
{
}

bool SolidColorBrushResNode::IsSingleValue()
{
    return true;
}

suic::String SolidColorBrushResNode::GetSingleXml()
{
    suic::SolidColorBrush* brush = suic::RTTICast<suic::SolidColorBrush>(GetValue());
    suic::String strXml = Color::ToString(brush->GetColor());//suic::String::Int32ToHex(brush->GetColor());
    return strXml;
}

suic::String SolidColorBrushResNode::GetResXml(const String& offset)
{
    suic::String strXml;
    suic::SolidColorBrush* brush = RTTICast<SolidColorBrush>(GetValue());

    if (NULL != brush)
    {
        strXml = offset + _U("<SolidColorBrush ");

        if (!GetKey().Empty())
        {
            strXml += _U("x:Key=\"");
            strXml += GetKey();
            strXml += _U("\" ");
        }

        strXml += _U("Color=\"");
        strXml += Color::ToString(brush->GetColor());
        strXml += _U("\"/>\n");
    }

    return strXml;
}

//================================================
// LinearGradientBrushResNode

LinearGradientBrushResNode::LinearGradientBrushResNode()
{
}

LinearGradientBrushResNode::LinearGradientBrushResNode(suic::LinearGradientBrush* val)
    : BrushResNode(val)
{
}

LinearGradientBrushResNode::~LinearGradientBrushResNode()
{
}

suic::String LinearGradientBrushResNode::GetResXml(const String& offset)
{
    suic::String strXml;
    LinearGradientBrush* brush = suic::RTTICast<LinearGradientBrush>(GetValue());
    GradientStops* linearInfo = brush->GetGradientStops();

    strXml = offset + _U("<LinearGradientBrush ");

    if (!GetKey().Empty())
    {
        strXml += _U("x:Key=\"");
        strXml += GetKey();
        strXml += _U("\" ");
    }

    strXml += _U("StartPoint=\"");
    strXml += String().Format(_U("%g,%g"), brush->GetStartPoint().x, brush->GetStartPoint().y);
    strXml += _U("\" EndPoint=\"");
    strXml += String().Format(_U("%g,%g"), brush->GetEndPoint().x, brush->GetEndPoint().y);

    strXml += _U("\">\n");

    for (int i = 0; i < linearInfo->colors.GetCount(); ++i)
    {
        strXml += offset + ResNode::OFFSET1 + _U("<GradientStop Offset=\"");
        strXml += String().Format(_U("%g"), linearInfo->ratios[i]);
        strXml += _U("\" Color=\"");
        strXml += Color::ToString(linearInfo->colors[i]);
        strXml += _U("\" />\n");
    }

    strXml += offset + _U("</LinearGradientBrush>\n");

    return strXml;
}

//================================================
// RadialGradientBrushResNode

RadialGradientBrushResNode::RadialGradientBrushResNode()
{
}

RadialGradientBrushResNode::RadialGradientBrushResNode(suic::RadialGradientBrush* val)
    : BrushResNode(val)
{
}

RadialGradientBrushResNode::~RadialGradientBrushResNode()
{
}

suic::String RadialGradientBrushResNode::GetResXml(const String& offset)
{
    suic::String strXml;
    RadialGradientBrush* brush = suic::RTTICast<RadialGradientBrush>(GetValue());
    GradientStops* radialInfo = brush->GetGradientStops();

    strXml = offset + _U("<RadialGradientBrush ");

    if (!GetKey().Empty())
    {
        strXml += _U("x:Key=\"");
        strXml += GetKey();
        strXml += _U("\" ");
    }

    strXml += _U("GradientOrigin=\"");
    strXml += String().Format(_U("%g,%g"), brush->GetOrigin().x, brush->GetOrigin().y);
    strXml += _U("\" RadiusX=\"");
    strXml += String().Format(_U("%g"), brush->GetRadiusX());
    strXml += _U("\" RadiusY=\"");
    strXml += String().Format(_U("%g"), brush->GetRadiusY());

    strXml += _U("\">\n");

    for (int i = 0; i < radialInfo->colors.GetCount(); ++i)
    {
        strXml += offset + ResNode::OFFSET1 + _U("<GradientStop Offset=\"");
        strXml += String().Format(_U("%g"), radialInfo->ratios[i]);
        strXml += _U("\" Color=\"");
        strXml += Color::ToString(radialInfo->colors[i]);//String::Int32ToHex(radialInfo->colors[i]);
        strXml += _U("\" />\n");
    }

    strXml += offset + _U("</RadialGradientBrush>\n");

    return strXml;
}

//================================================
// ImageBrushResNode

/*void ResPath::SetPath(const String& path)
{
    String strSource = path;
    strSource.Trim();
    int pos = strSource.IndexOf(_U("@"));

    if (pos != -1)
    {
        resPath = strSource.Substring(pos + 1);
        StringArray strArr;
        strArr.SplitString(strSource.Substring(0, pos), _U(","));
        if (strArr.GetCount() == 2)
        {
            resType = strArr[0];
            resFlag = strArr[1];
        }
    }
    else
    {
        resPath = strSource;
    }

    resPath = resPath.Replace(_U("\\"), _U("/"));
    resPath.Trim();
    resFlag.Trim();
}

String ResPath::GetXml(const String& resId)
{
    String strXml;

    if (!resType.Empty())
    {
        strXml += resType;
        strXml += _U(",");
        strXml += resFlag;
        strXml += _U("@");
        strXml += resPath;
    }
    else if (!resId.Empty())
    {
        strXml += _U("res+,");
        strXml += resId;
        strXml += _U("@");
        strXml += resPath;

        resType = _U("res+");
        resFlag = resId;
    }
    else
    {
        strXml += resPath;
    }

    return strXml;
}*/

ImageBrushResNode::ImageBrushResNode()
{
}

ImageBrushResNode::ImageBrushResNode(suic::ImageBrush* val)
    : BrushResNode(NULL)
{
    SetImageBrush(val);
}

ImageBrushResNode::~ImageBrushResNode()
{
}

void ImageBrushResNode::SetValue(suic::Object* val)
{
    BrushResNode::SetValue(val);
    if (NULL != GetImageBrush() && NULL != GetImageBrush()->GetImageSource())
    {
        _resPath = GetImageBrush()->GetImageSource()->GetUri();
    }
}

ResourceUri* ImageBrushResNode::GetResPath()
{
    return (&_resPath);
}

void ImageBrushResNode::SetImageBrush(suic::ImageBrush* val)
{
    SetValue(val);
}

/*void ImageBrushResNode::SetImageSource(const String& imgSource)
{
    _resPath.Parse(imgSource);

    //suic::ImageBrush* imgBr = new suic::ImageBrush();
    //imgBr->SetImageSource(new ImageSource(imgSource));
    //SetValue(imgBr);

    suic::ImageBrush* imgBr = GetImageBrush();

    if (NULL == imgBr)
    {
        imgBr = new suic::ImageBrush();
        imgBr->SetImageSource(new ImageSource(imgSource));
        SetValue(imgBr);
    }
    else
    {
        imgBr->SetImageSource(new ImageSource(imgSource));
    }
}*/

suic::ImageBrush* ImageBrushResNode::GetImageBrush()
{
    return RTTICast<suic::ImageBrush>(GetValue());
}

suic::ImageBrush* ImageBrushResNode::RefleshImageSource()
{
    suic::ImageBrush* imgBr = new suic::ImageBrush();
    imgBr->ref();
    imgBr->SetImageSource(new ImageSource(_resPath.ToString()));
    SetValue(imgBr);
    imgBr->unref();

    return imgBr;
}

suic::String ImageBrushResNode::GetResXml(const String& offset)
{
    suic::String strXml;
    const suic::fRect defViewbox(0, 0, 1, 1);

    ImageBrush* brush = suic::RTTICast<ImageBrush>(GetValue());

    strXml = offset + _U("<ImageBrush ");

    if (!GetKey().Empty())
    {
        strXml += _U("x:Key=\"");
        strXml += GetKey();
        strXml += _U("\" ");
    }

    strXml += _U("ImageSource=\"");
    strXml += _resPath.ToString();

    bool bEqual = (brush->GetViewport() == defViewbox);

    if (!bEqual)
    {
        strXml += _U("\" Viewport=\"");
        strXml += String().Format(_U("%g,%g,%g,%g")
            , brush->GetViewport().left
            , brush->GetViewport().top
            , brush->GetViewport().right
            , brush->GetViewport().bottom);
    }

    if (brush->GetViewportUnits() == BrushMappingMode::bmmAbsolute)
    {
        strXml += _U("\" ViewportUnits=\"Absolute");
    }

    bEqual = (brush->GetViewbox() == defViewbox);

    if (!bEqual)
    {
        strXml += _U("\" Viewbox=\"");
        strXml += String().Format(_U("%g,%g,%g,%g")
            , brush->GetViewbox().left
            , brush->GetViewbox().top
            , brush->GetViewbox().right
            , brush->GetViewbox().bottom);
    }

    if (brush->GetViewboxUnits() == BrushMappingMode::bmmAbsolute)
    {
        strXml += _U("\" ViewboxUnits=\"Absolute");
    }

    if (!brush->GetViewCorner().IsZero())
    {
        strXml += _U("\" Viewcorner=\"");
        strXml += String().Format(_U("%d,%d,%d,%d")
            , brush->GetViewCorner().left
            , brush->GetViewCorner().top
            , brush->GetViewCorner().right
            , brush->GetViewCorner().bottom);
    }

    if (brush->GetStretch() != Stretch::stFill)
    {
        strXml += _U("\" Stretch=\"");
        switch (brush->GetStretch())
        {
            case Stretch::stUniform:
                strXml += _U("Uniform");
                break;

            case Stretch::stUniformToFill:
                strXml += _U("UniformToFill");
                break;

            default:
                strXml += _U("None");
        }
    }

    if (brush->GetTileMode() != TileMode::tmNone)
    {
        strXml += _U("\" TileMode=\"");
        switch (brush->GetTileMode())
        {
        case TileMode::tmFlipX:
            strXml += _U("FlipX");
            break;

        case TileMode::tmFlipY:
            strXml += _U("FlipY");
            break;

        case TileMode::tmFlipXY:
            strXml += _U("FlipXY");
            break;

        default:
            strXml += _U("Tile");
        }
    }

    strXml += _U("\" />\n");

    return strXml;
}

//==========================================

ImageSourceResNode::ImageSourceResNode()
{
    _source = NULL;
}

ImageSourceResNode::ImageSourceResNode(suic::ImageSource* val)
{
    _source = val;
    ADDREFOBJ(_source);
}

ImageSourceResNode::~ImageSourceResNode()
{
    FREEREFOBJ(_source);
}

void ImageSourceResNode::CloneNode(ResNodePtr& obj)
{
    if (NULL != _source)
    {
        suic::ImageSource* src = new suic::ImageSource();
        src->SetUri(_source->GetUri());
        obj = new ImageSourceResNode(src);
    }
    else
    {
        obj = new ImageSourceResNode();
    }
}

bool ImageSourceResNode::IsSingleValue()
{
    return true;
}

suic::Object* ImageSourceResNode::GetValue()
{
    return _source;
}

void ImageSourceResNode::SetValue(suic::Object* val)
{
    if (NULL != val)
    {
        val->ref();
        SETREFOBJ(_source, suic::RTTICast<suic::ImageSource>(val));
        val->unref();
    }
    else
    {
        SETREFOBJ(_source, NULL);
    }
}

suic::String ImageSourceResNode::GetSingleXml()
{
    if (NULL != _source)
    {
        return _source->GetUri().ToString();
    }
    return suic::String();
}

suic::String ImageSourceResNode::GetResXml(const String& offset)
{
    suic::String strXml;
    strXml = offset + _U("<ImageSource x:Key=\"");
    strXml += GetKey() + _U("\" Source=\"");

    if (NULL != _source)
    {
        strXml += _source->GetUri().ToString() + _U("\" />\n");
    }
    else
    {
        strXml += _U("\" />\n");
    }
    
    return strXml;
}

ImageSource* ImageSourceResNode::GetImageSource()
{
    if (NULL == _source)
    {
        _source = new ImageSource();
        _source->ref();
    }
    return _source;
}

void ImageSourceResNode::ParseResPath(const String& path)
{
    GetImageSource()->SetUri(path);
}

void ImageSourceResNode::SetResPath(const String& comp, const String& path)
{
    ResourceUri uri;
    
    uri.SetComponent(comp);
    uri.SetResourcePath(path);
    GetImageSource()->SetUri(uri);
}
