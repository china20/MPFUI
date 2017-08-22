
#include "stdafx.h"

#include <Editor/XamlWalker.h>

#include <Core/StyleNode.h>
#include <Core/SingleUndefined.h>
#include <Controls/UserFrameworkElement.h>

#include <Main/Project.h>
#include <Main/XamlRootItem.h>

#include <System/Types/String.h>

#include <System/Windows/VisualStates.h>
#include <System/Windows/BuildFactory.h>

#include <System/Resources/StaticResource.h>

#include <System/Tools/StyleHelper.h>
#include <System/Tools/EventHelper.h>

#include <Framework/Controls/Grid.h>
#include <Framework/Controls/Menu.h>
#include <Framework/Controls/Track.h>
#include <Framework/Controls/GridView.h>

namespace suic 
{

XamlWalker::XamlWalker(Project* pPrj)
{
    _xamlDoc = NULL;
    _project = pPrj;

    _app = NULL;
    _resDic = NULL;
    _mainWnd = NULL;

    // 设计器
    _dElem = NULL;
    _dApp = NULL;
    _dResDic = NULL;

    _inDesignMode = true;
    _onlyReadAppRes = false;

    DesignHelper::EnterDesignMode();
}

XamlWalker::~XamlWalker()
{
    if (NULL != _xamlDoc)
    {
        _xamlDoc->unref();
    }

    if (NULL != _dElem)
    {
        _dElem->unref();
    }

    if (NULL != _dApp)
    {
        _dApp->unref();
    }

    if (NULL != _dResDic)
    {
        _dResDic->unref();
    }

    DesignHelper::ExitDesignMode();
}

bool XamlWalker::WalkUri(Object* dObj, Object* component, const String& strUri)
{
    _xamlDoc = XamlTools::CreateXamlDoc();
    _xamlDoc->LoadUri(strUri);

    return HandleXamlDoc(dObj, component);
}

bool XamlWalker::WalkMemory(Object* dObj, Object* component, const char* strXml, int size)
{
    _xamlDoc = XamlTools::CreateXamlDoc();
    _xamlDoc->LoadMemory((suic::Byte*)strXml, size);
    return HandleXamlDoc(dObj, component);
}

bool XamlWalker::ReadTemplateRes(RTTIOfInfo* targetInfo, suic::FrameworkElement* fe, IXamlNode* pNode, ResourceItem& resItem)
{
    DResourceItem dResItem;
    suic::RTTIOfInfo* rInfo = NULL;
    suic::String strName = pNode->GetName();

    rInfo = BuildFactory::Ins()->Lookup(strName);

    if (ReadTemplateResource(targetInfo, rInfo, fe, pNode, dResItem))
    {
        resItem = dResItem.resItem;
        return true;
    }
    return false;
}

bool XamlWalker::ReadStyleRes(suic::FrameworkElement* fe, IXamlNode* pNode, ResourceItem& resItem)
{
    DResourceItem dResItem;

    ReadStyle(fe, pNode, dResItem);
    resItem = dResItem.resItem;

    return true;
}

Project* XamlWalker::GetProject() const
{
    return _project;
}

bool XamlWalker::HandleXamlDoc(Object* dObj, Object* component)
{
    bool bSucc = true;
    IXamlNode* pRoot = _xamlDoc->GetRootNode();
    if (NULL != pRoot)
    {
        bool bCheckElem = false;
        suic::String strName = pRoot->GetName();
        suic::RTTIOfInfo* rootInfo = BuildFactory::Ins()->Lookup(strName);

        if (NULL != component)
        {
            _root = component;
        }
        else if (rootInfo != NULL)
        {
            _root = rootInfo->Create();
        }
        else
        {
            return false;
        }

        _dElem = RTTICast<DesignElement>(dObj);
        _dApp = RTTICast<ApplicationNode>(dObj);
        _dResDic = RTTICast<ResourceDictionaryNode>(dObj);

        _app = RTTICast<suic::Application>(_root.get());
        _resDic = RTTICast<suic::ResourceDictionary>(_root.get());
        _mainWnd = RTTICast<suic::Window>(_root.get());

        if (rootInfo->InheritFrom(suic::Application::RTTIType()))
        {
            if (NULL == _app)
            {
                _app = new suic::Application();
                _dApp = new ApplicationNode();
            }
        }
        else if (NULL == _resDic)
        {
            if (NULL == _dElem)
            {
                _dElem = CreateDesignElement(rootInfo);
                bCheckElem = true;
            }
        }

        if (NULL != _dElem)
        {
            _dElem->ref();
        }

        SetInDesignMode(_app);
        SetInDesignMode(_resDic);

        // Application节点
        if (NULL != _app)
        {
            _dApp->ref();
            DesignHelper::SetDesignApplication(_app);
            _dApp->SetApplication(_app);
            ReadXamlApplication(_dApp, _app, pRoot);
        }
        // ResourceDictionary节点
        else if (NULL != _resDic)
        {
            _dResDic->ref();

            DResourceDic resInfo;
            resInfo.dResDic = _dResDic;
            resInfo.resDic = _resDic;
            ReadXamlResource(NULL, &resInfo, pRoot, false);
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

                SetInDesignMode(feRoot);
                _dElem->SetNodeElement(feRoot);
                WalkXamlElement(_dElem, NULL, rCtx, pRoot);
            }
        }

        if (bCheckElem && NULL == _dElem->GetUIElement())
        {
            _dElem->unref();
            _dElem = NULL;
            bSucc = false;
        }
    }
    
    if (NULL == _root.get())
    {
        bSucc = false;
    }

    return bSucc;
}

void XamlWalker::SetInDesignMode(DpObject* d)
{
    if (_inDesignMode && NULL != d)
    {
        DesignHelper::SetInDesignMode(d);
    }
}

