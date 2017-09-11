
#include <System/Resources/BindParser.h>
#include <System/Resources/ResourceParserImpl.h>
#include <System/Resources/StaticResource.h>
#include <System/Resources/DynamicResource.h>

#include <System/Windows/CommandManager.h>
#include <System/Resources/PropertyFactory.h>

#include <System/Graphics/ImageBrush.h>
#include <System/Graphics/LinearGradientBrush.h>
#include <System/Graphics/RadialGradientBrush.h>

#include <System/Windows/VisualStates.h>
#include <System/Windows/SystemEffect.h>
#include <System/Windows/BuildFactory.h>

#include <System/Interop/System.h>

namespace suic
{

StringDic<IResourceParser*> ResourceParserI::_reses;
StringDic<IResourceParser*> ResourceParserI::_bases;

void ObjectParser::ParseValue(suic::RTTIOfInfo* riTemp, const String& val, ResourceItem& resItem)
{
    //ResourceParserOp::ParseExtension(riTemp, val, resItem);
    //resItem.type = Resource::ResType::resString;
    //resItem.res = new OString(val);
}

IResourceParser::IResourceParser(Resource::ResType t)
    : _resType(t)
{
}

void ArrayParser::ParseNode(suic::RTTIOfInfo* riTemp, IXamlNode* pNode, ResourceItem& resItem)
{
    XArray* pArray = new XArray();

    pNode->Reset();

    resItem.type = Resource::ResType::resArray;
    resItem.res = pArray;

    while (pNode->HasNext())
    {
        IXamlNode* subNode = pNode->Current();
        String strType = subNode->GetName();
        ResourceItem resSub;
        IResourceParser* resParser = ResourceParserI::FindResourceParser(strType);

        if (NULL != resParser)
        {
            resSub.type = Resource::ResType::resUnknown;
            resParser->ParseNode(NULL, subNode, resSub);
            if (resSub.type >= Resource::ResType::resUserDefine && 
                resSub.type <= Resource::ResType::resFrameworkTemplate)
            {
                pArray->Add(resSub.res.get());
            }
        }
    }
}

void DictionaryParser::ParseNode(suic::RTTIOfInfo* riTemp, IXamlNode* pNode, ResourceItem& resItem)
{
    XDictionary* pDic = new XDictionary();
    pNode->Reset();

    resItem.type= Resource::ResType::resDictionary;
    resItem.res = pDic;

    while (pNode->HasNext())
    {
        IXamlNode* subNode = pNode->Current();
        String strType = subNode->GetName();
        String strKey;
        
        strKey = subNode->FindAttri(_U("x:Key"));

        if (!strKey.Empty())
        {
            ResourceItem resSub;
            IResourceParser* resParser = ResourceParserI::FindResourceParser(strType);

            if (NULL != resParser)
            {
                resSub.type = Resource::ResType::resUnknown;
                resParser->ParseNode(NULL, subNode, resSub);
                if (resSub.type >= Resource::ResType::resUserDefine && 
                    resSub.type <= Resource::ResType::resFrameworkTemplate)
                {
                    pDic->Add(strKey, resSub.res.get());
                }
            }
        }
    }
}

IResourceParser::~IResourceParser()
{
}

void ResourceParserOp::ParseExtension(suic::RTTIOfInfo* riTemp, suic::DpProperty* dp, const String& val, ResourceItem& resItem)
{
    resItem.type = Resource::ResType::resUnknown;

    String strType;
    String strCont;
    String strRes = val;
    int pos = -1;

    strRes.Trim();

    int len = strRes.Length();

    if (len <= 2)
    {
        return;
    }
    // 引用格式非法
    else if (strRes[0] == _U('{') && strRes[len - 1] == _U('}'))
    {
        strRes = strRes.Substring(1, len - 2);
        strRes.Trim();

        if (strRes.Length() > 6 && strRes.IndexOf(_U("Binding")) == 0)
        {
            BindParser bp;

            strCont = strRes.Substring(8);
            strCont.Trim();
            resItem.res = bp.Parse(strCont);

            if (resItem.res.get() == NULL)
            {
                resItem.res = DpProperty::UnsetValue();
                resItem.type = Resource::ResType::resUnknown;
            }
            else
            {
                resItem.type = Resource::ResType::resBinding;
            }
            return;
        }
        else if (strRes.Length() >= 15 && strRes.IndexOf(_U("TemplateBinding")) == 0)
        {
            suic::DpProperty* dbBinding = NULL;

            if (NULL == riTemp)
            {
                resItem.type = Resource::ResType::resError;
                InvalidValueException e(String().Format(_U("TemplateBinding dont have TargetType attribute:%s"), 
                    strRes.c_str()), __FILELINE__);
            }
            else
            {
                strCont = strRes.Substring(16);
                strCont.Trim();
                if (strCont.Empty() && dp != NULL)
                {
                    strCont = dp->GetName();
                }

                if (!strCont.Empty())
                {
                    dbBinding = ResourceParserOp::DoAttachProp(strCont);
                    if (NULL == dbBinding)
                    {
                        dbBinding = DpProperty::Lookup(strCont, riTemp);
                    }
                    if (NULL == dbBinding)
                    {
                        resItem.type = Resource::ResType::resTbError;
                        InvalidValueException e(String().Format(_U("TemplateBinding wrong value:%s:%s"), 
                            riTemp->typeName, strCont.c_str()), __FILELINE__);
                    }
                    else
                    {
                        resItem.type = Resource::ResType::resTemplateBinding;
                        resItem.res = new TemplateBindingExtension(dbBinding);
                    }
                }
                else
                {
                    resItem.type = Resource::ResType::resTbError;
                    InvalidValueException e(String().Format(_U("TemplateBinding dont have value:%s:%s"), 
                        riTemp->typeName, strRes.c_str()), __FILELINE__);
                }
            }
        }

        if (resItem.type == Resource::ResType::resUnknown)
        {
            // 查找空格
            pos = strRes.IndexOf(_U(" "));

            // 如果不指明，则为静态资源
            if (pos == String::npos)
            {
                resItem.res = new SRExtension(strRes);
                resItem.type = Resource::ResType::resStaticResource;
            }
            else
            {
                strType = strRes.Substring(0, pos);
                strCont = strRes.Substring(pos + 1);
                strCont.Trim();

                if (strCont.Equals(_U("x:Null")))
                {
                    resItem.res = DpProperty::NullValue;
                    resItem.type = Resource::ResType::resNullable;
                }
                // 静态资源
                else if (strType.Equals(_U("StaticResource")))
                {
                    resItem.res = new SRExtension(strCont);
                    resItem.type = Resource::ResType::resStaticResource;
                }
                // 动态资源引用
                else if (strType.Equals(_U("DynamicResource")))
                {
                    resItem.res = new DRExtension(strCont);
                    resItem.type = Resource::ResType::resDynamicResource;
                }
                else if (strRes.Equals(_U("x:Type")))
                {
                    ;
                }
            }
        }

        if (resItem.type == Resource::ResType::resUnknown)
        {
            resItem.type = Resource::ResType::resError;
        }
    }
}

ResourceParserI::ResourceParserI()
{

}

void ResourceParserI::RemoveResourceParser(const String& name)
{
    _reses.Remove(name);
}

void ResourceParserI::RegisterResourceParser(const String& name, IResourceParser* resParser)
{
    _reses.Add(name, resParser);
}

IResourceParser* ResourceParserI::FindResourceParser(const String& name)
{
    InitResourceParser();

    IResourceParser* resParser = NULL;
    int iPos = name.IndexOf(_U(":"));
    if (iPos > 0)
    {
        suic::String sysFlag = name.Substring(0, iPos);
        suic::String sysName = name.Substring(iPos + 1);

        sysFlag.Trim();
        if (sysFlag.Equals(_U("sys")))
        {
            _bases.TryGetValue(sysName, resParser);
        }
        else if (sysFlag.Equals(_U("local")))
        {
            _bases.TryGetValue(sysName, resParser);
        }
    }
    else    
    {
        if (!_reses.TryGetValue(name, resParser))
        {
            _bases.TryGetValue(name, resParser);
        }
    }
    return resParser;
}

void ResourceParserI::InitResourceParser()
{
    if (_bases.GetCount() == 0)
    {
        _bases.Add(_U("String"), new BaseTypeParser<OString>(Resource::ResType::resString));
        _bases.Add(_U("Rect"), new BaseTypeParser<ORect>(Resource::ResType::resRect));
        _bases.Add(_U("RectF"), new BaseTypeParser<OfRect>(Resource::ResType::resfRect));
        _bases.Add(_U("Size"), new BaseTypeParser<OSize>(Resource::ResType::resSize));
        _bases.Add(_U("SizeF"), new BaseTypeParser<OfSize>(Resource::ResType::resfSize));
        _bases.Add(_U("Point"), new BaseTypeParser<OPoint>(Resource::ResType::resPoint));
        _bases.Add(_U("PointF"), new BaseTypeParser<OfPoint>(Resource::ResType::resfPoint));
        _bases.Add(_U("Float"), new BaseTypeParser<OFloat>(Resource::ResType::resFloat));
        
        _bases.Add(_U("Color"), new BaseTypeParser<OColor>(Resource::ResType::resColor));

        _bases.Add(_U("PathGeometry"), new BaseTypeParser<PathGeometry>(Resource::ResType::resPath));

        ObjectParser* intParser = new BaseTypeParser<Integer>(Resource::ResType::resInteger);

        _bases.Add(_U("Int32"), intParser);
        _bases.Add(_U("Integer"), intParser);

        //_bases.Add(_U("Geometry"), new GeometryParser(Resource::ResType::resPath));
        //_bases.Add(_U("Object"), new ObjectParser(Resource::ResType::resString));

        ImageSourceParser* isParser = new ImageSourceParser();
        _bases.Add(_U("Cursor"), new CursorParser());
        _bases.Add(_U("ImageSource"), isParser);
        _bases.Add(_U("Bitmap"), isParser);
        _bases.Add(_U("Boolean"), new BooleanParser());

        _bases.Add(_U("Array"), new ArrayParser());
        _bases.Add(_U("Dictionary"), new DictionaryParser());

        _bases.Add(_U("TransformGroup"), new TransformGroupParser());
        _bases.Add(_U("ScaleTransform"), new ScaleTransformParser());
        _bases.Add(_U("TranslateTransform"), new TranslateTransformParser());
        _bases.Add(_U("SkewTransform"), new SkewTransformParser());
        _bases.Add(_U("RotateTransform"), new RotateTransformParser());
        _bases.Add(_U("Rotate3DTransform"), new Rotate3DTransformParser());

        _bases.Add(_U("Brush"), new BrushParser(SolidColorBrush::RTTIType(), Resource::ResType::resBrush));
        _bases.Add(_U("SolidColorBrush"), new BrushParser(SolidColorBrush::RTTIType(), Resource::ResType::resBrush));
        _bases.Add(_U("ImageBrush"), new BrushParser(ImageBrush::RTTIType(), Resource::ResType::resBrush));
        _bases.Add(_U("LinearGradientBrush"), new BrushParser(LinearGradientBrush::RTTIType(), Resource::ResType::resBrush));
        _bases.Add(_U("RadialGradientBrush"), new BrushParser(RadialGradientBrush::RTTIType(), Resource::ResType::resBrush));
    }
}

void ResourceParserI::Clear()
{

}

void ResourceParser::RemoveResourceParser(const String& name)
{
    ResourceParserI::RemoveResourceParser(name);
}

void ResourceParser::RegisterResourceParser(const String& name, IResourceParser* resParser)
{
    ResourceParserI::RegisterResourceParser(name, resParser);
}

IResourceParser* ResourceParser::FindResourceParser(const String& name)
{
    return ResourceParserI::FindResourceParser(name);
}

//=====================================================

struct fnEventInfo
{
    FrameworkElement* fe;
    FnEventCall fnCall;
};

static bool __FindEventInfo(fnEventInfo& entryInfo, const String& name)
{
    bool bFind = false;
    FnEventDic* fnInfo = NULL;

    while (entryInfo.fe != NULL)
    {
        fnInfo = entryInfo.fe->GetEventInfo();
        if (NULL == fnInfo || !fnInfo->TryGetValue(name, entryInfo.fnCall))
        {
            FrameworkElement* fe = entryInfo.fe->GetParent();
            if (NULL == fe)
            {
                fe = CASTTOFE(entryInfo.fe->GetUIParent());
            }
            entryInfo.fe = fe;
        }
        else
        {
            bFind = true;
            break;
        }
    }

    return bFind;
}

void ResourceParserOp::CreateElement(const suic::String& name, suic::ObjectPtr& elem)
{
    CoreSystem::Ins()->BuildElement(name, elem);
}

void ResourceParserOp::DoRoutedEvent(FrameworkElement* fe, RoutedEvent* routedEvt, const suic::String& name)
{
    suic::Handler* hEvt = RTTICast<suic::Handler>(routedEvt->GetHandlerType()->Create());

    if (NULL != hEvt && NULL != fe)
    {
        fnEventInfo entryInfo;

        entryInfo.fnCall = NULL;
        entryInfo.fe = fe;

        if (__FindEventInfo(entryInfo, name))
        {
            hEvt->AddEvent(entryInfo.fe, entryInfo.fnCall);
            fe->AddHandler(routedEvt, hEvt);
        }
    }
}

DpProperty* ResourceParserOp::DoAttachProp(const String& name)
{
    AttachDpInfo attachDb;

    PropFactory::ConvertAttachProp(name, attachDb);

    if (attachDb.dp != NULL && 
        !attachDb.dp->IsAttached() && 
        !attachDb.rttiInfo->InheritFrom(attachDb.dp->GetOwnerType()))
    {
        throw InvalidValueException(String().Format(_U("'%s' is not a attach property"), name.c_str()), __FILELINE__);
    }
    return attachDb.dp;
}

void ResourceParserOp::DoCommand(FrameworkElement* fe, const String& val)
{
    if (NULL == fe)
    {
        return;
    }

    int pos = -1;
    RoutedCommand* cmd = NULL;
    ICommandSource* cmdSource = fe->GetCommandSource();

    if (NULL == cmdSource)
    {
        cmdSource = DynamicCast<ICommandSource>(fe);
    }

    if (cmdSource != NULL)
    {
        bool bHasCmd = false;
        String strCmd = val;

        strCmd.Trim();
        pos = strCmd.IndexOf(_U("{"));
        if (pos == 0)
        {
            strCmd = strCmd.Substring(1, strCmd.Length() - 2);
        }

        pos = strCmd.IndexOf(_U("Command"));
        if (pos != String::npos)
        {
            bHasCmd = true;
            strCmd = strCmd.Substring(0, pos);
        }

        pos = strCmd.IndexOf(_U("."));

        if (pos != String::npos)
        {
            bool bFind = false;
            String strType = strCmd.Substring(0, pos);
            String strName = strCmd.Substring(pos + 1);
            RTTIOfInfo* tcls = BuildFactory::Ins()->Lookup(strType);

            if (NULL == cmdSource->GetCommand())
            {
                //
                // 系统命令
                //
                if (tcls != NULL)
                {
                    CommandBinding* cmdBind = CommandManager::FindMatch(tcls, strName);
                    if (cmdBind != NULL && cmdBind->GetCommand())
                    {
                        bFind = true;
                        cmdSource->SetCommand(cmdBind->GetCommand());
                    }
                }
            }
        }
    }
}

void ResourceParserOp::ReadResource(suic::RTTIOfInfo* rInfo, const suic::String& name, IXamlNode* node, ResourceItem& resItem)
{
    IResourceParser* resParser = ResourceParser::FindResourceParser(name);
    if (NULL != resParser)
    {
        resParser->ParseNode(rInfo, node, resItem);
    }
}

void ResourceParserOp::ReadResource(suic::RTTIOfInfo* rInfo, suic::DpProperty* dp, const suic::String& val, ResourceItem& resItem)
{
    ResourceParserOp::ParseExtension(rInfo, dp, val, resItem);

    if (resItem.res.get() == DpProperty::UnsetValue() || 
        resItem.type < Resource::ResType::resExtension)
    {
        if (Resource::ResType::resTbError == resItem.type)
        {
            DpProperty* dbBind = DpProperty::Lookup(dp->GetName(), rInfo);
            if (NULL != dbBind)
            {
                resItem.type = Resource::ResType::resTemplateBinding;
                resItem.res = new TemplateBindingExtension(dbBind);
            }
            else
            {
                ;
            }
        }
        else
        {
            resItem.type = Resource::ResType::resUnknown;

            if (dp->CanConvert())
            {
                suic::OString oStr(val);
                oStr.setAutoDelete(false);
                dp->ConvertValue(&oStr, resItem.res);
                if (resItem.res.get() != DpProperty::UnsetValue())
                {
                    resItem.type = Resource::ResType::resInteger;
                    return;
                }
            }

            IResourceParser* resParser = ResourceParser::FindResourceParser(dp->GetPropType()->typeName);
            if (NULL != resParser)
            {
                resParser->ParseValue(rInfo, val, resItem);
            }
            else 
            {
                resItem.type = Resource::ResType::resString;
                resItem.res = new OString(val);
            }
        }
    }
}

int ResourceParserOp::ParseReference(const String& strType, String& strKey)
{
    int iType = Resource::ResType::resUnknown;
    String strRes = strType;
    int len = strRes.Length();

    strRes.Trim();

    // 引用格式非法
    if (strRes[0] == _U('{') && strRes[len - 1] == _U('}'))
    {
        int pos = -1;

        if (len > 2)
        {
            strRes = strRes.Substring(1, len - 2);
            strRes.Trim();

            if (strRes.IndexOf(_U("Binding")) == 0)
            {
                strKey = strRes.Substring(8);
                iType = Resource::ResType::resBinding;
            }
            else if (strRes.IndexOf(_U("TemplateBinding ")) == 0)
            {
                strKey = strRes.Substring(15);
                iType = Resource::ResType::resTemplateBinding;
            }

            if (iType == Resource::ResType::resUnknown)
            {
                String strType;

                // 查找空格
                pos = strRes.IndexOf(_U(" "));

                // 如果不指明，则为静态资源
                if (pos == String::npos)
                {
                    strKey = strRes;
                    iType = Resource::ResType::resStaticResource;
                }
                else
                {
                    strType = strRes.Substring(0, pos);
                    strKey = strRes.Substring(pos + 1);

                    if (strKey.Equals(_U("x:Null")))
                    {
                        iType = Resource::ResType::resNullable;
                    }
                    // 静态资源
                    else if (strKey.Equals(_U("StaticResource")))
                    {
                        iType = Resource::ResType::resStaticResource;
                    }
                    // 动态资源引用
                    else if (strKey.Equals(_U("DynamicResource")))
                    {
                        iType = Resource::ResType::resDynamicResource;
                    }
                    else if (strKey.Equals(_U("x:Type")))
                    {
                        ;
                    }
                }
            }
        }
    }

    return iType;
}

}
