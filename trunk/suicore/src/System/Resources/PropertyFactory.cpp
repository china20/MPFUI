
#include <System/Resources/PropertyFactory.h>
#include <System/Windows/BuildFactory.h>

namespace suic
{

StringAutoDic<Object> PropFactory::_props;

PropFactory::PropFactory()
{
}

bool PropFactory::ConvertAttachProp(const String& name, AttachDpInfo& attachDb)
{
    int pos = name.IndexOf(_U("."));

    attachDb.dp = NULL;
    attachDb.rttiInfo = NULL;

    if (pos != String::npos)
    {
        String strType = name.Substring(0, pos);
        String strName = name.Substring(pos + 1);
        
        attachDb.rttiInfo = BuildFactory::Ins()->Lookup(strType);

        if (attachDb.rttiInfo != NULL)
        {
            attachDb.dp = DpProperty::Lookup(strName, attachDb.rttiInfo);
        }
    }

    return (NULL != attachDb.dp);
}

void PropFactory::ReadProp(RTTIOfInfo* owner, const String& prop, String val, ObjectPtr& obj)
{
}

void PropFactory::ReadProp(RTTIOfInfo* owner, const String& prop, IXamlNode* node, ObjectPtr& obj)
{
}

void PropFactory::RegisterPropReader(const String& prop, IPropReader* val)
{
    if (NULL != val)
    {
        //val->ref();
        _props.Add(prop, val);
        val->ref();
    }
}

IPropReader* PropFactory::FindPropReader(const String& prop)
{
    Object* pr = NULL;

    if (_props.TryGetValue(prop, pr))
    {
        return (IPropReader*)pr;
    }
    else
    {
        return NULL;
    }
}

void PropFactory::Clear()
{
    _props.Clear();
}

}