bool XamlWalker::OnReadingTrack(DesignTrack* dTrack, Track* pTrack, IXamlNode* node)
{
    String name(node->GetName());

    if (node->HasNext())
    {
        IXamlNode* child = node->Current();

        if (name.Equals(_U("Track.DecreaseRepeatButton")))
        {
            ObjectPtr elem;
            ResourceParserOp::CreateElement(child->GetName(), elem);
            RepeatButton* pBtn = RTTICast<RepeatButton>(elem.get());

            if (NULL != pBtn)
            {
                pTrack->SetDecreaseButton(pBtn);

                // 设计器
                DesignElement* dElem = new DesignElement(dTrack->IsTemplate());
                dElem->SetNodeElement(pBtn);
                dTrack->SetDecreaseButton(dElem);
            }

            return true;
        }
        else if (name.Equals(_U("Track.IncreaseRepeatButton")))
        {
            ObjectPtr elem;
            ResourceParserOp::CreateElement(child->GetName(), elem);
            RepeatButton* pBtn = RTTICast<RepeatButton>(elem.get());

            if (NULL != pBtn)
            {
                pTrack->SetIncreaseButton(pBtn);

                // 设计器
                DesignElement* dElem = new DesignElement(dTrack->IsTemplate());
                dElem->SetNodeElement(pBtn);
                dTrack->SetIncreaseButton(dElem);
            }

            return true;
        }
        else if (name.Equals(_U("Track.Thumb")))
        {
            ObjectPtr elem;
            ResourceParserOp::CreateElement(child->GetName(), elem);
            Thumb* pThumb = RTTICast<Thumb>(elem.get());

            if (NULL != pThumb)
            {
                pTrack->SetThumb(pThumb);
                // 设计器
                DesignElement* dElem = new DesignElement(dTrack->IsTemplate());
                dElem->SetNodeElement(pThumb);
                dTrack->SetThumb(dElem);
            }
            return true;
        }
    }

    return false;
}

bool XamlWalker::OnReadingGrid(DesignGrid* dGrid, Grid* pGrid, IXamlNode* node)
{
    bool bFind = true;
    String name = node->GetName();

    if (name.Equals(_U("Grid.RowDefinitions")))
    {
        ReadGridRowDefs(dGrid, pGrid, node);
    }
    else if (name.Equals(_U("Grid.ColumnDefinitions")))
    {
        ReadGridColumnDefs(dGrid, pGrid, node);
    }
    else
    {
        bFind = false;
    }

    return bFind;
}

void XamlWalker::ReadGridRowDefs(DesignGrid* dGrid, Grid* pGrid, IXamlNode* pNode)
{
    // 读取子节点
    pNode->Reset();

    while (pNode->HasNext())
    {
        IXamlNode* child = pNode->Current();
        String name = child->GetName();

        if (name.Equals(_U("RowDefinition")))
        {
            //RowDefinition* rowDef = (RowDefinition*)(RowDefinition::RTTIType()->Create());
            IXamlAttris* attri = child->GetAttris();

            //rowDef->ref();

            attri->Reset();

            // 设计器
            int index = dGrid->GetRowCount();

            //pGrid->GetRowDefs()->AddItem(rowDef);

            SetterCollectionNode* pRow = dGrid->AddRow();

            while (attri->HasNext())
            {
                String name = attri->GetName();
                String val = attri->GetValue();

                val.Trim();

                if (val.Empty())
                {
                    continue;
                }

                DpProperty* dp = DpProperty::Lookup(name, RowDefinition::RTTIType());

                if (NULL != dp)
                {
                    ResourceItem resItem;
                    ResourceParserOp::ReadResource(NULL, dp, val, resItem);

                    if (resItem.res.get() != DpProperty::UnsetValue())
                    {
                        //rowDef->SetValue(dp, resItem.res.get());
                        // 设计器
                        if (dp == RowDefinition::MinHeightProperty)
                        {
                            dGrid->SetRowMinHeight(index, resItem.res->ToFloat());
                        }
                        else if (dp == RowDefinition::HeightProperty)
                        {
                            suic::GridSize* grdSize = RTTICast<suic::GridSize>(resItem.res.get());
                            dGrid->SetRowHeight(index, grdSize->GetGridSizeType(), grdSize->GetValue());
                        }
                        else if (dp == RowDefinition::MaxHeightProperty)
                        {
                            dGrid->SetRowMaxHeight(index, resItem.res->ToFloat());
                        }
                    }
                }
            }

            //rowDef->unref();
        }
        else
        {
            throw InvalidValueException(_U("Node of grid is error!"), __FILELINE__);
        }
    }
}

void XamlWalker::ReadGridColumnDefs(DesignGrid* dGrid, Grid* pGrid, IXamlNode* pNode)
{
    // 读取子节点
    pNode->Reset();

    while (pNode->HasNext())
    {
        IXamlNode* child = pNode->Current();
        String name = child->GetName();

        if (name.Equals(_U("ColumnDefinition")))
        {
            //ColumnDefinition* colDef = (ColumnDefinition*)(ColumnDefinition::RTTIType()->Create());
            IXamlAttris* attri = child->GetAttris();

            //colDef->ref();
            attri->Reset();

            // 设计器
            int index = dGrid->GetColumnCount();

            //pGrid->GetColumnDefs()->AddItem(colDef);

            while (attri->HasNext())
            {
                String name = attri->GetName();
                String val = attri->GetValue();

                val.Trim();

                if (val.Empty())
                {
                    continue;
                }

                DpProperty* dp = DpProperty::Lookup(name, ColumnDefinition::RTTIType());

                if (NULL != dp)
                {
                    ResourceItem resItem;
                    ResourceParserOp::ReadResource(NULL, dp, val, resItem);

                    if (resItem.res.get() != DpProperty::UnsetValue())
                    {
                        //colDef->SetValue(dp, resItem.res.get());

                        // 设计器
                        if (dp == ColumnDefinition::MinWidthProperty)
                        {
                            dGrid->SetColumnMinWidth(index, resItem.res->ToFloat());
                        }
                        else if (dp == ColumnDefinition::WidthProperty)
                        {
                            suic::GridSize* grdSize = RTTICast<suic::GridSize>(resItem.res.get());
                            dGrid->SetColumnWidth(index, grdSize->GetGridSizeType(), grdSize->GetValue());
                        }
                        else if (dp == ColumnDefinition::MaxWidthProperty)
                        {
                            dGrid->SetColumnMaxWidth(index, resItem.res->ToFloat());
                        }
                    }
                    
                }
            }

            //colDef->unref();
        }
        else
        {
            throw InvalidValueException(_U("Node of grid is error!"), __FILELINE__);
        }
    }
}

