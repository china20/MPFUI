
#include <System/Resources/XamlWalker.h>
#include <System/Types/String.h>

#include <System/Windows/VisualStates.h>
#include <System/Windows/BuildFactory.h>
#include <System/Windows/MemberVisitor.h>

#include <System/Resources/StaticResource.h>

#include <System/Tools/StyleHelper.h>
#include <System/Tools/EventHelper.h>

#include <Framework/Controls/Grid.h>
#include <Framework/Controls/Menu.h>
#include <Framework/Controls/Track.h>
#include <Framework/Controls/GridView.h>

namespace suic 
{

XamlWalker::XamlWalker()
{
    _xamlDoc = NULL;
    _app = NULL;
    _resDic = NULL;
    _mainWnd = NULL;
}

XamlWalker::~XamlWalker()
{
    if (NULL != _xamlDoc)
    {
        _xamlDoc->unref();
    }
}

bool XamlWalker::WalkUri(Element* parent, Object* component, const String& strUri)
{
    if (strUri.Empty())
    {
        return false;
    }
	else 
    {
        _xamlDoc = XamlTools::CreateXamlDoc();
        _xamlDoc->LoadUri(strUri);
        return WalkNode(parent, component, _xamlDoc->GetRootNode());
    }
}

bool XamlWalker::WalkMemory(Element* parent, Object* component, const Byte* data, int size)
{
	_xamlDoc = XamlTools::CreateXamlDoc();
	_xamlDoc->LoadMemory(data, size);
	return WalkNode(parent, component, _xamlDoc->GetRootNode());
}

bool XamlWalker::WalkNode(Element* parent, Object* component, IXamlNode* pNode)
{
    bool bSucc = true;
    IXamlNode* pRoot = pNode;
    if (NULL != pRoot)
    {
        suic::String strName = pRoot->GetName();
        suic::RTTIOfInfo* rootInfo = BuildFactory::Ins()->Lookup(strName);

        if (NULL != component)
        {
            _root = component;
        }
        else
        {
            CreateElement(pRoot, strName, _root);
        }

        _app = RTTICast<suic::Application>(_root.get());
        _resDic = RTTICast<suic::ResourceDictionary>(_root.get());
        _mainWnd = RTTICast<suic::Window>(_root.get());

        if (rootInfo->InheritFrom(suic::Application::RTTIType()))
        {
            if (NULL == _app)
            {
                _app = new suic::Application();
            }
        }
        else if (NULL != _app)
        {
            return false;
        }

        // Application节点
        if (NULL != _app)
        {
            Application::SetCurrent(_app);
            ReadXamlApplication(_app, pRoot);
        }
        // ResourceDictionary节点
        else if (NULL != _resDic)
        {
            ReadXamlResource(NULL, _resDic, pRoot, false);
        }
        // 控件元素节点
        else
        {
            suic::FrameworkElement* feRoot = CASTTOFE(_root.get());
            if (NULL != feRoot)
            {
                ReadContext rCtx;

                rCtx.d = feRoot;
                rCtx.fe = feRoot;
                rCtx.ac = NULL;

                bool bSetParent = false;

                if (NULL != parent && rCtx.fe->GetParent() == NULL)
                {
                    bSetParent = true;
                    MemberVisitor::SetParent(parent, rCtx.fe);
                }

                WalkXamlElement(NULL, rCtx, pRoot);

                if (bSetParent)
                {
                    MemberVisitor::SetParent(NULL, rCtx.fe);
                }
            }
        }
    }

    return bSucc;
}

void XamlWalker::CreateElement(IXamlNode* pNode, const String& name, ObjectPtr& child)
{
    if (_mainWnd != NULL)
    {
        _mainWnd->OnBuild(pNode, child);
    }

    if (NULL == child.get() && NULL != Application::Current())
    {
        Application::Current()->OnBuild(pNode, child);
    }

    if (NULL == child.get())
    {
        ResourceParserOp::CreateElement(name, child);
    }
}

void XamlWalker::WalkXamlElement(suic::FrameworkElement* fe, ReadContext& rCtx, IXamlNode* pNode)
{
    Array<ExtItem, false> extProps;

    extProps.Resize(8);
    // 读取节点的属性
    ReadXamlProperties(rCtx, pNode, extProps);

    if (NULL != rCtx.fe)
    {
        rCtx.fe->BeginInit();
    }

    suic::FrameworkElement* resFe = (fe == NULL ? rCtx.fe : fe);
    suic::String namePrefix = pNode->GetName() + _U(".");

    while (pNode->HasNext())
    {
        IXamlNode* pChildNode = pNode->Current();
        suic::String strName = pChildNode->GetName();
        int iPos = strName.IndexOf(namePrefix);

        if (NULL == rCtx.fe || !rCtx.fe->OnReadingChild(pChildNode))
        {
            strName.Trim();

            // 可能是资源或属性
            if (iPos >= 0)
            {
                suic::String strProp = strName.Substring(namePrefix.Length());
                strProp.Trim();
                // 如果是xxx.Resources，则读取控件元素资源
                if (strProp.Equals(_U("Resources")))
                {
                    ReadElementResource(rCtx.fe, pChildNode);
                }
                // 读取一般属性值
                else
                {
                    if (pChildNode->HasNext())
                    {                    
                        // 查询属性
                        suic::DpProperty* dp = DpProperty::Lookup(strProp, rCtx.d->GetRTTIType());
                        IXamlNode* xamlChild = pChildNode->Current();

                        if (NULL != dp)
                        {
                            suic::String strKey;
                            ResourceItem resItem;

                            ReadMetaResource(rCtx.d->GetRTTIType(), resFe, xamlChild->GetName(), xamlChild, resItem, strKey, false);

                            if (resItem.type > Resource::ResType::resError)
                            {
                                if (resItem.type < Resource::ResType::resExtension)
                                {
                                    rCtx.d->SetValue(dp, resItem.res.get());
                                }
                                else
                                {
                                    throw InvalidValueException(_U("not extension for property of element"), __FILELINE__);
                                }
                            }
                        }
                    }
                }
            }
            // 子节点
            else
            {
                // 首先创建子节点对象
                suic::ObjectPtr child;
                CreateElement(pChildNode, strName, child);

                if (NULL != child.get())
                {
                    if (NULL == rCtx.ac)
                    {
                        rCtx.ac = dynamic_cast<suic::IAddChild*>(rCtx.d);
                    }

                    if (NULL != rCtx.ac)
                    {
                        rCtx.ac->AddChild(child.get());
                    }

                    if (_mainWnd != NULL)
                    {
                        _mainWnd->OnConnect(pChildNode, child.get());
                    }

                    ReadContext rChildCtx;

                    rChildCtx.ac = NULL;

                    suic::DpObject* d = RTTICast<suic::DpObject>(child.get());
                    if (NULL != d)
                    {
                        rChildCtx.d = d;
                        rChildCtx.fe = CASTTOFE(d);

                        if (NULL == rChildCtx.fe)
                        {
                            suic::MemberVisitor::SetInheritParent(rCtx.d, d);
                        }

                        // 递归读取子控件元素
                        WalkXamlElement(fe, rChildCtx, pChildNode);
                    }
                }
                else
                {
                    // 非法元素节点
                }
            }
        }
    }

    // 读取节点的属性
    //ReadXamlProperties(rCtx, pNode);
    for (int i = 0; i < extProps.GetCount(); ++i)
    {
        ObjectPtr resVal;
        const ExtItem& exten = extProps.GetItem(i);
        // 静态资源
        if (exten.type == Resource::ResType::resStaticResource)
        {
            resVal = FindRes(resFe, ((SRExtension*)exten.ext)->GetResourceKey());
        }
        else
        {
            exten.ext->ProvideValue(rCtx.d, exten.dp, resVal);
        }
        if (DpProperty::UnsetValue() != resVal.get())
        {
            rCtx.d->SetValue(exten.dp, resVal.get());
        }
        exten.ext->unref();
    }

    if (NULL != rCtx.fe)
    {
        rCtx.fe->EndInit();
    }
}

void XamlWalker::ReadXamlProperties(ReadContext& rCtx, IXamlNode* pNode, Array<ExtItem, false>& extProps)
{
	IXamlAttris* pAttris = pNode->GetAttris();
    pAttris->Reset();

	while (pAttris->HasNext())
	{
		suic::String strName = pAttris->GetName();
		suic::String strValue = pAttris->GetValue();
        suic::DpProperty* dp = NULL;

        if (strName.Length() == 7 && strName.Equals(_U("Command")))
        {
            ResourceParserOp::DoCommand(rCtx.fe, strValue);
            continue;
        }

        dp = ParseProp(rCtx.d->GetRTTIType(), strName);

		if (NULL != dp)
		{
			suic::ResourceItem resItem;
			ResourceParserOp::ReadResource(NULL, dp, strValue, resItem);

            // 读取资源可用
            if (resItem.type > Resource::ResType::resError)
            {
                // 表达式
                if (resItem.type > Resource::ResType::resTemplateBinding)
                {
                    suic::Extension* ext = (suic::Extension*)(resItem.res.get());
                    ext->ref();
                    ExtItem extItem;
                    extItem.ext = ext;
                    extItem.dp = dp;
                    extItem.type = resItem.type;
                    extProps.Add(extItem);
#ifdef _DEBUG
                    if (dynamic_cast<suic::Extension*>(resItem.res.get()) == NULL)
                    {
                        throw InvalidValueException(_U("Extension invalid"), __FILELINE__);
                    }
#endif
                }
                else if (resItem.type != Resource::ResType::resTemplateBinding)
                {
                    rCtx.d->SetValue(dp, resItem.res.get());
                }
            }
		}
		else
		{
            RoutedEvent* evt = NULL;
            if (NULL != rCtx.fe)
            {
                evt = EventHelper::FindRoutedEvent(strName, rCtx.fe->GetRTTIType());
            }

            if (NULL == evt)
            {
                ObjectPtr resVal = new OString(strValue);
                rCtx.d->SetAttribute(strName, resVal.get());
            }
            else
            {
                ResourceParserOp::DoRoutedEvent(rCtx.fe, evt, strValue);
            }
		}
	}
}

void XamlWalker::ReadResourceDictionaryFromNode(IXamlNode* pNode)
{
    bool bHasSource = pNode->ContainsAttri(_U("Source"));
    if (bHasSource && pNode->HasNext())
    {
        // 错误
    }

    _resDic = new suic::ResourceDictionary();
    _root = _resDic;
    ReadXamlResource(NULL, _resDic, pNode, false);
}

void XamlWalker::ReadXamlApplication(suic::Application* app, IXamlNode* pNode)
{
    while (pNode->HasNext())
    {
        IXamlNode* childNode = pNode->Current();
        String childName(pNode->CurrentName());

        // 如果是Resources节点，则读取资源节点
        if (childName.Equals(_U("Application.Resources")))
        {
            String strSource;
            ResourceDictionary* resDic = ParseElementResource(childNode, strSource);
            if (NULL == resDic)
            {
                // 读取资源
                ReadXamlResource(NULL, app->GetResources(), childNode, true);
            }
            else
            {
                app->SetResources(resDic);
                resDic->unref();
            }
        }
        else if (childName.Equals(_U("Application.Window")))
        {
        }
    }
    
    IXamlAttris* pAttris = pNode->GetAttris();
    while (pAttris->HasNext())
    {
        suic::String strName = pAttris->GetName();
        suic::String strValue = pAttris->GetValue();

        if (strName.Equals(_U("StartupUri")))
        {
            app->SetStartupUri(strValue);
            XamlWalker walk;
            WindowPtr mainWnd = _root.get();

            if (NULL == mainWnd.get())
            {
                mainWnd = app->GetMainWindow();
            }

            walk.WalkUri(NULL, mainWnd.get(), strValue);

            mainWnd = walk.GetRoot();

            if (NULL != mainWnd.get())
            {
                app->SetMainWindow(mainWnd.get());
            }

            break;
        }
    }
}

ResourceDictionary* XamlWalker::ParseElementResource(IXamlNode* pNode, String& strSource)
{
    ResourceDictionary* pResDic = NULL;

    if (pNode->HasNext())
    {
        IXamlNode* curNode = pNode->Current();
        String strName = curNode->GetName();

        if (strName.Length() == VisualState::lbResourceDictionary.Length() && 
            strName.Equals(VisualState::lbResourceDictionary))
        {
            if (curNode->ContainsAttri(VisualState::lbSource))
            {
                strSource = curNode->FindAttri(VisualState::lbSource);

                if (pNode->HasNext())
                {
                    throw InvalidValueException(_U("contain ResourceDictionary and other resources"), __FILELINE__);
                }

                pResDic = ResourceDictionary::FindResourceDictionary(strSource);
                if (NULL == pResDic)
                {
                    pResDic = new ResourceDictionary();
                    pResDic->ref();
                    ReadResourceDictionaryFromUri(strSource, pResDic);
                }
                else
                {
                    pResDic->ref();
                }
            }
        }
    }
    pNode->Reset();

    return pResDic;
}

void XamlWalker::ReadElementResource(suic::FrameworkElement* fe, IXamlNode* pNode)
{
    String strSource;
    ResourceDictionary* resDic = ParseElementResource(pNode, strSource);
    if (NULL == resDic)
    {
        ReadXamlResource(fe, fe->GetResources(), pNode, true);
    }
    else
    {
        fe->SetResources(resDic);
        resDic->unref();
    }
}

bool XamlWalker::ReadResourceDictionaryFromUri(const suic::String& strSource, suic::ResourceDictionary* resDic)
{
    XamlWalker walk;
    return walk.WalkUri(NULL, resDic, strSource);
}

void XamlWalker::ReadMergedResourceDictionaryFromUri(suic::ResourceDictionary* resDic, const suic::String& strSource)
{
	suic::ResourceDictionaryPtr mergedRes = new suic::ResourceDictionary();
	// 先加入，否则一些静态资源查找不到
	resDic->AddMergedDictionary(mergedRes.get());
	// 读取失败移除
	if (!ReadResourceDictionaryFromUri(strSource, mergedRes.get()))
	{
		resDic->RemoveMergedDictionary(resDic->IndexOfMergedDictionary(mergedRes.get()));
	}
}

void XamlWalker::ReadMergedDictionaries(suic::FrameworkElement* fe, suic::ResourceDictionary* resDic, IXamlNode* pNode)
{
    while (pNode->HasNext())
    {
        IXamlNode* pChildNode = pNode->Current();
        suic::String strName = pChildNode->GetName();

        if (strName.Length() == VisualState::lbResourceDictionary.Length() && 
            strName.Equals(VisualState::lbResourceDictionary))
        {
            bool bHasSource = pChildNode->ContainsAttri(_U("Source"));
            if (bHasSource)
            {
                // 错误，有了Source属性，不能有其它资源节点
                if (pChildNode->HasNext())
                {
					return;
                }
                else
                {
					suic::String strSource = pChildNode->FindAttri(_U("Source"));
					// 根据Source路径从缓存中查找是否已经读入
                    suic::ResourceDictionary* mergedRes = suic::ResourceDictionary::FindResourceDictionary(strSource);
                    if (NULL == mergedRes)
                    {
                        ReadMergedResourceDictionaryFromUri(resDic, strSource); // 按路径读取
                    }
                    else
                    {
                        resDic->AddMergedDictionary(mergedRes);
                    }
                }
            }
            else
            {
                suic::ResourceDictionary* mergedRes = new suic::ResourceDictionary();

                mergedRes->ref();
                resDic->AddMergedDictionary(mergedRes);
                ReadXamlResource(fe, mergedRes, pChildNode, false);
                mergedRes->unref();
            }
        }
    }
}

void XamlWalker::ReadSetter(suic::RTTIOfInfo* targetType, suic::FrameworkElement* fe, suic::SetterCollection* setterColl, IXamlNode* pNode, FrameworkTemplate* ft)
{
    suic::RTTIOfInfo* feInfo = targetType;
    suic::String strProp = pNode->FindAttri(VisualState::lbProperty);
    suic::String strTargetName = pNode->FindAttri(VisualState::lbTargetName);
    suic::DpProperty* dp = NULL;
    suic::Setter* pSetter = NULL;

    strTargetName.Trim();

    if (ft != NULL && !strTargetName.Empty())
    {
        ChildNameIndex nameIndex = ft->ChildNameIndexFromChildName(strTargetName);
        feInfo = nameIndex.typecls;
    }

    dp = ParseProp(feInfo, strProp);
    
    if (NULL != dp)
    {
        ResourceItem resSetter;
        if (pNode->HasNext())
        {
            IXamlNode* pSetterValue = pNode->Current();
            if (NULL != pSetterValue && pSetterValue->HasNext())
            {
                IXamlNode* resNode = pSetterValue->Current();
                suic::String strName = resNode->GetName();
                ReadBaseMetaResource(targetType, fe, strName, resNode, resSetter);
                if (resSetter.res.get() != DpProperty::UnsetValue())
                {
                    pSetter = new suic::Setter();
                    pSetter->SetValue(resSetter.res.get());
                }
            }
        }
        else
        {
            ObjectPtr resVal;
            suic::String strVal = pNode->FindAttri(VisualState::lbValue);

            ResourceParserOp::ReadResource(NULL == ft ? NULL : targetType, dp, strVal, resSetter);

            if (resSetter.type >= Resource::ResType::resExtension)
            {
                if (resSetter.type == Resource::ResType::resStaticResource)
                {
                    resVal = DoStaticResource(fe, resSetter);
                }
                else
                {
                    suic::Extension* ext = (suic::Extension*)(resSetter.res.get());
                    ext->ProvideValue(fe, dp, resVal);
                }
            }
            else
            {
                resVal = resSetter.res;
            }

            if (resVal.get() != DpProperty::UnsetValue())
            {
                pSetter = new suic::Setter();
                pSetter->SetValue(resVal.get());
            }
        }

        if (NULL != pSetter)
        {
            pSetter->SetProperty(dp);
            pSetter->SetTargetName(strTargetName);
            setterColl->AddSetter(pSetter);
        }
    }
}

void XamlWalker::ReadTrigger(suic::RTTIOfInfo* targetType, suic::FrameworkElement* fe, suic::TriggerCollection* trgColl, IXamlNode* pNode, FrameworkTemplate* ft)
{
    Trigger* pTrg = new Trigger();
    suic::String strProp = pNode->FindAttri(VisualState::lbProperty);
    suic::String trgVal = pNode->FindAttri(VisualState::lbValue);
    suic::DpProperty* dp = ParseProp(targetType, strProp);
    suic::ResourceItem resItem;
    if (NULL == dp)
    {
        return;
    }
    suic::ResourceParserOp::ReadResource(NULL, dp, trgVal, resItem);

    pTrg->SetProperty(dp);
    pTrg->SetValue(resItem.res.get());

    trgColl->AddTrigger(pTrg);

    while (pNode->HasNext())
    {
        IXamlNode* pChildNode = pNode->Current();
        suic::String strName = pChildNode->GetName();

        if (strName.Length() == VisualState::lbSetter.Length() && 
            strName.Equals(VisualState::lbSetter))
        {
            ReadSetter(targetType, fe, pTrg->GetSetters(), pChildNode, ft);
        }
    }
}

void XamlWalker::ReadMultiTriggerConditions(RTTIOfInfo* tcls, MultiTrigger* pTrg, IXamlNode* node, FrameworkTemplate* ft)
{
    ArrayCondition* conds = pTrg->GetConditions();

    node->Reset();

    while (node->HasNext())
    {
        IXamlNode* condNode = node->Current();
        String condName = condNode->GetName();

        if (condName.Equals(_U("Condition")))
        {
            String prop(condNode->FindAttri(VisualState::lbProperty));
            String val(condNode->FindAttri(VisualState::lbValue));
            DpProperty* dp = ParseProp(tcls, prop);

            if (NULL == dp)
            {
                throw InvalidValueException(String().Format(_U("'prop' is invalid value")), __FILELINE__);
            }
  
            ResourceItem resItem;
            ResourceParserOp::ReadResource(tcls, dp, val, resItem);

            if (resItem.type > Resource::ResType::resError)
            {
                Condition cond;
                cond.prop = dp;
                cond.SetValue(resItem.res.get());
                conds->Add(cond);
            }
        }
    }
}

void XamlWalker::ReadMultiTrigger(suic::RTTIOfInfo* targetType, suic::FrameworkElement* fe, suic::TriggerCollection* trgColl, IXamlNode* pNode, FrameworkTemplate* ft)
{
    MultiTrigger* pTrg = new MultiTrigger();

    trgColl->AddTrigger(pTrg);

    while (pNode->HasNext())
    {
        IXamlNode* pChildNode = pNode->Current();
        suic::String strName = pChildNode->GetName();

        if (strName.Length() == VisualState::lbSetter.Length() && 
            strName.Equals(VisualState::lbSetter))
        {
            ReadSetter(targetType, fe, pTrg->GetSetters(), pChildNode, ft);
        }
        else if (strName.Length() == VisualState::lbMultiTriggerConditions.Length() &&
            strName.Equals(VisualState::lbMultiTriggerConditions))
        {
            ReadMultiTriggerConditions(targetType, pTrg, pChildNode, ft);
        }
    }
}

void XamlWalker::ReadTriggers(suic::RTTIOfInfo* targetType, suic::FrameworkElement* fe, suic::TriggerCollection* trgColl, IXamlNode* pNode, FrameworkTemplate* ft)
{
    while (pNode->HasNext())
    {
        IXamlNode* pChildNode = pNode->Current();
        suic::String strName = pChildNode->GetName();

        if (strName.Length() == VisualState::lbTrigger.Length() && 
            strName.Equals(VisualState::lbTrigger))
        {
            ReadTrigger(targetType, fe, trgColl, pChildNode, ft);
        }
        else if (strName.Length() == VisualState::lbMultiTrigger.Length() && 
            strName.Equals(VisualState::lbMultiTrigger))
        {
            ReadMultiTrigger(targetType, fe, trgColl, pChildNode, ft);
        }
    }
}

void XamlWalker::ReadStyle(suic::FrameworkElement* fe, IXamlNode* pNode, ResourceItem& resItem, String& strKey)
{
    Style* pStyle = new Style();
    suic::String strType = pNode->FindAttri(VisualState::lbTargetType);
    suic::String strRealType;
    ResourceParserOp::ParseReference(strType, strRealType);

    if (strRealType.Empty())
    {
        strRealType = strType;
        strRealType.Trim();
    }

    suic::RTTIOfInfo* targetInfo = BuildFactory::Ins()->Lookup(strRealType);

    if (strKey.Empty())
    {
        strKey = strRealType;
    }

    resItem.res = pStyle;
    resItem.type = Resource::ResType::resStyle;
    pStyle->SetTargetType(targetInfo);

    while (pNode->HasNext())
    {
        IXamlNode* pChildNode = pNode->Current();
        suic::String strName = pChildNode->GetName();

        if (strName.Length() == VisualState::lbStyleTrigger.Length() && 
            strName.Equals(VisualState::lbStyleTrigger))
        {
            ReadTriggers(targetInfo, fe, pStyle->GetTriggers() ,pChildNode, NULL);
        }
        else if (strName.Length() == VisualState::lbSetter.Length() && 
            strName.Equals(VisualState::lbSetter))
        {
            ReadSetter(targetInfo, fe, pStyle->GetSetters(), pChildNode, NULL);
        }
    }

    pStyle->Seal();
}

void XamlWalker::WalkTemplateElementProperties(suic::FrameworkElement* fe, IXamlNode* pNode, suic::FEFactory* feFactory)
{
    suic::RTTIOfInfo* tempInfo = feFactory->GetFrameworkTemplate()->GetTargetType();
    IXamlAttris* pAttris = pNode->GetAttris();

    pAttris->Reset();

    while (pAttris->HasNext())
    {
        String name = pAttris->GetName();
        String val = pAttris->GetValue();

        if (name.Equals(_U("Command")))
        {
            CommandProp* cmdProp = new CommandProp(val);
            feFactory->SetValue(NULL, cmdProp);
            continue;
        }

        if (name.Equals(_U("x:Name")))
        {
            name = _U("Name");
        }

        suic::DpProperty* dp = ParseProp(feFactory->GetType(), name);

        if (NULL != dp)
        {
            ResourceItem resItem;
            ResourceParserOp::ReadResource(tempInfo, dp, val, resItem);
            if (resItem.type > Resource::ResType::resError && 
                resItem.type != Resource::ResType::resNullable)
            {
                feFactory->MappingChildName(dp, val);

                if (resItem.type == Resource::ResType::resStaticResource)
                {
                    suic::ObjectPtr resVal;
                    resVal = DoStaticResource(fe, resItem);
                    if (DpProperty::UnsetValue() != resVal.get())
                    {
                        feFactory->SetValue(dp, resVal.get());
                    }
                }
                else
                {
                    feFactory->SetValue(dp, resItem.res.get());
                }
            }
        }
    }

    feFactory->AutoDoProperties();
}

void XamlWalker::WalkTemplateElement(suic::FrameworkElement* fe, IXamlNode* pNode, suic::FEFactory* feFactory)
{
    suic::String namePrefix = pNode->GetName() + _U(".");

    WalkTemplateElementProperties(fe, pNode, feFactory);

    pNode->Reset();

    while (pNode->HasNext())
    {
        IXamlNode* pChildNode = pNode->Current();
        suic::String strName = pChildNode->GetName();
        suic::FEFactory* feChild = new suic::FEFactory();
        suic::RTTIOfInfo* childInfo = BuildFactory::Ins()->Lookup(strName);

        if (strName.IndexOf(namePrefix, 0) != String::npos)
        {
            String kidName = strName.Substring(namePrefix.Length());
            suic::DpProperty* dp = DpProperty::Lookup(kidName, feFactory->GetType());
            if (NULL != dp)
            {
                if (pChildNode->HasNext())
                {
                    ResourceItem resItem;
                    IXamlNode* xamlChild = pChildNode->Current();

                    ReadBaseMetaResource(NULL, fe, xamlChild->GetName(), xamlChild, resItem);

                    if (resItem.type > Resource::ResType::resError)
                    {
                        feFactory->SetValue(dp, resItem.res.get());
                    }
                }

                continue;
            }
        }

        feChild->SetType(childInfo);
        feChild->SetName(strName);
        feFactory->AddChild(feChild);
        WalkTemplateElement(fe, pChildNode, feChild);
    }
}

void XamlWalker::ReadTemplateResource(suic::RTTIOfInfo* trInfo, suic::FrameworkElement* fe, IXamlNode* pNode, ResourceItem& resItem, bool fromControlTemp)
{
    suic::FrameworkTemplate* ft = NULL;
    suic::ControlTemplate* ftControl = NULL;
    suic::String strType = pNode->FindAttri(VisualState::lbTargetType);
    suic::RTTIOfInfo* targetType = BuildFactory::Ins()->Lookup(ParseKey(strType));

    if (fromControlTemp)
    {
        ftControl = new ControlTemplate();
        ft = ftControl;
    }
    else
    {
        suic::RTTIOfInfo* rInfo = BuildFactory::Ins()->Lookup(pNode->GetName());
        ft = (suic::FrameworkTemplate*)(rInfo->Create());
    }

    if (NULL == targetType)
    {
        targetType = trInfo;
    }

    ft->SetTargetType(targetType);
    resItem.res = ft;
    resItem.type = Resource::ResType::resFrameworkTemplate;

    pNode->Reset();

    while (pNode->HasNext())
    {
        IXamlNode* pChildNode = pNode->Current();
        suic::String strName = pChildNode->GetName();

        if (strName.Length() == VisualState::lbControlTemplateTrigger.Length() && 
            strName.Equals(VisualState::lbControlTemplateTrigger))
        {
            if (NULL != ftControl)
            {
                ReadTriggers(ftControl->GetTargetType(), fe, ftControl->GetTriggersAndCreate(), pChildNode, ft);
            }
        }
        else
        {
            suic::FEFactory* feFactory = new suic::FEFactory();
            suic::RTTIOfInfo* feInfo = BuildFactory::Ins()->Lookup(pChildNode->GetName());

            feFactory->SetType(feInfo);
            feFactory->SetName(strName);
            ft->SetVisualTree(feFactory);

            WalkTemplateElement(fe, pChildNode, feFactory);
        }
    }

    if (NULL != ft)
    {
        ft->Seal();
    }
}

void XamlWalker::ReadBaseMetaResource(suic::RTTIOfInfo* trInfo, suic::FrameworkElement* fe, const suic::String& strName, IXamlNode* pNode, ResourceItem& resItem)
{
    IResourceParser* resParse = ResourceParser::FindResourceParser(strName);
    if (NULL != resParse)
    {
        // 解析基本资源
        resParse->ParseNode(NULL, pNode, resItem);
    }
    else
    {
        suic::RTTIOfInfo* rInfo = BuildFactory::Ins()->Lookup(strName);
        if (NULL != rInfo)
        {
            //if (!fromTemp)
            {
                if (rInfo->InheritFrom(ControlTemplate::RTTIType()))
                {
                    ReadTemplateResource(trInfo, fe, pNode, resItem, true);
                    return;
                }
                else if (rInfo->InheritFrom(FrameworkTemplate::RTTIType()))
                {
                    ReadTemplateResource(trInfo, fe, pNode, resItem, false);
                    return;
                }
            }

            if (rInfo->InheritFrom(DpObject::RTTIType()))
            {
                DpObject* feChild = (DpObject*)(rInfo->Create());         
                ReadContext rElem;

                rElem.d = feChild;
                rElem.fe = CASTTOFE(feChild);
                rElem.ac = NULL;

                resItem.res = feChild;
                resItem.type = Resource::ResType::resLayout;

                suic::MemberVisitor::SetInheritParent(fe, feChild);

                WalkXamlElement(fe, rElem, pNode);
            }
        }
    }
}

void XamlWalker::ReadMetaResource(suic::RTTIOfInfo* trInfo, suic::FrameworkElement* fe, const suic::String& strName, IXamlNode* pNode, ResourceItem& resItem, String& strKey, bool fromTemp)
{
    strKey = pNode->FindAttri(VisualState::lbxKey);

    if (strName.Length() == VisualState::lbStyle.Length() && 
        strName.Equals(VisualState::lbStyle))
    {
        ReadStyle(fe, pNode, resItem, strKey);
    }
    else
    {
        ReadBaseMetaResource(trInfo, fe, strName, pNode, resItem);
    }    
}

void XamlWalker::ReadXamlResource(suic::FrameworkElement* fe, suic::ResourceDictionary* resDic, IXamlNode* pNode, bool fromElem)
{
    int iResCount = 0;
	bool hasResDic = false;
	bool hasMergedResDic = false;

    while (pNode->HasNext())
    {
        IXamlNode* pChildNode = pNode->Current();
        suic::String strName = pChildNode->GetName();

		++iResCount;

		// 错误
		if (hasResDic && hasMergedResDic)
		{
			return;
		}
		else if (hasResDic)
		{
			if (!fromElem || iResCount > 1)
			{
				return;
			}
		}
		else if (hasMergedResDic && fromElem)
		{
			return;
		}

        strName.Trim();
        if (strName.Length() == VisualState::lbResourceDictionary.Length() && 
            strName.Equals(VisualState::lbResourceDictionary))
        {
			hasResDic = true;
            if (fromElem)
            {
                ReadXamlResource(fe, resDic, pChildNode, false);
            }
            else
            {
                throw InvalidValueException(_U("ResourceDictionary dont have ResourceDictionary node"), __FILELINE__);
            }
        }
        else if (strName.Length() == VisualState::lbMergedDictionaries.Length() && 
            strName.Equals(VisualState::lbMergedDictionaries))
        {
			hasMergedResDic = true;
            // 读取
            if (fromElem)
            {
                // 格式错误
                throw InvalidValueException(_U("ResourceDictionary dont have ResourceDictionary.MergedDictionaries node"), __FILELINE__);
				return;
            }
            else
            {
                // 读取合并资源
                ReadMergedDictionaries(fe, resDic, pChildNode);
            }
        }
        else
        {
            // 读取基础资源，比如Style、ImageBrush、String、ControlTemplate等资源
            ResourceItem resItem;
            suic::String strKey;
            ReadMetaResource(NULL, fe, strName, pChildNode, resItem, strKey, false);

            if (!strKey.Empty())
            {
                if (resItem.res.get() != DpProperty::UnsetValue() && 
                    resItem.type < Resource::ResType::resExtension)
                {
                    resDic->Add(strKey, resItem.res.get());
                }
                else
                {
                    InvalidValueException e(String().Format(_U("invalid resource:%s"), strKey.c_str()), __FILELINE__);
                }
            }
            else
            {
                InvalidValueException e(String().Format(_U("Resource lack of x:Key Value:%s"), strName.c_str()), __FILELINE__);
            }
        }
    }

    resDic->Seal();
}

suic::String XamlWalker::ParseKey(const String& strType)
{
    suic::String strKey;
    ResourceParserOp::ParseReference(strType, strKey);
    if (strKey.Empty())
    {
        strKey = strType;
        strKey.Trim();
    }
    return strKey;
}

Object* XamlWalker::DoStaticResource(suic::FrameworkElement* fe, ResourceItem& resItem)
{
    suic::SRExtension* srExt = (suic::SRExtension*)(resItem.res.get());
    return FindRes(fe, srExt->GetResourceKey());
}

suic::DpProperty* XamlWalker::ParseProp(suic::RTTIOfInfo* rInfo, const String& strProp)
{
    suic::DpProperty* dp = ResourceParserOp::DoAttachProp(strProp);
    if (NULL == dp)
    {
        dp = DpProperty::Lookup(strProp, rInfo);
    }
    return dp;
}

suic::Object* XamlWalker::FindRes(suic::FrameworkElement* elem, const suic::String& name)
{
    if (NULL != elem)
    {
        return elem->FindRes(NULL, name);
    }
    else if (NULL != _resDic)
    {
        return _resDic->Lookup(name);
    }
    else if (NULL != _app)
    {
        return _app->GetResources()->Lookup(name);
    }
    else 
    {
        return NULL;
    }
}

//=========================================================

WalkTemplate::WalkTemplate(FEFactory* feRoot, int iCount)
{
    _bindColl = NULL;
    _count = iCount;
    _rootTemplate = feRoot;
}

AutoObj WalkTemplate::InstantiateTree(DpObject* container, DpObject* parent)
{
    FrameworkElement* feRoot = RTTICast<FrameworkElement>(_rootTemplate->GetType()->Create());
    AutoObj rootElem = feRoot;

    if (NULL != container)
    {
        _bindColl = StyleHelper::TemplatedBindField->GetValue(container);
        if (NULL == _bindColl)
        {
            _bindColl = new XArray();
            StyleHelper::TemplatedBindField->SetValue(container, _bindColl);
        }
        else
        {
            _bindColl->Clear();
        }

        _bindColl->Relength(_count);
    }

    if (InstantiateChildren(_rootTemplate, container, parent, feRoot))
    {
        return rootElem;
    }
    else
    {
        return NULL;
    }
}

bool WalkTemplate::InstantiateChildren(FEFactory* fef, DpObject* container, DpObject* parent, FrameworkElement* elem)
{
    if (NULL != fef->GetType())
    {
        RTTIOfInfo* targetClass = fef->GetFrameworkTemplate()->GetTargetType();

        if (container != NULL && targetClass != NULL && !container->GetRTTIType()->InheritFrom(targetClass))
        {
            if (container->GetRTTIType() != MenuBase::RTTIType() && targetClass != MenuItem::RTTIType())
            {
#ifdef _DEBUG
                InvalidOperationException e(String().Format(_U("Container[%s] don't map TargetType[%s]")
                    , container->GetRTTIType()->typeName, targetClass->typeName), __FILELINE__);
#endif
            }
        }

        if (elem != NULL)
        {
            elem->ref();

            fef->BeginInit(container, parent, elem);

            // 读取属性
            fef->ReadElementProperties(elem);
            fef->BindCountTemplateElement(_bindColl, elem);

            // 初始化子元素
            FEFactory* child = fef->GetFirstChild();
            while (NULL != child)
            {
                if (child->GetType() != NULL)
                {
                    FrameworkElement* childElem = NULL;
                    childElem = RTTICast<FrameworkElement>(child->GetType()->Create());
                    InstantiateChildren(child, container, elem, childElem);
                }
                else
                {
                    if (elem->GetRTTIType()->InheritFrom(Grid::RTTIType()))
                    {
                        WalkGrid(child, elem);
                    }
                    else if (elem->GetRTTIType()->InheritFrom(Track::RTTIType()))
                    {
                        WalkTrack(child, elem);
                    }
                }
                child = child->GetNextSibling();
            }

            fef->EndInit(container, elem);

            elem->unref();
        }

        return true;
    }
    else
    {
        return false;
    }
}

void WalkTemplate::WalkGrid(FEFactory* fef, FrameworkElement* elem)
{
    Grid* grid = RTTICast<Grid>(elem);
    FEFactory* tmp = fef->GetFirstChild();
    DpObject* container = grid->GetTemplatedParent();

    while (NULL != tmp)
    {
        if (tmp->GetType() == ColumnDefinition::RTTIType())
        {
            ColumnDefinition* colDef = new ColumnDefinition();

            tmp->BindCountTemplateElement(_bindColl, colDef);

            for (int i = 0; i < tmp->GetPropCount(); ++i)
            {
                PropValue* propVal = tmp->GetPropValue(i);
                colDef->SetValue(propVal->prop, propVal->GetValue());
            }

            grid->GetColumnDefs()->AddItem(colDef);
        }
        else if (tmp->GetType() == RowDefinition::RTTIType())
        {
            RowDefinition* rowDef = new RowDefinition();

            tmp->BindCountTemplateElement(_bindColl, rowDef);

            for (int i = 0; i < tmp->GetPropCount(); ++i)
            {
                PropValue* propVal = tmp->GetPropValue(i);
                rowDef->SetValue(propVal->prop, propVal->GetValue());
            }

            grid->GetRowDefs()->AddItem(rowDef);
        }

        tmp = tmp->GetNextSibling();
    }
}

void WalkTemplate::WalkTrack(FEFactory* child, FrameworkElement* elem)
{
    Track* track = RTTICast<Track>(elem);
    FEFactory* tmp = child->GetFirstChild();

    if (NULL != tmp)
    {
        if (child->GetName().Equals(_U("Track.DecreaseRepeatButton")))
        {
            RepeatButton* btn = RTTICast<RepeatButton>(tmp->GetType()->Create());
            if (btn)
            {
                track->SetDecreaseButton(btn);
                // 读取属性
                tmp->ReadElementProperties(btn);
                tmp->BindCountTemplateElement(_bindColl, btn);
            }
        }
        else if (child->GetName().Equals(_U("Track.IncreaseRepeatButton")))
        {
            RepeatButton* btn = RTTICast<RepeatButton>(tmp->GetType()->Create());

            if (btn)
            {
                track->SetIncreaseButton(btn);
                // 读取属性
                tmp->ReadElementProperties(btn);
                tmp->BindCountTemplateElement(_bindColl, btn);
            }
        }
        else if (child->GetName().Equals(_U("Track.Thumb")))
        {
            Thumb* thumb = RTTICast<Thumb>(tmp->GetType()->Create());

            if (thumb)
            {
                track->SetThumb(thumb);
                // 读取属性
                tmp->ReadElementProperties(thumb);
                tmp->BindCountTemplateElement(_bindColl, thumb);
            }
        }
    }
}

}
