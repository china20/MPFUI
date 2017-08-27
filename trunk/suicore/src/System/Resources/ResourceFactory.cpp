// ======================================================================
//
// Copyright (c) 2008-2012 Inc. All rights reserved.
//
// 作者：汪荣
// 邮箱：china0851@foxmail.com
//
// suiwgx库源码遵循CPL协议进行开源，任何个人或团体可以免费使用，但不能居于此库衍生任何商业性质的库或代码。
//
// ======================================================================

#include <System/Resources/ResourceFactory.h>
#include <System/Resources/XamlReader.h>
#include <System/Resources/ResourceDictionary.h>
#include <System/Graphics/Font.h>
#include <System/Graphics/SolidcolorBrush.h>
#include <System/Graphics/ImageBrush.h>
#include <System/Graphics/LinearGradientBrush.h>
#include <System/Graphics/Bitmap.h>

#include <System/Interop/System.h>

namespace suic 
{

ResourceFactory::ResourceFactory(ResourceDictionary* resDic)
    : _resDic(resDic)
{
}

String ReadFontResource(IXamlNode* pNode, RefBasePtr& obj)
{
    String strName;
    Font* pFont = new Font();
    XamlPropPtr pAttri = pNode->Attributes();

    obj = pFont;
    pAttri->Reset();

    while (pAttri->HasNext())
    {
        String name(pAttri->Name());
        String val(pAttri->Value());

        if (name.Equals(_T("Key")) || name.Equals(_T("x:Key")))
        {
            strName = val;
        }
        else if (name.Equals(_T("Family")))
        {
            pFont->SetFamily(val);
        }
        else if (name.Equals(_T("Size")))
        {
            pFont->SetSize(val.ToInt());
        }
        else if (name.Equals(_T("Weight")))
        {
            pFont->SetWeight(val.ToInt());
        }
        else if (name.Equals(_T("Italic")))
        {
            pFont->SetItalic(val.ToInt());
        }
        else if (name.Equals(_T("Underline")))
        {
            pFont->SetUnderline(val.ToInt());
        }
        else if (name.Equals(_T("Orientation")))
        {
        }
        else if (name.Equals(_T("Angle")))
        {
            pFont->SetAngle(val.ToInt());
        }
        else if (name.Equals(_T("StrikeOut")))
        {
            pFont->SetType(pFont->GetType() | 0x0004);
        }

        pAttri->Next();
    }

    return strName;
}

String ReadImageResource(IXamlNode* pNode, RefBasePtr& obj)
{
    String strSrc = pNode->Attribute(_T("Source"));
    String strKey = pNode->Attribute(_T("Key"));

    if (strKey.Empty())
    {
        strKey = pNode->Attribute(_T("x:Key"));
    }

    if (strSrc.Empty())
    {
        strKey = _T("");
    }
    else
    {
        BitmapPtr pImg = new Bitmap();

        if (pImg->Load(strSrc))
        {
            obj = pImg;
        }
        else
        {
            strKey = _T("");
        }
    }

    return strKey;
}

String ResourceFactory::ReadNodeAsString(IXamlNode* pNode)
{
    String strXml(_T("<"));

    strXml += pNode->Name();

    String strProp;
    XamlPropPtr pAttri = pNode->Attributes();
    pAttri->Reset();

    while (pAttri->HasNext())
    {
        String name(pAttri->Name());
        String val(pAttri->Value());

        strProp += _T(" ");
        strProp += name;
        strProp += _T("=\"");
        strProp += val;
        strProp += _T("\"");

        pAttri->Next();
    }

    strXml += strProp;

    String strChild;
    pNode->Reset();

    while (pNode->HasNext())
    {
        // 读取Setter.Value
        XamlNodePtr valPtr = pNode->ChildNode();
        String subStr(ReadNodeAsString(valPtr.get()));

        strChild += subStr;

        pNode->Next();
    }

    if (!strChild.Empty())
    {
        strXml += _T(" >");
        strXml += strChild;
        strXml += _T("</");
        strXml += pNode->Name();
        strXml += _T(" >");
    }
    else
    {
        strXml += _T(" />");
    }

    return strXml;
}

void ResourceFactory::
ReadSetter(SetterCollection* pColl, FrameworkElement* pElem, IXamlNode* pNode, bool bCall)
{
    SetterPtr ret;
    pNode->Reset();

    // 有子节点 <Setter Attri=""> </Setter>
    if (pNode->HasNext())
    {
        // 读取Setter.Value
        XamlNodePtr valPtr = pNode->ChildNode();

        valPtr->Reset();

        if (valPtr->HasNext())
        {
            valPtr = valPtr->ChildNode();

            RefBasePtr obj;

            // 读取资源
            BuildResource(pElem, valPtr.get(), obj, bCall);

            if (obj)
            {
                ret = new Setter();
                /*ret->Prop = pNode->Attribute(_T("Attri"));
                if (ret->Prop.Empty())
                {
                    ret->Prop = pNode->Attribute(_T("Property"));
                }*/
                ret->SetValue(obj);
            }
        }
    }
    else
    {
        ElementWalker walker;
        
        String strKey(pNode->Attribute(_T("Attri")));
        String strVal(pNode->Attribute(_T("Value")));

        if (strKey.Empty())
        {
            strKey = pNode->Attribute(_T("Property"));
        }

        walker.OnStartNode(ret, pNode, strKey);

        if (strVal.IndexOf(_T("{"), 0) != npos)
        {
            /*PropDependent pd;
            pd.name = strVal;
            pd.prop = strKey;
            pColl->ResourceDriven().insert(pd);*/
        }
        else
        {
            ret = ResourceFactory::HandleSetter(pElem, strKey, strVal);
        }

        walker.OnEndNode(ret, pNode, strKey);
    }

    if (ret)
    {
        pColl->AddItem(ret);
    }
}

TriggerPtr ResourceFactory::
ReadTrigger(const String& name, FrameworkElement* pElem
            , IXamlNode* pNode, int index, bool bCall)
{
    ElementWalker walker;
    TriggerPtr pTrigger(new Trigger());
    String strName(pNode->Attribute(_T("Name")));

    if (strName.Empty())
    {
        strName = pNode->Attribute(_T("Property"));
    }

    if (!name.Empty() && !strName.Empty())
    {
        strName += _U("|");
        strName += name;
    }

    //pTrigger->SetName(strName);
    //pTrigger->SetValue(pNode->Attribute(_T("Value")));

    walker.OnStartNode(pTrigger, pNode, strName);

    pNode->Reset();

    while (pNode->HasNext())
    {
        XamlNodePtr childPtr = pNode->ChildNode();
        String name = pNode->ChildName();

        if (name.Equals(_T("Setter")))
        {
            ReadSetter(pTrigger->GetSetters(), pElem, childPtr.get(), bCall);
        }

        pNode->Next();
    }

    walker.OnEndNode(pTrigger, pNode, strName);

    return pTrigger;
}

bool ReadTemplate(IXamlNode* pNode, String& strKey, RefBasePtr& obj)
{
    pNode->Reset();

    String key(pNode->Attribute(_T("Key")));
    String source(pNode->Attribute(_T("Source")));

    if (key.Empty())
    {
        key = pNode->Attribute(_T("x:Key"));
    }

    RefBasePtr tempObj;

    if (pNode->HasNext())
    {
        XamlNodePtr childPtr = pNode->ChildNode();
        XamlReader xml(NULL, true);
        tempObj = xml.InternalLoad(NULL, childPtr.get());
    }

    return false;
}

bool ReadTemplateGroup(IXamlNode* pNode, String& strKey, RefBasePtr& obj)
{
    return false;
}

void ResourceFactory::ReadTriggerGroup(TriggerCollection* trgColl, FrameworkElement* pElem, IXamlNode* pNode, bool bCall)
{
    TriggerPtr trg;
    int indexTrg = 1;
    ElementWalker walker;

    //defTrg->SetName(pNode->Attribute(_T("Name")));
    walker.OnStartNode(trgColl, pNode, _U(""));

    pNode->Reset();

    while (pNode->HasNext())
    {
        String strName(pNode->ChildName());

        if (strName.Equals(_T("State"))
            || strName.Equals(_T("Trigger")))
        {
            TriggerPtr trgPtr/*(ResourceFactory::ReadTrigger(trgColl->GetName(), pElem
                , pNode->ChildNode().get(), indexTrg, bCall))*/;

            if (trgPtr)
            {
                trgColl->AddItem(trgPtr);
                ++indexTrg;
            }
        }
        else if (strName.Equals(_T("Setter")))
        {
            if (!trg)
            {
                trg = new Trigger();
                trgColl->AddItem(trg);
            }
            ResourceFactory::ReadSetter(trg->GetSetters(), pElem, pNode->ChildNode().get(), bCall);
        }

        pNode->Next();
    }

    walker.OnEndNode(trgColl, pNode, _U(""));
}

String ResourceFactory::ReadStyleResource(FrameworkElement* pElem, IXamlNode* pNode, RefBasePtr& obj, bool bCall)
{
    String strKey = pNode->Attribute(_T("Key"));
    StylePtr pStyle(new Style());
    int indexTrg = 1;

    if (strKey.Empty())
    {
        strKey = pNode->Attribute(_T("x:Key"));
    }

    obj = pStyle;

    String strTarget(pNode->Attribute(_T("Target")));
    if (strTarget.Empty())
    {
        strTarget = pNode->Attribute(_T("TargetType"));
    }

    //pStyle->SetTargetType(strTarget);
    //pStyle->SetBasedOn(pNode->Attribute(_T("BasedOn")));
    pNode->Reset();

    ElementWalker walker;

    walker.OnStartStyle(GetResDic(), pStyle, pNode);

    while (pNode->HasNext())
    {
        XamlNodePtr childPtr = pNode->ChildNode();
        String name = pNode->ChildName();

        if (name.Equals(_T("Setter")))
        {
            ResourceFactory::ReadSetter(pStyle->GetSetters(), pElem, childPtr.get(), bCall);
        }
        else if (name.Equals(_T("State")))
        {
            TriggerPtr trgPtr(ResourceFactory::ReadTrigger(_U(""), pElem, childPtr.get(), indexTrg, bCall));

            if (trgPtr)
            {
                //defTrg->SetTrigger(trgPtr);
                ++indexTrg;
            }
        }
        else if (name.Equals(_T("StateGroup")))
        {
            /*TriggersPtr grp(ReadTriggerGroup(pElem, childPtr.get(), bCall));
            if (grp)
            {
                pStyle->SetTriggers(grp->GetName(), grp);
            }*/
        }
        else if (name.Equals(_T("Style.Triggers")) || name.Equals(_T("Triggers")))
        {
            /*TriggersPtr grp(ReadTriggerGroup(pElem, childPtr.get(), bCall));
            if (grp)
            {
                pStyle->SetTriggers(grp->GetName(), grp);
            }*/
        }

        pNode->Next();
    }

    if (strKey.Empty())
    {
        //strKey = pStyle->GetTarget();
    }

    walker.OnEndStyle(GetResDic(), pStyle, pNode);

    return strKey;
}

String ResourceFactory::
BuildResource(FrameworkElement* pElem
              , IXamlNode* pNode, RefBasePtr& obj, bool bCall)
{
    String strName(pNode->Name());
    String strKey;

    if (strName.Equals(_T("Style")))
    {
        strKey = ReadStyleResource(pElem, pNode, obj, bCall);
    }
    else
    {
        ElementWalker walker;
        walker.OnStartRes(GetResDic(), obj, pNode);

        if (strName.Equals(_T("ImageBrush")))
        {
            ImageBrush* pBrush = new ImageBrush(_T(""));

            obj = pBrush;
            pBrush->Parse(pNode);

            strKey = pBrush->GetKey();
        }
        else if (strName.Equals(_T("Font")))
        {
            strKey = ReadFontResource(pNode, obj);
        }
        else if (strName.Equals(_T("Image")))
        {
            strKey = ReadImageResource(pNode, obj);
        }
        else if (strName.Equals(_T("LinearGradientBrush")))
        {
            LinearGradientBrush* pBrush = new LinearGradientBrush();

            obj = pBrush;
            pBrush->Parse(pNode); 
            strKey = pBrush->GetKey();
        }
        else if (strName.Equals(_T("SolidColorBrush")))
        {
            SolidColorBrush* pBrush = new SolidColorBrush(0);

            obj = pBrush;
            pBrush->Parse(pNode);
            strKey = pBrush->GetKey();
        }
        else if (strName.Equals(_T("Cursor")))
        {
            CursorPtr pCursor = new OCursor();
            String source = pNode->Attribute(_T("Source"));
            strKey = pNode->Attribute(_T("Key"));

            if (strKey.Empty())
            {
                strKey = pNode->Attribute(_T("x:Key"));
            }

            if (pCursor->Load(source))
            {
                obj = pCursor;
            }
        }
        else if (strName.Equals(_T("Template")))
        {
            ReadTemplate(pNode, strKey, obj);
        }
        else if (strName.Equals(_T("TemplateGroup")))
        {
            ReadTemplateGroup(pNode, strKey, obj);
        }
        else if (strName.Equals(_T("String")))
        {
            strKey = pNode->Attribute(_T("Key"));
            obj = new OString(pNode->Attribute(_T("Value")));

            if (strKey.Empty())
            {
                strKey = pNode->Attribute(_T("x:Key"));
            }
        }

        walker.OnEndRes(GetResDic(), obj, pNode);
    }

    return strKey;
}

SetterPtr ResourceFactory::HandleSetter(FrameworkElement* pElem, const String& prop, const String& val)
{
    RefBasePtr objVal(PropertyFactory::GetProperty(prop, val));

    if (!objVal)
    {
        return SetterPtr();
    }
    else
    {
        Setter* ret = new Setter();
        DpProperty* dp = DpProperty::FromName(prop);

        if (dp)
        {
            ret->SetProperty(dp);
        }
        else
        {
            ret->SetUnresolvedProperty(prop);
        }

        ret->SetValue(objVal);

        return ret;
    }
}

RefBasePtr CalcValueObject(String key, String val);

RefBasePtr PropertyFactory::GetProperty(String prop, String val)
{
    if (prop.IndexOf(_T("MinWidth")) != -1 || prop.IndexOf(_T("MinHeight")) != -1)
    {
        return new Integer(val, 0);
    }
    else if (prop.IndexOf(_T("Width")) != -1 || prop.IndexOf(_T("Height")) != -1)
    {
        return new Integer(val, -1);
    }
    else if (prop.Equals(_T("Background")) 
        || prop.Equals(_T("Foreground"))
        || prop.Equals(_T("StrokeBrush"))
        || prop.Equals(_U("BorderBrush")))
    {
        if (val.IndexOf(_T(".")) != -1 || val.IndexOf(_T("/")) != -1)
        {
            return new ImageBrush(val);
        }
        else
        {
            Color clr = OColor::StringToColor(val);
            if (clr == COLOR_NULL)
            {
                return RefBasePtr();
            }
            else
            {
                return new SolidColorBrush(clr);
            }
        }
    }
    else if (prop.Equals(_T("Opacity")))
    {
        return new ODouble(val);
    }
    else if (prop.Equals(_T("Margin")) || prop.Equals(_T("Padding"))
         || prop.Equals(_T("StrokeThickness"))
         || prop.Equals(_U("BorderThickness")))
    {
        return new ORect(val);
    }
    else if (prop.Equals(_T("Cursor")))
    {
        CursorPtr pCursor = new OCursor();

        if (pCursor->Load(val))
        {
            return pCursor.get();
        }
        else
        {
            return RefBasePtr();
        }
    }
    else
    {
        return new OString(val);
    }
}

}