void XamlWalker::WalkXamlElement(DesignElement* dElem, suic::FrameworkElement* fe, ReadContext& rCtx, IXamlNode* pNode)
{
    Array<ExtItem> extProps;

    extProps.Resize(8);
    // 读取节点的属性
    ReadXamlProperties(dElem->GetSetterCollection(), rCtx, pNode, &extProps);

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
        bool bSkipChild = false;

        if (dElem->GetRTTIType()->InheritFrom(DesignGrid::RTTIType()))
        {
            bSkipChild = OnReadingGrid((DesignGrid*)dElem, RTTICast<Grid>(rCtx.fe), pChildNode);
        }
        else if (dElem->GetRTTIType()->InheritFrom(DesignTrack::RTTIType()))
        {
            bSkipChild = OnReadingTrack((DesignTrack*)dElem, RTTICast<Track>(rCtx.fe), pChildNode);
        }

        if (!bSkipChild)
        {
            strName.Trim();

            // 可能是资源或控件元素属性
            if (iPos >= 0)
            {
                suic::String strProp = strName.Substring(namePrefix.Length());
                strProp.Trim();
                // 如果是xxx.Resources，则读取控件元素资源
                if (strProp.Equals(_U("Resources")))
                {
                    ReadElementResource(dElem->GetResourceDictionary(), rCtx.fe, pChildNode);
                }
                // 读取一般属性值
                else
                {
                    if (pChildNode->HasNext())
                    {
                        bool bCanRegProp = false;                    
                        // 查询属性
                        suic::DpProperty* dp = DpProperty::Lookup(strProp, rCtx.d->GetRTTIType());
                        IXamlNode* xamlChild = pChildNode->Current();

                        if (NULL != dp)
                        {
                            DResourceItem dResItem;
                            ReadMetaResource(rCtx.d->GetRTTIType(), resFe, xamlChild->GetName(), xamlChild, dResItem, false);

                            if (dResItem.resItem.type > Resource::ResType::resError)
                            {
                                if (dResItem.resItem.type < Resource::ResType::resExtension)
                                {
                                    rCtx.d->SetValue(dp, dResItem.resItem.res.get());
                                    // 记录元素属性
                                    dElem->AddSetter(strProp, dp, dResItem.dItem.get());
                                    bCanRegProp = true;
                                }
                                else
                                {
                                    throw InvalidValueException(_U("not extension for property of element"), __FILELINE__);
                                }
                            }
                        }

                        // 属性没有读取成功，按未定义处理
                        if (!bCanRegProp) 
                        {
                            NodeUndefined* undefinedNode = new NodeUndefined(pChildNode);
                            undefinedNode->ref();
                            dElem->AddSetter(strProp, dp, undefinedNode);
                            undefinedNode->unref();
                        }
                    }
                }
            }
            // 子节点
            else
            {
                // 首先创建子节点对象
                suic::ObjectPtr child;
                ResourceParserOp::CreateElement(strName, child);

                if (NULL == rCtx.ac)
                {
                    rCtx.ac = dynamic_cast<suic::IAddChild*>(rCtx.d);
                }

                if (NULL != child.get())
                {
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

                        SetInDesignMode(d);

                        DesignElement* dChild = CreateDesignElement(d->GetRTTIType());

                        dChild->SetNodeElement(d);
                        // 记录子元素
                        dElem->AddXamlElement(dChild);

                        // 递归读取子控件元素
                        WalkXamlElement(dChild, fe, rChildCtx, pChildNode);
                    }
                }
                else
                {
                    // 非法元素节点
                    WalkUserXamlElement(dElem, rCtx, pChildNode);
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
            resVal = FindRes(rCtx.fe, ((SRExtension*)exten.ext)->GetResourceKey());
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

void XamlWalker::WalkUserXamlElement(DesignElement* parent, ReadContext& rParentCtx, IXamlNode* pNode)
{
    UserFrameworkElement* userElem = new UserFrameworkElement();
    SetInDesignMode(userElem);
    UserDesignElement* dChild = new UserDesignElement(pNode);
    ReadContext rCtx;

    rCtx.d = userElem;
    rCtx.fe = userElem;
    rCtx.ac = NULL;

    if (NULL != rParentCtx.ac)
    {
        rParentCtx.ac->AddChild(userElem);
    }

    dChild->SetNodeElement(userElem);
    // 记录子元素
    parent->AddXamlElement(dChild);

    ReadXamlProperties(dChild->GetSetterCollection(), rCtx, pNode, NULL);
}

void XamlWalker::ReadXamlProperties(SetterCollectionNode* dSetColl, ReadContext& rCtx, IXamlNode* pNode, Array<ExtItem>* extProps)
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
                SetterNode* setNode = new SetterNode(strName, dp, resItem.res.get());
                dSetColl->AddSetter(setNode);

                // 表达式
                if (resItem.type > Resource::ResType::resTemplateBinding)
                {
                    if (NULL != extProps)
                    {
                        suic::Extension* ext = (suic::Extension*)(resItem.res.get());
                        ext->ref();
                        ExtItem extItem;
                        extItem.ext = ext;
                        extItem.dp = dp;
                        extItem.type = resItem.type;
                        extProps->Add(extItem);
                    }
                    else
                    {
                        dSetColl->AddSetter(new UndefinedSetterNode(strName, strValue));
                    }
                }
                else if (resItem.type != Resource::ResType::resTemplateBinding)
                {
                    rCtx.d->SetValue(dp, resItem.res.get());
                }
            }
        }
        else
        {
            dSetColl->AddSetter(new UndefinedSetterNode(strName, strValue));

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
    _dResDic = new ResourceDictionaryNode();
    _dResDic->ref();
    _root = _resDic;

    DResourceDic dResInfo;
    dResInfo.dResDic = _dResDic;
    dResInfo.resDic = _resDic;

    ReadXamlResource(NULL, &dResInfo, pNode, false);
}

void XamlWalker::ReadXamlApplication(ApplicationNode* dApp, suic::Application* app, IXamlNode* pNode)
{
    while (pNode->HasNext())
    {
        IXamlNode* childNode = pNode->Current();
        String childName(pNode->CurrentName());

        // 如果是Resources节点，则读取资源节点
        if (childName.Equals(_U("Application.Resources")))
        {
            String strSource;
            DResourceDic resInfo = ParseElementResource(dApp->GetResourceDictionary(), childNode, strSource);
            if (NULL == resInfo.resDic)
            {
                DResourceDic dResInfo;
                dResInfo.dResDic = dApp->GetResourceDictionary();
                dResInfo.resDic = app->GetResources();

                dResInfo.dResDic->Clear();

                // 读取资源
                ReadXamlResource(NULL, &dResInfo, childNode, true);
            }
            else
            {
                app->SetResources(resInfo.resDic);
                resInfo.resDic->unref();
                resInfo.dResDic->unref();
            }
        }
        else if (childName.Equals(_U("Application.Window")))
        {
        }
    }

    if (!_onlyReadAppRes)
    {
        IXamlAttris* pAttris = pNode->GetAttris();
        while (pAttris->HasNext())
        {
            suic::String strName = pAttris->GetName();
            suic::String strValue = pAttris->GetValue();

            if (strName.Equals(_U("StartupUri")))
            {
                ElementRootItem* mainRootItem = RTTICast<ElementRootItem>(_project->FindRootItem(strValue));
                if (NULL != mainRootItem)
                {
                    mainRootItem->Reset();

                    app->SetStartupUri(strValue);
                    XamlWalker walk(_project);
      
                    walk.WalkUri(mainRootItem->GetRootElement(), NULL, strValue);
                    
                    Window* pWnd = RTTICast<Window>(walk.GetRoot());

                    if (NULL != pWnd)
                    {
                        dApp->SetMainRootItem(mainRootItem);
                        app->SetMainWindow(pWnd);
                        mainRootItem->SetLoaded();
                    }
                }
                break;
            }
        }
    }
}

DResourceDic XamlWalker::ParseElementResource(ResourceDictionaryNode* dResDic, IXamlNode* pNode, String& strSource)
{
    DResourceDic dResInfo;

    dResInfo.resDic = NULL;
    dResInfo.dResDic = NULL;

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

                RootItem* rootItem = _project->FindRootItem(strSource);
                ResourceDicRootItem* resRootItem = RTTICast<ResourceDicRootItem>(rootItem);

                rootItem->Reset();

                dResInfo.dResDic = resRootItem->GetResourceDicNode();

                if (!rootItem->IsLoaded() || NULL == dResInfo.dResDic)
                {
                    dResInfo.resDic = new ResourceDictionary();
                    dResInfo.resDic->ref();
                    dResInfo.dResDic = new ResourceDictionaryNode();
                    dResInfo.dResDic->ref();

                    ReadResourceDictionaryFromUri(strSource, &dResInfo);
                }
                else
                {
                    dResInfo.resDic = dResInfo.dResDic->GetResourceDictionary();
                    dResInfo.dResDic->ref();
                    dResInfo.resDic->ref();
                }

                resRootItem->SetLoaded();
            }
        }
    }
    pNode->Reset();

    return dResInfo;
}

