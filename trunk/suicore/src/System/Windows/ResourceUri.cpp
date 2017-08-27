/////////////////////////////////////////////////////////////////////////
// ResourceUri.cpp

#include <System/Tools/Array.h>
#include <System/Windows/ResourceUri.h>

namespace suic
{

ResourceUri::ResourceUri()
{
    _uriKind = UriKind::ukRelative;
}

ResourceUri::ResourceUri(const String& uri)
{
    Parse(uri);
}

ResourceUri::ResourceUri(const ResourceUri& Other)
{
    _component = Other._component;
    _resPath = Other._resPath;
    _uriKind = Other._uriKind;
}

ResourceUri& ResourceUri::operator=(const ResourceUri& Other)
{
    if (&Other != this)
    {
        _component = Other._component;
        _resPath = Other._resPath;
        _uriKind = Other._uriKind;
    }

    return (*this);
}

bool ResourceUri::operator==(const ResourceUri& Other)
{
    return Euqals(Other);
}

bool ResourceUri::operator!=(const ResourceUri& Other)
{
    return !Euqals(Other);
}

bool ResourceUri::Euqals(const ResourceUri& Other) const
{
    if (_component.Equals(Other._component) && 
        _resPath.Equals(Other._resPath) &&
        _uriKind == Other._uriKind)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool ResourceUri::Parse(const String& uri)
{
    StringArray vecTmp;

    _resPath = _U("");

    vecTmp.SplitString(uri, _U(";"));

    if (vecTmp.GetCount() == 1)
    {
        _resPath = vecTmp[0];
    }
    else if (vecTmp.GetCount() == 2)
    {
        _component = vecTmp[0];
        _resPath = vecTmp[1];
    }

    CheckValue(_resPath);
    CheckValue(_component);

    return true;
}

void ResourceUri::CheckValue(String& val)
{
    val.Trim();
    if (val.Length() > 0 && val[0] == '/')
    {
        val = val.Substring(1);
    }
}

String ResourceUri::ToString()
{
    String strXml;
    if (_component.Empty())
    {
        strXml = _resPath;
    }
    else
    {
        strXml += _U("/");
        strXml += _component;
        strXml += _U(";/");
        strXml += _resPath;
    }

    return strXml;
}

String ResourceUri::GenerateKey()
{
    String strKey;
    String strPath;

    strPath = _resPath;
    strPath.Replace(_U("."), _U(""));
    strPath.Replace(_U("/"), _U(""));
    strPath.Trim();

    if (_component.Empty())
    {
        strKey = strPath;
    }
    else
    {
        strKey += _U("/");
        strKey += _component;
        strKey += _U(";/");
        strKey += strPath;
    }
    return strKey;
}

String ResourceUri::GetCompoment() const
{
    return _component;
}

void ResourceUri::SetComponent(const String& val)
{
    _component = val;
}

String ResourceUri::GetResourcePath() const
{
    return _resPath;
}

void ResourceUri::SetResourcePath(const String& val)
{
    _resPath = val;
}

UriKind ResourceUri::GetUriKind() const
{
    return _uriKind;
}

void ResourceUri::SetUriKind(UriKind val)
{
    _uriKind = val;
}

}
