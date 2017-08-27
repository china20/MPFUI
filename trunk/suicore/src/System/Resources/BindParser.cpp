
#include <System/Resources/BindParser.h>
#include <System/Tools/Array.h>
#include <System/Windows/Binding.h>
#include <System/Windows/BuildFactory.h>

/*
// 没有Path和Source，默认获取父对象的DataContext对象的ToString
<binding />
// 下面几个等价，Source为父对象的DataContext
<binding name />
<binding Path=name />
<binding Path=name,Mode=TwoMode />

enum RelativeSourceMode
{
Self,
PreviousData,
TemplatedParent,
FindAncestor,
};

<binding RelativeSource={RelativeSource Self} />
<binding RelativeSource={RelativeSource TemplatedParent} />
<binding RelativeSource={RelativeSource FindAncestor,AncestorType={x:Type Window}, AncestorLevel=1} />

*/
namespace suic
{

BindParser::BindParser()
{
    _binding = NULL;
}

Binding* BindParser::Parse(const String& val)
{
    if (NULL != _binding)
    {
        _binding->unref();
    }

    _binding = new Binding();
    _binding->ref();

    if (ParseBinding(val))
    {
        return _binding;
    }
    else
    {
        _binding->unref();
        _binding = NULL;
        return NULL;
    }
}

int BindParser::FindCommaPos(const String& str, int iStart)
{
    int iNewPos = -1;

    for (int i = iStart; i < str.Length(); ++i)
    {
        if (str[i] == _U(','))
        {
            iNewPos = i;
            break;
        }
    }

    return iNewPos;
}

int BindParser::ParseSingle(const String& str, int iStart, String& val)
{
    int iNewPos = FindCommaPos(str, iStart);

    if (iNewPos == -1)
    {
        val = str.Substring(iStart);
        iNewPos = str.Length();
    }
    else
    {
        val = str.Substring(iStart, iNewPos - iStart);
        iNewPos += 1;
    }

    val.Trim();

    return iNewPos;
}

int BindParser::ParseUpdateSourceTrigger(const String& str, int iStart)
{
    String val;
    int iNewPos = ParseSingle(str, iStart, val);

    if (val.Equals(_U("PropertyChanged")))
    {
        _binding->SetUpdateSourceTrigger(UpdateSourceTrigger::ustPropertyChanged);
    }
    else if (val.Equals(_U("LostFocus")))
    {
        _binding->SetUpdateSourceTrigger(UpdateSourceTrigger::ustLostFocus);
    }
    else if (val.Equals(_U("Explicit")))
    {
        _binding->SetUpdateSourceTrigger(UpdateSourceTrigger::ustExplicit);
    }
    else
    {
        _binding->SetUpdateSourceTrigger(UpdateSourceTrigger::ustDefault);
    }

    return iNewPos;
}

int BindParser::ParseMode(const String& str, int iStart)
{
    String val;
    int iNewPos = ParseSingle(str, iStart, val);

    if (val.Equals(_U("OneTime")))
    {
        _binding->SetBindMode(BindingMode::OneTime);
    }
    else if (val.Equals(_U("OneWayToSource")))
    {
        _binding->SetBindMode(BindingMode::OneWayToSource);
    }
    else if (val.Equals(_U("TwoWay")))
    {
        _binding->SetBindMode(BindingMode::TwoWay);
    }
    else
    {
        _binding->SetBindMode(BindingMode::OneWay);
    }

    return iNewPos;
}

int BindParser::ParsePath(const String& str, int iStart)
{
    String val;
    PropertyPath ppath;
    int iNewPos = ParseSingle(str, iStart, val);

    val.Trim();
    ppath.Path = val;
    _binding->SetPath(ppath);

    return iNewPos;
}

int BindParser::ParseConverter(const String& str, int iStart)
{
    String val;
    int iNewPos = ParseSingle(str, iStart, val);

    _binding->SetConverterName(val);

    return iNewPos;
}

int BindParser::ParseSource(const String& str, int iStart)
{
    String val;
    int iNewPos = ParseSingle(str, iStart, val);
    ObjectSource* obj = new ObjectSource();
    
    obj->ObjName = val;
    _binding->SetSourceRef(obj);

    return iNewPos;
}

int BindParser::ParseConverterParam(const String& str, int iStart)
{
    String val;
    int iNewPos = ParseSingle(str, iStart, val);

    _binding->SetConverterParam(val);

    return iNewPos;
}

int BindParser::ParseElementName(const String& str, int iStart)
{
    String val;
    int iNewPos = ParseSingle(str, iStart, val);
    ElementNameSource* obj = new ElementNameSource();

    obj->Name = val;
    _binding->SetSourceRef(obj);

    return iNewPos;
}

int BindParser::ParseRelativeSource(const String& str, int iStart)
{
    int i = 0;
    int iBegPos = -1;
    int iEndPos = -1;
    int iBraceCount = 0;

    for (i = iStart; i < str.Length(); ++i)
    {
        if (str[i] == _U('{'))
        {
            if (iBegPos == -1)
            {
                iBegPos = i + 1;
            }
            iBraceCount += 1;
        }
        else if (str[i] == _U('}'))
        {
            iBraceCount -= 1;
            if (iBraceCount == 0)
            {
                iEndPos = i;
                break;
            }
        }
    }

    if (iBegPos < iStart)
    {
        return 0;
    }

    if (iEndPos < 0)
    {
        return 0;
    }

    // 取得RelativeSource值
    String val = str.Substring(iBegPos, iEndPos - iBegPos);
    StringArray vec;
    int iPos = val.IndexOf(_U("RelativeSource"));

    if (val.Length() > 14 && iPos != suic::String::npos)
    {
        RelativeSource* obj = new RelativeSource();
        String cont = val.Substring(14);

        cont.Trim();
        vec.SplitString(cont, _U(","));

        for (i = 0; i < vec.Length(); ++i)
        {
            String strKey;
            String temp = vec[i];
            StringArray vecVal;

            vecVal.SplitString(temp, _U("="));

            if (vecVal.Length() != 2)
            {
                strKey = vecVal[0];
                strKey.Trim();
                bool bAncestor = strKey.Equals(_U("FindAncestor"));
                
                if (vec.Length() != 1 && !bAncestor)
                {
                    return 0;
                }

                if (strKey.Equals(_U("TemplatedParent")))
                {
                    obj->Mode = RelativeSourceMode::TemplatedParent;
                }

                if (!bAncestor)
                {
                    break;
                }
            }

            strKey = vecVal[0];
            strKey.Trim();

            if (strKey.Equals(_U("Mode")) && vecVal.Length() == 2)
            {
                String valTem = vecVal[1];
                valTem.Trim();

                if (valTem.Equals(_U("Self")))
                {
                    obj->Mode = RelativeSourceMode::Self;
                }
                else if (valTem.Equals(_U("PreviousData")))
                {
                    obj->Mode = RelativeSourceMode::PreviousData;
                }
                else if (valTem.Equals(_U("FindAncestor")))
                {
                    obj->Mode = RelativeSourceMode::FindAncestor;
                }
                else
                {
                    obj->Mode = RelativeSourceMode::TemplatedParent;
                }
            }
            else if (strKey.Equals(_U("FindAncestor")))
            {
                obj->Mode = RelativeSourceMode::FindAncestor;
            }
            else if (strKey.Equals(_U("TemplatedParent")))
            {
                obj->Mode = RelativeSourceMode::TemplatedParent;
            }
            else if (strKey.Equals(_U("AncestorLevel")) && vecVal.Length() == 2)
            {
                obj->AncestorLevel = min(1, vecVal[1].ToInt());
                obj->Mode = RelativeSourceMode::FindAncestor;
            }
            else if (strKey.Equals(_U("AncestorType")) && vecVal.Length() == 2)
            {
                String valTemp = vecVal[1];

                valTemp.Trim();
                if (valTemp.Length() > 2)
                {
                    if (valTemp[0] == _U('{'))
                    {
                        valTemp = valTemp.Substring(1, valTemp.Length() - 2);
                        valTemp.Trim();

                        int iPos = valTemp.IndexOf(_U(" "));

                        if (iPos != suic::String::npos)
                        {
                            valTemp = valTemp.Substring(iPos);
                            valTemp.Trim();
                        }
                    }
                }

                obj->Mode = RelativeSourceMode::FindAncestor;
                obj->AncestorType = BuildFactory::Ins()->Lookup(valTemp);
            }
        }

        _binding->SetSourceRef(obj);
    }

    if (iEndPos >= str.Length() - 1)
    {
        return str.Length();
    }
    else
    {
        return FindCommaPos(str, iEndPos + 1) + 1;
    }
}

bool BindParser::ParseBinding(String str)
{
    str.Trim();

    if (str.Length() < 1)
    {
        return true;
    }

    // 开始位置
    int iStart = 0;
    bool bSource = false;
    bool bPath = false;
    bool bConvertParam = false;

    for (int i = iStart; i < str.Length(); ++i)
    {
        if (str[i] == _U('='))
        {
            // 找到关键字
            String strKey = str.Substring(iStart, i - iStart);

            strKey.Trim();

            if (strKey.Equals(_U("Path")))
            {
                if (bPath)
                {
                    return false;
                }

                bPath = true;

                iStart = ParsePath(str, i + 1);

                if (iStart <= 0)
                {
                    return false;
                }

                i = iStart;
            }
            else if (strKey.Equals(_U("Converter")))
            {
                iStart = ParseConverter(str, i + 1);

                if (iStart <= 0)
                {
                    return false;
                }

                i = iStart;
            }
            else if (strKey.Equals(_U("ConverterParam")))
            {
                if (bConvertParam)
                {
                    return false;
                }

                bConvertParam = true;
                iStart = ParseConverterParam(str, i + 1);

                if (iStart <= 0)
                {
                    return false;
                }

                i = iStart;
            }
            else if (strKey.Equals(_U("ElementName")))
            {
                if (bSource)
                {
                    return false;
                }

                bSource = true;
                iStart = ParseElementName(str, i + 1);

                if (iStart <= 0)
                {
                    return false;
                }

                i = iStart;
            }
            else if (strKey.Equals(_U("RelativeSource")))
            {
                if (bSource)
                {
                    return false;
                }

                bSource = true;
                iStart = ParseRelativeSource(str, i + 1);

                if (iStart <= 0)
                {
                    return false;
                }

                i = iStart;
            }
            else if (strKey.Equals(_U("Source")))
            {
                if (bSource)
                {
                    return false;
                }

                bSource = true;
                iStart = ParseSource(str, i + 1);

                if (iStart <= 0)
                {
                    return false;
                }

                i = iStart;
            }
            else if (strKey.Equals(_U("Mode")))
            {
                iStart = ParseMode(str, i + 1);

                if (iStart <= 0)
                {
                    return false;
                }

                i = iStart;
            }
            else if (strKey.Equals(_U("UpdateSourceTrigger")))
            {
                iStart = ParseUpdateSourceTrigger(str, i + 1);

                if (iStart <= 0)
                {
                    return false;
                }

                i = iStart;
            }
            else
            {
                return false;
            }
        }
        else if (iStart == 0 && str[i] == _U(','))
        {
            PropertyPath ppath;
            String strVal;

            bPath = true;
            strVal = str.Substring(iStart, i - iStart);
            strVal.Trim();
            ppath.Path = strVal;
            _binding->SetPath(ppath);
            iStart = i + 1;
        }
    }

    if (iStart == 0 && !bPath)
    {
        PropertyPath ppath;
        String strVal = str;

        strVal.Trim();
        ppath.Path = strVal;
        _binding->SetPath(ppath);
    }

    if (NULL != _binding && _binding->GetSourceRef() == NULL)
    {
        PropertyPath ppath = _binding->GetPath();

        if (ppath.Path.IndexOf(_U("TemplatedParent.")) == 0)
        {
            RelativeSource* obj = new RelativeSource();

            obj->Mode = RelativeSourceMode::TemplatedParent;
            _binding->SetSourceRef(obj);

            ppath.Path = ppath.Path.Substring(16);
            _binding->SetPath(ppath);
        }
    }

    return true;
}

}