void XamlWalker::ReadElementResource(ResourceDictionaryNode* dResDic, suic::FrameworkElement* fe, IXamlNode* pNode)
{
    String strSource;
    DResourceDic resInfo = ParseElementResource(dResDic, pNode, strSource);

    if (NULL == resInfo.resDic)
    {
        DResourceDic dResInfo;
        dResInfo.dResDic = dResDic;
        dResInfo.resDic = fe->GetResources();
        ReadXamlResource(fe, &dResInfo, pNode, true);
    }
    else
    {
        fe->SetResources(resInfo.resDic);

        resInfo.resDic->unref();
        resInfo.dResDic->unref();
    }
}

bool XamlWalker::ReadResourceDictionaryFromUri(const suic::String& strSource, DResourceDic* resDic)
{
    XamlWalker walk(_project);
    bool bSucc = false;
    RootItem* rootItem = _project->FindRootItem(strSource);
    ResourceDicRootItem* resRootItem = RTTICast<ResourceDicRootItem>(rootItem);

    rootItem->Reset();

    if (NULL != resRootItem && walk.WalkUri(resRootItem->GetResourceDicNode(), resDic->resDic, strSource))
    {
        ResourceDictionaryNode* targetDic = resDic->dResDic;

        rootItem->SetProject(_project);
        rootItem->SetLoaded();

        targetDic->SetSourceResource(resRootItem);
        //targetDic->SetResourceSourceUri(strSource);
        bSucc = true;
    }

    return bSucc;
}

void XamlWalker::ReadMergedResourceDictionaryFromUri(DResourceDic* resInfo, const suic::String& strSource)
{
    suic::ResourceDictionaryPtr mergedRes = new suic::ResourceDictionary();
    ResourceDictionaryNode* mergedDic = new ResourceDictionaryNode();

    // 先加入，否则一些静态资源查找不到
    resInfo->resDic->AddMergedDictionary(mergedRes.get());

    DResourceDic mergedResInfo;

    mergedResInfo.dResDic = mergedDic;
    mergedResInfo.resDic = mergedRes.get();

    mergedDic->ref();

    // 读取失败移除
    if (!ReadResourceDictionaryFromUri(strSource, &mergedResInfo))
    {
        resInfo->resDic->RemoveMergedDictionary(resInfo->resDic->IndexOfMergedDictionary(mergedRes.get()));
    }
    else
    {
        resInfo->dResDic->AddMergedDictionary(mergedDic);
    }

    mergedDic->unref();
}

void XamlWalker::ReadMergedDictionaries(suic::FrameworkElement* fe, DResourceDic* resInfo, IXamlNode* pNode)
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
                    RootItem* dicRoot = _project->FindRootItem(strSource);
                    ResourceDicRootItem* resRootItem = RTTICast<ResourceDicRootItem>(dicRoot);

                    if (NULL != resRootItem)
                    {
                        resRootItem->Reset();

                        // 根据Source路径从缓存中查找是否已经读入
                        ResourceDictionaryNode* mergedRes = resRootItem->GetResourceDicNode();

                        if (!dicRoot->IsLoaded() || NULL == mergedRes)
                        {
                            ReadMergedResourceDictionaryFromUri(resInfo, strSource); // 按路径读取
                        }
                        else
                        {
                            //resInfo->resDic->AddMergedDictionary(mergedRes);
                            resInfo->dResDic->AddMergedDictionary(mergedRes);
                        }

                        resRootItem->SetLoaded();
                    }
                }
            }
            else
            {
                suic::ResourceDictionary* mergedRes = new suic::ResourceDictionary();
                ResourceDictionaryNode* dMergedRes = new ResourceDictionaryNode();

                dMergedRes->ref();
                mergedRes->ref();

                resInfo->resDic->AddMergedDictionary(mergedRes);
                resInfo->dResDic->AddMergedDictionary(dMergedRes);

                DResourceDic dResInfo;

                dResInfo.dResDic = dMergedRes;
                dResInfo.resDic = mergedRes;

                ReadXamlResource(fe, &dResInfo, pChildNode, false);

                dMergedRes->unref();
                mergedRes->unref();
            }
        }
    }
}

void XamlWalker::ReadSetter(suic::RTTIOfInfo* targetType, suic::FrameworkElement* fe, DSetterColl* setterColl, IXamlNode* pNode, FrameworkTemplate* ft)
{
    suic::RTTIOfInfo* feInfo = targetType;
    suic::String strProp = pNode->FindAttri(VisualState::lbProperty);
    suic::String strTargetName = pNode->FindAttri(VisualState::lbTargetName);
    suic::DpProperty* dp = NULL;
    suic::Setter* pSetter = NULL;
    DResourceItem resSetter;

    strTargetName.Trim();

    if (ft != NULL && !strTargetName.Empty())
    {
        ChildNameIndex nameIndex = ft->ChildNameIndexFromChildName(strTargetName);
        feInfo = nameIndex.typecls;
    }

    dp = ParseProp(feInfo, strProp);

    if (NULL != dp)
    {
        if (pNode->HasNext())
        {
            IXamlNode* pSetterValue = pNode->Current();
            if (NULL != pSetterValue && pSetterValue->HasNext())
            {
                IXamlNode* resNode = pSetterValue->Current();
                suic::String strName = resNode->GetName();
                ReadBaseMetaResource(targetType, fe, strName, resNode, resSetter, false);
                if (resSetter.resItem.res.get() != DpProperty::UnsetValue())
                {
                    pSetter = new suic::Setter();
                    pSetter->SetValue(resSetter.resItem.res.get());
                }
            }
        }
        else
        {
            ObjectPtr resVal;
            suic::String strVal = pNode->FindAttri(VisualState::lbValue);
            ResourceParserOp::ReadResource(NULL == ft ? NULL : targetType, dp, strVal, resSetter.resItem);
            resSetter.dItem = resSetter.resItem.res;

            if (resSetter.resItem.type >= Resource::ResType::resExtension)
            {
                if (resSetter.resItem.type == Resource::ResType::resStaticResource)
                {
                    resVal = DoStaticResource(fe, resSetter.resItem);
                }
                else
                {
                    suic::Extension* ext = (suic::Extension*)(resSetter.resItem.res.get());
                    ext->ProvideValue(fe, dp, resVal);
                }
            }
            else
            {
                resVal = resSetter.resItem.res;
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
            setterColl->setColl->AddSetter(pSetter);
        }
    }
    
    if (resSetter.dItem.get() == NULL || resSetter.dItem.get() == suic::DpProperty::UnsetValue())
    {
        pNode->Reset();

        if (pNode->HasNext())
        {
            IXamlNode* pSetterValue = pNode->Current();
            if (NULL != pSetterValue && pSetterValue->HasNext())
            {
                IXamlNode* resNode = pSetterValue->Current();
                // 属性没有读取成功，按未定义处理
                NodeUndefined* undefinedNode = new NodeUndefined(resNode);
                resSetter.dItem = undefinedNode;
            }
        }
        else
        {
            // 属性没有读取成功，按未定义处理
            suic::String strVal = pNode->FindAttri(VisualState::lbValue);
            SingleUndefined* undefinedNode = new SingleUndefined(strProp, strVal);
            resSetter.dItem = undefinedNode;
        }
    }

    SetterNode* pSetterNode = new SetterNode(strProp, dp, resSetter.dItem.get());
    pSetterNode->SetTargetName(strTargetName);
    setterColl->dSetColl->AddSetter(pSetterNode);
}

void XamlWalker::ReadTrigger(suic::RTTIOfInfo* targetType, suic::FrameworkElement* fe, DTriggerColl* trgInfo, IXamlNode* pNode, FrameworkTemplate* ft)
{
    Trigger* pTrg = new Trigger();
    TriggerNode* dTrg = new TriggerNode();
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

    // 记录Trigger条件
    dTrg->AddValue(dp, resItem.res.get());

    trgInfo->dTrgColl->AddTriggerNode(dTrg);
    trgInfo->trgColl->AddTrigger(pTrg);

    while (pNode->HasNext())
    {
        IXamlNode* pChildNode = pNode->Current();
        suic::String strName = pChildNode->GetName();

        if (strName.Length() == VisualState::lbSetter.Length() && 
            strName.Equals(VisualState::lbSetter))
        {
            DSetterColl setterInfo;
            setterInfo.dSetColl = dTrg->GetSetterCollection();
            setterInfo.setColl = pTrg->GetSetters();
            ReadSetter(targetType, fe, &setterInfo, pChildNode, ft);
        }
    }
}

void XamlWalker::ReadMultiTriggerConditions(RTTIOfInfo* tcls, TriggerNode* dTrg, MultiTrigger* pTrg, IXamlNode* node, FrameworkTemplate* ft)
{
    ArrayCondition* conds = pTrg->GetConditions();
    SetterCollectionNode* condColl = dTrg->GetContSetterCollection();

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

                // 设计器用，增加条件
                condColl->AddSetter(new SetterNode(prop, dp, resItem.res.get()));
            }
        }
    }
}

void XamlWalker::ReadMultiTrigger(suic::RTTIOfInfo* targetType, suic::FrameworkElement* fe, DTriggerColl* trgColl, IXamlNode* pNode, FrameworkTemplate* ft)
{
    MultiTrigger* pTrg = new MultiTrigger();
    TriggerNode* dTrg = new TriggerNode();

    trgColl->dTrgColl->AddTriggerNode(dTrg);
    trgColl->trgColl->AddTrigger(pTrg);

    while (pNode->HasNext())
    {
        IXamlNode* pChildNode = pNode->Current();
        suic::String strName = pChildNode->GetName();

        if (strName.Length() == VisualState::lbSetter.Length() && 
            strName.Equals(VisualState::lbSetter))
        {
            DSetterColl setterInfo;
            setterInfo.dSetColl = dTrg->GetSetterCollection();
            setterInfo.setColl = pTrg->GetSetters();
            ReadSetter(targetType, fe, &setterInfo, pChildNode, ft);
        }
        else if (strName.Length() == VisualState::lbMultiTriggerConditions.Length() &&
            strName.Equals(VisualState::lbMultiTriggerConditions))
        {
            ReadMultiTriggerConditions(targetType, dTrg, pTrg, pChildNode, ft);
        }
    }
}

void XamlWalker::ReadTriggers(suic::RTTIOfInfo* targetType, suic::FrameworkElement* fe, DTriggerColl* trgInfo, IXamlNode* pNode, FrameworkTemplate* ft)
{
    while (pNode->HasNext())
    {
        IXamlNode* pChildNode = pNode->Current();
        suic::String strName = pChildNode->GetName();

        if (strName.Length() == VisualState::lbTrigger.Length() && 
            strName.Equals(VisualState::lbTrigger))
        {
            ReadTrigger(targetType, fe, trgInfo, pChildNode, ft);
        }
        else if (strName.Length() == VisualState::lbMultiTrigger.Length() && 
            strName.Equals(VisualState::lbMultiTrigger))
        {
            ReadMultiTrigger(targetType, fe, trgInfo, pChildNode, ft);
        }
    }
}

void XamlWalker::ReadStyle(suic::FrameworkElement* fe, IXamlNode* pNode, DResourceItem& dResItem)
{
    Style* pStyle = new Style();
    StyleNode* pStyleNode = new StyleNode();

    suic::String strType = pNode->FindAttri(VisualState::lbTargetType);
    suic::String strRealType;
    ResourceParserOp::ParseReference(strType, strRealType);

    // 设计器用
    pStyleNode->SetKey(dResItem.dKey);

    if (strRealType.Empty())
    {
        strRealType = strType;
        strRealType.Trim();
    }

    suic::RTTIOfInfo* targetInfo = BuildFactory::Ins()->Lookup(strRealType);

    if (dResItem.dKey.Empty())
    {
        dResItem.dKey = strRealType;
    }

    // 设计器用
    pStyleNode->SetTargetType(targetInfo);

    dResItem.dItem = pStyleNode;
    dResItem.resItem.res = pStyle;
    dResItem.resItem.type = Resource::ResType::resStyle;

    pStyle->SetTargetType(targetInfo);

    DTriggerColl trgInfo;
    trgInfo.dTrgColl = pStyleNode->GetTriggerCollection();
    trgInfo.trgColl = pStyle->GetTriggers();

    DSetterColl setterInfo;
    setterInfo.dSetColl = pStyleNode->GetSetterCollection();
    setterInfo.setColl = pStyle->GetSetters();

    while (pNode->HasNext())
    {
        IXamlNode* pChildNode = pNode->Current();
        suic::String strName = pChildNode->GetName();

        if (strName.Length() == VisualState::lbStyleTrigger.Length() && 
            strName.Equals(VisualState::lbStyleTrigger))
        {
            ReadTriggers(targetInfo, fe, &trgInfo, pChildNode, NULL);
        }
        else if (strName.Length() == VisualState::lbSetter.Length() && 
            strName.Equals(VisualState::lbSetter))
        {
            ReadSetter(targetInfo, fe, &setterInfo, pChildNode, NULL);
        }
    }

    pStyle->Seal();
}

void XamlWalker::WalkTemplateElementProperties(SetterCollectionNode* dSetColl, suic::FrameworkElement* fe, IXamlNode* pNode, suic::FEFactory* feFactory)
{
    suic::RTTIOfInfo* tempInfo = feFactory->GetFrameworkTemplate()->GetTargetType();
    IXamlAttris* pAttris = pNode->GetAttris();

    pAttris->Reset();

    while (pAttris->HasNext())
    {
        String name = pAttris->GetName();
        String val = pAttris->GetValue();

        if (name.Length() == VisualState::lbxKey.Length() && name.Equals(VisualState::lbxKey))
        {
            continue;
        }
        else if (name.Equals(_U("Command")))
        {
            CommandProp* cmdProp = new CommandProp(val);
            feFactory->SetValue(NULL, cmdProp);
            // 设计器用
            dSetColl->AddSetter(new UndefinedSetterNode(name, val));
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
                // 设计器用
                dSetColl->AddSetter(new SetterNode(dp->GetName(), dp, resItem.res.get()));

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

void XamlWalker::WalkTemplateElement(DesignElement* dElem, suic::FrameworkElement* fe, IXamlNode* pNode, suic::FEFactory* feFactory)
{
    suic::String strNodeName = pNode->GetName();
    suic::String namePrefix = strNodeName + _U(".");
    SetterCollectionNode* pSetterNode = dElem->GetSetterCollection();

    dElem->SetNodeName(strNodeName);
    WalkTemplateElementProperties(pSetterNode, fe, pNode, feFactory);

    pNode->Reset();

    while (pNode->HasNext())
    {
        IXamlNode* pChildNode = pNode->Current();
        suic::String strName = pChildNode->GetName();
        suic::FEFactory* feChild = suic::FEFactory::Create();
        suic::RTTIOfInfo* childInfo = NULL;

        if (strName.IndexOf(namePrefix, 0) != String::npos)
        {
            childInfo = BuildFactory::Ins()->Lookup(strNodeName);
            String kidName = strName.Substring(namePrefix.Length());
            suic::DpProperty* dp = ParseProp(childInfo, kidName);
            if (NULL != dp)
            {
                if (pChildNode->HasNext())
                {
                    DResourceItem dResItem;
                    IXamlNode* xamlChild = pChildNode->Current();

                    ReadBaseMetaResource(NULL, fe, xamlChild->GetName(), xamlChild, dResItem, true);

                    if (dResItem.resItem.type > Resource::ResType::resError)
                    {
                        feFactory->SetValue(dp, dResItem.resItem.res.get());

                        // 设计器用
                        pSetterNode->AddSetter(new SetterNode(dp->GetName(), dp, dResItem.dItem.get()));
                    }
                }

                continue;
            }
        }
        else
        {
            childInfo = BuildFactory::Ins()->Lookup(strName);
        }

        if (NULL == childInfo)
        {
            // 非法元素节点
            ReadContext rCtx;

            rCtx.d = NULL;
            rCtx.ac = NULL;
            rCtx.fe = NULL;
            WalkUserXamlElement(dElem, rCtx, pChildNode);
        }
        else
        {
            feChild->SetType(childInfo);
            feChild->SetName(strName);
            feFactory->AddChild(feChild);

            DesignElement* dFeChild = CreateDesignElement(childInfo);

            dFeChild->SetTypeInfo(childInfo);
            dFeChild->SetNodeName(strName);

            dElem->AddXamlElement(dFeChild);

            WalkTemplateElement(dFeChild, fe, pChildNode, feChild);

            dElem->OnFinishReadChildElement(dFeChild);
        }
    }
}

bool XamlWalker::ReadTemplateResource(suic::RTTIOfInfo* trInfo, suic::RTTIOfInfo* templateInfo, suic::FrameworkElement* fe, IXamlNode* pNode, DResourceItem& dResItem)
{
    suic::FrameworkTemplate* ft = NULL;
    suic::ControlTemplate* ftControl = NULL;
    suic::String strType = pNode->FindAttri(VisualState::lbTargetType);
    suic::RTTIOfInfo* targetType = BuildFactory::Ins()->Lookup(ParseKey(strType));

    if (NULL == templateInfo || !templateInfo->InheritFrom(FrameworkTemplate::RTTIType()))
    {
        // 不可识别的模版
        return false;
    }

    ft = (suic::FrameworkTemplate*)(templateInfo->Create());
    ftControl = RTTICast<ControlTemplate>(ft);

    if (NULL == targetType)
    {
        targetType = trInfo;
    }

    ft->SetTargetType(targetType);
    dResItem.resItem.res = ft;
    dResItem.resItem.type = Resource::ResType::resFrameworkTemplate;

    // 设计器用
    TemplateRootItem* dTempRoot = new TemplateRootItem();
    dResItem.dItem = dTempRoot;

    dTempRoot->SetTemplateType(templateInfo);
    dTempRoot->SetTargetType(targetType);

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
                DTriggerColl trgInfo;
                trgInfo.dTrgColl = dTempRoot->GetTriggerCollection();
                trgInfo.trgColl = ftControl->GetTriggersAndCreate();
                ReadTriggers(ftControl->GetTargetType(), fe, &trgInfo, pChildNode, ft);
            }
        }
        else
        {
            suic::FEFactory* feFactory = suic::FEFactory::Create();
            suic::RTTIOfInfo* feInfo = BuildFactory::Ins()->Lookup(pChildNode->GetName());

            feFactory->SetType(feInfo);
            feFactory->SetName(strName);
            ft->SetVisualTree(feFactory);

            // 设计器
            DesignElement* feElem = CreateDesignElement(feInfo);
            dTempRoot->SetRootElement(feElem);

            WalkTemplateElement(feElem, fe, pChildNode, feFactory);
        }
    }

    if (NULL != ft)
    {
        ft->Seal();
    }

    return true;
}

void XamlWalker::ReadBaseMetaResource(suic::RTTIOfInfo* trInfo, suic::FrameworkElement* fe, const suic::String& strName, IXamlNode* pNode, DResourceItem& dResItem, bool fromTemp)
{
    IResourceParser* resParse = ResourceParser::FindResourceParser(strName);
    if (NULL != resParse)
    {
        // 解析基本资源
        resParse->ParseNode(NULL, pNode, dResItem.resItem);
        // 设计器
        dResItem.dItem = dResItem.resItem.res;
    }
    else
    {
        suic::RTTIOfInfo* rInfo = BuildFactory::Ins()->Lookup(strName);
        if (NULL != rInfo)
        {
            if (ReadTemplateResource(trInfo, rInfo, fe, pNode, dResItem))
            {
                return;
            }
            else if (rInfo->InheritFrom(DpObject::RTTIType()))
            {
                DpObject* feChild = (DpObject*)(rInfo->Create());         
                ReadContext rElem;

                rElem.d = feChild;
                rElem.fe = CASTTOFE(feChild);
                rElem.ac = NULL;

                dResItem.resItem.res = feChild;
                dResItem.resItem.type = Resource::ResType::resLayout;

                // 设计器
                DesignElement* dElem = CreateDesignElement(rInfo);

                dResItem.dItem = dElem;
                dElem->SetNodeElement(feChild);

                WalkXamlElement(dElem, fe, rElem, pNode);
            }
        }
        else
        {
            // 当成未定义资源读取
            //NodeUndefined* undefinedNode = new NodeUndefined(pNode);
            //undefinedNode->ref();
            //dElem->AddSetter(strProp, dp, undefinedNode);
            //undefinedNode->unref();
        }
    }
}

void XamlWalker::ReadMetaResource(suic::RTTIOfInfo* trInfo, suic::FrameworkElement* fe, const suic::String& strName, IXamlNode* pNode, DResourceItem& dResItem, bool fromTemp)
{
    dResItem.dKey = pNode->FindAttri(VisualState::lbxKey);

    if (strName.Length() == VisualState::lbStyle.Length() && 
        strName.Equals(VisualState::lbStyle))
    {
        ReadStyle(fe, pNode, dResItem);
    }
    else
    {
        ReadBaseMetaResource(trInfo, fe, strName, pNode, dResItem, fromTemp);
    }    
}

void XamlWalker::ReadXamlResource(suic::FrameworkElement* fe, DResourceDic* resInfo, IXamlNode* pNode, bool fromElem)
{
    int iResCount = 0;
    bool hasResDic = false;
    bool hasMergedResDic = false;

    resInfo->dResDic->Clear();

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
                ReadXamlResource(fe, resInfo, pChildNode, false);
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
                ReadMergedDictionaries(fe, resInfo, pChildNode);
            }
        }
        else
        {
            // 读取基础资源，比如Style、ImageBrush、String、ControlTemplate等资源
            DResourceItem dResItem;
            ReadMetaResource(NULL, fe, strName, pChildNode, dResItem, false);

            if (!dResItem.dKey.Empty())
            {
                if (dResItem.resItem.res.get() != DpProperty::UnsetValue() && 
                    dResItem.resItem.type < Resource::ResType::resExtension)
                {
                    resInfo->resDic->Add(dResItem.dKey, dResItem.resItem.res.get());
                    // 设计器
                    resInfo->dResDic->AddResItem(dResItem.dKey, dResItem.dItem.get());
                }
                else
                {
                    InvalidValueException e(String().Format(_U("invalid resource:%s"), dResItem.dKey.c_str()), __FILELINE__);
                }
            }
            else
            {
                throw InvalidValueException(_U("Resource lack of x:Key Value"), __FILELINE__);
            }
        }
    }

    resInfo->resDic->Seal();
    resInfo->dResDic->SetResourceDictionary(resInfo->resDic);
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

DesignElement* XamlWalker::CreateDesignElement(suic::RTTIOfInfo* typeInfo)
{
    DesignElement* dFeChild = NULL;

    if (typeInfo->InheritFrom(Grid::RTTIType()))
    {
        dFeChild = new DesignGrid();
    }
    else if (typeInfo->InheritFrom(Track::RTTIType()))
    {
        dFeChild = new DesignTrack();
    }
    else if (typeInfo->InheritFrom(ContentControl::RTTIType()))
    {
        dFeChild = new DesignContent();
    }
    else
    {
        dFeChild = new DesignElement();
    }

    if (NULL != dFeChild)
    {
        dFeChild->SetTypeInfo(typeInfo);
    }

    return dFeChild;
}

//=========================================================

WalkTemplate::WalkTemplate(FEFactory* feRoot, int iCount)
{
    _bindColl = NULL;
    _count = iCount;
    _rootTemplate = feRoot;
}

AutoObj WalkTemplate::InstantiateTree(DpObject* container, DpObject* parent, FrameworkElement* logicalParent)
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

    if (InstantiateChildren(_rootTemplate, container, parent, logicalParent, feRoot))
    {
        return rootElem;
    }
    else
    {
        return NULL;
    }
}

bool WalkTemplate::InstantiateChildren(FEFactory* fef, DpObject* container, DpObject* parent, FrameworkElement* logicalParent, FrameworkElement* elem)
{
    if (NULL != fef->GetType())
    {
        RTTIOfInfo* targetClass = fef->GetFrameworkTemplate()->GetTargetType();

        if (container != NULL && targetClass != NULL && !container->GetRTTIType()->InheritFrom(targetClass))
        {
            if (container->GetRTTIType() != MenuBase::RTTIType() && targetClass != MenuItem::RTTIType())
            {
                InvalidOperationException e(String().Format(_U("classtype don't map[%s]"), targetClass->typeName), __FILELINE__);
            }
        }

        fef->BindCountTemplateElement(_bindColl, elem);

        if (elem != NULL)
        {
            elem->ref();

            fef->BeginInit(container, parent, elem);

            // 读取属性
            fef->ReadElementProperties(elem);

            // 初始化子元素
            FEFactory* child = fef->GetFirstChild();
            while (NULL != child)
            {
                if (child->GetType() != NULL)
                {
                    FrameworkElement* childElem = NULL;
                    childElem = RTTICast<FrameworkElement>(child->GetType()->Create());
                    InstantiateChildren(child, container, elem, logicalParent, childElem);
                }
                else
                {
                    if (elem->GetRTTIType() == Grid::RTTIType())
                    {
                        WalkGrid(child, elem);
                    }
                    else if (elem->GetRTTIType() == Track::RTTIType())
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
