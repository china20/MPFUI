/////////////////////////////////////////////////////
// ThemeEditorWindow.cpp

#include "stdafx.h"

#include <Editor/EditRootPanel.h>
#include <Editor/ThemeEditorWindow.h>
#include <Editor/SystemResource.h>

#include <Main/MainWindow.h>

#include <Core/StyleNode.h>
#include <Framework/Controls/Rectangle.h>

ThemeEditorWindow::ThemeEditorWindow(RootItem* root, ResourceDictionaryNode* resNode)
{
    _project = root->GetProject();
    _rootItem = root;
    //_editElem = elem;
    _resTarget = NULL;
    _prevEditCtrl = NULL;
    _resListBox = NULL;

    _editRootPanel = NULL;

    _tempEditorWnd = NULL;
    _tempTrgEditorWnd = NULL;
    _styleTrgEditorWnd = NULL;
    _stylePropEditorWnd = NULL;
    _templatePropEditorWnd = NULL;

    _styleNode = NULL;
    _templateNode = NULL;
    _resourceNode = resNode;
    _hideMainReturnButton = false;
}

ThemeEditorWindow::~ThemeEditorWindow()
{
    Dispose();
}

void ThemeEditorWindow::Dispose()
{
    /*FREEREFOBJ(_tempEditorWnd);
    FREEREFOBJ(_stylePropEditorWnd);
    FREEREFOBJ(_templatePropEditorWnd);
    FREEREFOBJ(_tempTrgEditorWnd);
    FREEREFOBJ(_styleTrgEditorWnd);*/

    FREEREFOBJ(_styleNode);
    FREEREFOBJ(_templateNode);
}

StyleNode* ThemeEditorWindow::GetStyleNode() const
{
    return _styleNode;
}

void ThemeEditorWindow::SetStyleNode(StyleNode* node)
{
    SETREFOBJ(_styleNode, node);
}

TemplateRootItem* ThemeEditorWindow::GetTemplateNode() const
{
    return _templateNode;
}

void ThemeEditorWindow::SetTemplateNode(TemplateRootItem* node)
{
    SETREFOBJ(_templateNode, node);
}

RootItem* ThemeEditorWindow::GetRootItem() const
{
    return _rootItem;
}

DesignElement* ThemeEditorWindow::GetResourceElement() const
{
    return _resTarget;
}

void ThemeEditorWindow::SetResourceElement(DesignElement* resTarget)
{
    _resTarget = resTarget;
}

void ThemeEditorWindow::SetHideMainReturnButton()
{
    _hideMainReturnButton = true;
}

bool ThemeEditorWindow::OnBuild(suic::IXamlNode* pNode, suic::ObjectPtr& obj)
{
    return false;
}

void ThemeEditorWindow::OnConnect(suic::IXamlNode* pNode, suic::Object* target)
{
}

void ThemeEditorWindow::InitChildEditPanel(const String& strPath, Element* pElem, const String& strName)
{
    suic::Panel* tempPanel = FindElem<suic::Panel>(strName);

    pElem->ref();

    if (NULL != tempPanel)
    {
        // tempPanel tbl
        ObjectPtr child = suic::XamlReader::LoadUri(tempPanel, pElem, strPath);
        if (child.get() == pElem)
        {
            tempPanel->AddChild(child.get());
        }
    }

    pElem->unref();
}

void ThemeEditorWindow::OnInitialized(EventArg* e)
{
    suic::Window::OnInitialized(e);

    String strPath("/mpfuid;/resource/uidesign/layout/editor/TemplateEditorWindow.xaml");

    _editorStatck.SetOwnerWnd(this);

    // 模版编辑页面
    _tempEditorWnd = new TemplateEditorWindow(NULL, this);
    //_tempEditorWnd->ref();
    InitChildEditPanel(strPath, _tempEditorWnd, _U("TemplatePanel"));

    // 样式属性编辑页面
    strPath = _U("/mpfuid;/resource/uidesign/layout/editor/PropEditorWindow.xaml");
    _stylePropEditorWnd = new PropEditorWindow(NULL, this, ePropertyType::ePropStyle);
    //_stylePropEditorWnd->ref();
    InitChildEditPanel(strPath, _stylePropEditorWnd, _U("StylePropertyPanel"));
    
    // 模版属性编辑页面
    strPath = _U("/mpfuid;/resource/uidesign/layout/editor/PropEditorWindow.xaml");
    _templatePropEditorWnd = new PropEditorWindow(NULL, this, ePropertyType::ePropStyleTrigger);
    //_templatePropEditorWnd->ref();
    InitChildEditPanel(strPath, _templatePropEditorWnd, _U("TemplatePropertyPanel"));

    // Style编辑页面
    strPath = _U("/mpfuid;/resource/uidesign/layout/editor/TriggEditorWindow.xaml");
    _styleTrgEditorWnd = new TriggerEditorWindow(NULL, NULL, this, eTriggerType::eTrgStyle);
    //_styleTrgEditorWnd->ref();
    InitChildEditPanel(strPath, _styleTrgEditorWnd, _U("StyleTriggersPanel"));

    // 模版Triggers编辑页面
    strPath = _U("/mpfuid;/resource/uidesign/layout/editor/TriggEditorWindow.xaml");
    _tempTrgEditorWnd = new TriggerEditorWindow(NULL, NULL, this, eTriggerType::eTrgTemplate);
    //_tempTrgEditorWnd->ref();
    InitChildEditPanel(strPath, _tempTrgEditorWnd, _U("TemplateTriggersPanel"));

    // 加入默认的View显示到显示栈
    _editorStatck.Push(_U("ResPanel"), _U("编辑资源"), _U(""));

    suic::TextBox* pXKey = FindElem<suic::TextBox>(_U("xKey"));
    if (NULL != pXKey)
    {
        pXKey->AddPreKeyDown(new suic::KeyboardEventHandler(this, &ThemeEditorWindow::OnxKeyPreDown));
        //pXKey->AddTextChanged(new RoutedEventHandler(this, &ThemeEditorWindow::OnxKeyChanged));
    }

    EditRootPanel* editPanel = FindElem<EditRootPanel>(_U("editRootPanel"));
    if (NULL != editPanel)
    {
        editPanel->SetRootItem(_rootItem);
        editPanel->InitEditPanelInfo(GetRootItem(), NULL);
    }

    if (_styleNode != NULL)
    {
        RefleshEditPanel(_U("StyleTriggersPanel"), _U("编辑样式"), _U(""));

        _styleTrgEditorWnd->FindName(_U("TriggersPanel_Return"))->SetVisibility(suic::Visibility::Collapsed);
        _styleTrgEditorWnd->FindName(_U("MainWindow_Return_FromStyle"))->SetVisibility(suic::Visibility::Visible);
        
        _styleTrgEditorWnd->SetSetterColl(_styleNode->GetSetterCollection());
        _styleTrgEditorWnd->SetTriggerCollectionNode(NULL, _styleNode->GetTriggerCollection());
        _styleTrgEditorWnd->Reflesh();
    }
    else if (_templateNode != NULL)
    {
        _tempEditorWnd->SetRootTemplateElement(_templateNode);
        RefleshEditPanel(_U("TemplatePanel"), _U("编辑模版"), _U(""));
        _tempEditorWnd->FindName(_U("TemplatePanel_Return"))->SetVisibility(suic::Visibility::Collapsed);
        _tempEditorWnd->FindName(_U("MainWindow_Return_FromTemplate"))->SetVisibility(suic::Visibility::Visible);
    }

    // 注册快捷键
    AcceleratorOp::Register(ModifierKey::mkControl, _U("c"), this);
    AcceleratorOp::Register(ModifierKey::mkControl, _U("v"), this);
}

void ThemeEditorWindow::OnxKeyPreDown(Element* sender, suic::KeyboardEventArg* e)
{
    suic::TextBox* pText = RTTICast<suic::TextBox>(sender);

    if (NULL != _prevEditCtrl && NULL != pText && e->GetKey() == suic::Key::kReturn)
    {
        e->SetHandled(true);

        String strKey = pText->GetText();
        ResNodePtr resNode = _prevEditCtrl->GetResValue();

        strKey.Trim();

        if (resNode.get() != NULL && !strKey.Empty())
        {
            SetModified();
            resNode->SetKey(strKey);
            resNode->UpdateDisplayName();
        }
    }
}

void ThemeEditorWindow::OnxKeyChanged(Element* sender, RoutedEventArg* e)
{
    e->SetHandled(true);
    suic::TextBox* pText = RTTICast<suic::TextBox>(sender);

    if (NULL != _prevEditCtrl && NULL != pText)
    {
        String strKey = pText->GetText();
        ResNodePtr resNode = _prevEditCtrl->GetResValue();
        if (resNode.get() != NULL)
        {
            strKey.Trim();
            if (strKey.Empty())
            {
                resNode->SetKey(_U(""));
                pText->SetText(resNode->GetKey());
            }
            else
            {
                resNode->SetKey(strKey);
            }

            SetModified();
            resNode->UpdateDisplayName();
        }
    }
}

suic::ListBox* ThemeEditorWindow::GetResListBox()
{
    if (NULL == _resListBox)
    {
        _resListBox = FindElem<ListBox>(_U("ResList"));
    }
    return _resListBox;
}

Project* ThemeEditorWindow::GetProject() const
{
    return _project;
}

void ThemeEditorWindow::OnLoaded(suic::LoadedEventArg* e)
{
    suic::Window::OnLoaded(e);
    suic::ListBox* resListBox = GetResListBox();

    if (NULL != resListBox && _resourceNode != NULL)
    {
        resListBox->AddSelectionChanged(new suic::SelectionChangedEventHandler(this, &ThemeEditorWindow::OnResItemSelectedChanged));
        ItemCollection* itemsColl = _resourceNode->GetResourceCollection();
        resListBox->SetItemsSource(itemsColl);

        if (itemsColl->GetCount() > 0)
        {
            resListBox->SetSelectedItem(itemsColl->GetItem(0));
        }
        resListBox->AddPreviewMouseDoubleClick(new MouseButtonEventHandler(this, &ThemeEditorWindow::OnDbClickTree));
    }

    suic::Button* pReturn = _styleTrgEditorWnd->FindElem<suic::Button>(_U("TriggersPanel_Return"));
    if (NULL != pReturn)
    {
        pReturn->AddClick(new ClickEventHandler(this, &ThemeEditorWindow::OnReturnPrePanel));
    }

    pReturn = _tempTrgEditorWnd->FindElem<suic::Button>(_U("TriggersPanel_Return"));
    if (NULL != pReturn)
    {
        pReturn->AddClick(new ClickEventHandler(this, &ThemeEditorWindow::OnReturnPrePanel));
    }

    pReturn = _stylePropEditorWnd->FindElem<suic::Button>(_U("PropertyPanel_Return"));
    if (NULL != pReturn)
    {
        pReturn->AddClick(new ClickEventHandler(this, &ThemeEditorWindow::OnReturnPrePanel));
    }

    pReturn = _templatePropEditorWnd->FindElem<suic::Button>(_U("PropertyPanel_Return"));
    if (NULL != pReturn)
    {
        pReturn->AddClick(new ClickEventHandler(this, &ThemeEditorWindow::OnReturnPrePanel));
    }

    pReturn = FindElem<suic::Button>(_U("TemplatePanel_Return"));
    if (NULL != pReturn)
    {
        pReturn->AddClick(new ClickEventHandler(this, &ThemeEditorWindow::OnReturnPrePanel));
    }


    pReturn = FindElem<suic::Button>(_U("MainWindow_Return"));
    if (NULL != pReturn)
    {
        if (_hideMainReturnButton)
        {
            pReturn->SetVisibility(suic::Visibility::Collapsed);
        }
        pReturn->AddClick(new ClickEventHandler(this, &ThemeEditorWindow::OnReturnMainPanel));
    }

    pReturn = FindElem<suic::Button>(_U("MainWindow_Return_FromStyle"));
    if (NULL != pReturn)
    {
        pReturn->AddClick(new ClickEventHandler(this, &ThemeEditorWindow::OnReturnMainPanel));
    }

    pReturn = FindElem<suic::Button>(_U("MainWindow_Return_FromTemplate"));
    if (NULL != pReturn)
    {
        pReturn->AddClick(new ClickEventHandler(this, &ThemeEditorWindow::OnReturnMainPanel));
    }

    // 响应增加资源按钮
    suic::Button* pAddRes = FindElem<suic::Button>(_U("AddResBtn"));
    if (NULL != pAddRes)
    {
        pAddRes->AddClick(new ClickEventHandler(this, &ThemeEditorWindow::OnAddResButton));
    }

    suic::Button* pOpRes = FindElem<suic::Button>(_U("DelResBtn"));
    if (NULL != pOpRes)
    {
        pOpRes->AddClick(new ClickEventHandler(this, &ThemeEditorWindow::OnOpResButton));
    }

    pOpRes = FindElem<suic::Button>(_U("UpResBtn"));
    if (NULL != pOpRes)
    {
        pOpRes->AddClick(new ClickEventHandler(this, &ThemeEditorWindow::OnOpResButton));
    }

    pOpRes = FindElem<suic::Button>(_U("DownResBtn"));
    if (NULL != pOpRes)
    {
        pOpRes->AddClick(new ClickEventHandler(this, &ThemeEditorWindow::OnOpResButton));
    }
}

void ThemeEditorWindow::RefleshStackShow()
{
    _editorStatck.Pop();
}

void ThemeEditorWindow::RefleshEditPanel(const String& name, const String& title, const String& returnName)
{
    _editorStatck.Push(name, title, returnName);
}

void ThemeEditorWindow::OnReturnPrePanel(Element* sender, RoutedEventArg* e)
{
    e->SetHandled(true);
    RefleshStackShow();
}

void ThemeEditorWindow::OnReturnMainPanel(Element* sender, RoutedEventArg* e)
{
    e->SetHandled(true);
    MainWindow* mainWnd = dynamic_cast<MainWindow*>(suic::Application::Current()->GetMainWindow());

    if (_mainReturnEvent.safe_bool())
    {
        _mainReturnEvent(this, &suic::EventArg::Empty);
    }

    if (NULL != mainWnd)
    {
        mainWnd->SwitchToMainView();
    }
}

void ThemeEditorWindow::OnAccessKey(suic::AccessKeyEventArg* e)
{
    if (GetResListBox()->IsVisible() && GetResListBox()->IsKeyboardFocusWithin())
    {
        if (e->GetKey().Equals(_U("c")))
        {
            ;
        }
        else if (e->GetKey().Equals(_U("v")))
        {
            ;
        }
    }
}

void ThemeEditorWindow::ShowPropWindow()
{
    
}

void ThemeEditorWindow::SetModified()
{
    if (_rootItem != NULL)
    {
        _rootItem->SetModified(true);
    }
}

void ThemeEditorWindow::SetMainReturnEvent(suic::EventHandler h)
{
    _mainReturnEvent.Clear();
    _mainReturnEvent += h;
}

PropEditorWindow* ThemeEditorWindow::ShowStylePropEditorWindow()
{
    RefleshEditPanel(_U("StylePropertyPanel"), _U("编辑样式属性"), _U("PropertyPanel_Return"));
    return _stylePropEditorWnd;
}

PropEditorWindow* ThemeEditorWindow::ShowTemplatePropEditorWindow()
{
    RefleshEditPanel(_U("TemplatePropertyPanel"), _U("编辑Trigger属性"), _U("PropertyPanel_Return"));
    return _templatePropEditorWnd;
}

void ThemeEditorWindow::SwitchToTemplateEditor(TemplateRootItem* tempNode)
{
    _tempEditorWnd->SetRootTemplateElement(tempNode);
    RefleshEditPanel(_U("TemplatePanel"), _U("编辑模版"), _U("TemplatePanel_Return"));
}

void ThemeEditorWindow::ShowTriggersEditorWindow(TemplateRootItem* tempRoot)
{
    //_tempTrgEditorWnd->Set(setterColl);
    RefleshEditPanel(_U("TemplateTriggersPanel"), _U("编辑模版trigger"), _U("TriggersPanel_Return"));
    _tempTrgEditorWnd->SetTriggerCollectionNode(tempRoot, NULL);
    _tempTrgEditorWnd->Reflesh();
}

void ThemeEditorWindow::OnDbClickTree(Element* sender, MouseButtonEventArg* e)
{
    if (e->GetMouseButton() != suic::MouseButton::mbLeftButton)
    {
        return;
    }

    Element* clickElem = RTTICast<Element>(e->GetOriginalSource());
    if (NULL != clickElem)
    {
        Object* item = clickElem->GetDataContext();
        StyleNode* styleNode = RTTICast<StyleNode>(item);

        // 双击了样式节点
        if (NULL != styleNode)
        {
            RefleshEditPanel(_U("StyleTriggersPanel"), _U("编辑样式"), _U("TriggersPanel_Return"));
            _styleTrgEditorWnd->SetSetterColl(styleNode->GetSetterCollection());
            _styleTrgEditorWnd->SetTriggerCollectionNode(NULL, styleNode->GetTriggerCollection());
            _styleTrgEditorWnd->Reflesh();
        }
        else
        {
            // 点击了模版节点
            TemplateRootItem* tempNode = RTTICast<TemplateRootItem>(item);
            if (NULL != tempNode)
            {
                SwitchToTemplateEditor(tempNode);
            }
        }
    }
    e->SetHandled(true);
}

EditRootPanel* ThemeEditorWindow::GetEditRootPanel()
{
    if (_editRootPanel == NULL)
    {
        _editRootPanel = FindElem<EditRootPanel>(_U("editRootPanel"));
    }
    return _editRootPanel;
}

void ThemeEditorWindow::OnResItemSelectedChanged(suic::Element* sender, suic::SelectionChangedEventArg* e)
{
    ResNode* resNode = NULL;
    e->SetHandled(true);

    if (e->GetAddedItems()->GetCount() == 1)
    {
        resNode = RTTICast<ResNode>(e->GetAddedItems()->GetItem(0));
    }

    if (NULL != _prevEditCtrl)
    {
        _prevEditCtrl->SetVisibility(Visibility::Collapsed);
        _prevEditCtrl = NULL;
    }

    GetEditRootPanel()->SetCurrentResNode(resNode);

    if (NULL != resNode)
    {
        String strName;
        Object* val = resNode->GetValue();
        
        if (RTTICast<FrameworkTemplate>(val) != NULL)
        {
            strName = _U("Template_Res");
        }
        else if (RTTICast<Element>(val) != NULL)
        {
            strName = _U("Layout_Res");
        }
        else if (RTTICast<StyleNode>(resNode) != NULL)
        {
            val = resNode;
            strName = _U("StyleNode_Res");
        }

        if (NULL != val)
        {
            if (strName.Empty())
            {
                strName.Format(_U("%s_Res"), val->GetRTTIType()->typeName);
            }
        }

        suic::TextBox* keyBox = FindElem<suic::TextBox>(_U("xKey"));
        EditorControl* editCtrl = FindElem<EditorControl>(strName);

        if (NULL != keyBox)
        {
            keyBox->SetText(resNode->GetKey());
        }

        if (NULL != editCtrl && NULL != val)
        {
            editCtrl->SetVisibility(Visibility::Visible);
            editCtrl->UpdateResValue(resNode);

            _prevEditCtrl = editCtrl;
        }
    }
}

//=================================== 增加资源操作

void ThemeEditorWindow::CreateStyleNode(ResTypeItem* resItem, TypeItem* ctrlItem, ResNodePtr& resNode)
{
    if (NULL != resItem && resItem->GetType()->InheritFrom(Style::RTTIType()))
    {
        SystemResource::Ins()->FindAndCloneStyle(ctrlItem->GetType()->typeName, resNode);
    }
}

void ThemeEditorWindow::CreateControlTemplateNode(ResTypeItem* resItem, TypeItem* ctrlItem, ResNodePtr& resNode)
{
    if (NULL != resItem && resItem->GetType()->InheritFrom(ControlTemplate::RTTIType()))
    {
        SystemResource::Ins()->FindAndCloneControlTemplate(ctrlItem->GetType()->typeName, resNode);
    }
}

void ThemeEditorWindow::AddResourceItem(ResTypeItem* resItem, TypeItem* ctrlItem, bool bFromType)
{
    if (NULL != _prevEditCtrl)
    {
        _prevEditCtrl->SetVisibility(Visibility::Collapsed);
        _prevEditCtrl = NULL;
    }

    String strName;
    RTTIOfInfo* rttiInfo = resItem->GetType();

    if (rttiInfo->InheritFrom(FrameworkTemplate::RTTIType()))
    {
        strName = _U("Template_Res");
    }
    else if (rttiInfo->InheritFrom(Element::RTTIType()))
    {
        strName = _U("Layout_Res");
    }
    else if (rttiInfo->InheritFrom(Style::RTTIType()))
    {
        strName = _U("StyleNode_Res");
    }

    if (strName.Empty())
    {
        strName.Format(_U("%s_Res"), rttiInfo->typeName);
    }

    suic::TextBox* keyBox = FindElem<suic::TextBox>(_U("xKey"));
    EditorControl* editCtrl = FindElem<EditorControl>(strName);

    if (NULL != editCtrl && NULL != _resourceNode)
    {
        ResNodePtr resNode;
        StyleNode* styleNode = NULL;

        if (bFromType)
        {
            CreateStyleNode(resItem, ctrlItem, resNode);
            if (NULL == resNode.get())
            {
                CreateControlTemplateNode(resItem, ctrlItem, resNode);
            }
        }

        if (NULL == resNode.get())
        {
            editCtrl->CreateDefaultValue(resNode);
        }

        styleNode = RTTICast<StyleNode>(resNode.get());

        SetModified();

        editCtrl->SetVisibility(Visibility::Visible);

        if (styleNode != NULL)
        {
            styleNode->SetTargetType(ctrlItem->GetType());
        }
        else
        {
            TemplateRootItem* tempNode = RTTICast<TemplateRootItem>(resNode.get());
            if (tempNode != NULL)
            {
                if (rttiInfo->InheritFrom(ControlTemplate::RTTIType()))
                {
                    tempNode->SetIsControlTemplate(true);
                }
                else
                {
                    tempNode->SetIsControlTemplate(false);
                }

                tempNode->SetTemplateType(rttiInfo);
            }
        }

        editCtrl->UpdateResValue(resNode);

        // 生成一个x:Key值
        resNode->SetKey(GenerateXKey());

        _resourceNode->AddInternalresItem(resNode.get());

        keyBox->SetText(resNode->GetKey());

        GetResListBox()->SetSelectedItem(resNode.get());
        GetResListBox()->ScrollIntoView(resNode.get());

        _prevEditCtrl = editCtrl;
    }
}

String ThemeEditorWindow::GenerateXKey()
{
    static int keyCount = 0;
    String strKey;


    for (;;)
    {
        ++keyCount;
        strKey.Format(_U("Key%d"), keyCount);

        if (_resourceNode->FindResItem(strKey) == NULL)
        {
            break;
        }
    }

    return strKey;
}

void ThemeEditorWindow::OnOpResButton(Element* sender, RoutedEventArg* e)
{
    e->SetHandled(true);

    String strName = sender->GetName();
    Object* selObj = GetResListBox()->GetSelectedItem();
    ItemCollection* itemsColl = _resourceNode->GetResourceCollection();
    int selIndex = itemsColl->IndexOf(selObj);

    if (strName.Equals(_U("DelResBtn")))
    {
        itemsColl->RemoveItem(selObj);
        if (selIndex >= itemsColl->GetCount())
        {
            selIndex = itemsColl->GetCount() - 1;
        }
        
        if (selIndex >= 0)
        {
            selObj = itemsColl->GetItem(selIndex);
            GetResListBox()->SetSelectedItem(selObj);
        }
    }
    else if (strName.Equals(_U("UpResBtn")))
    {
        if (selIndex > 0)
        {
            itemsColl->SwapItem(selIndex, selIndex - 1);
        }
    }
    else if (strName.Equals(_U("DownResBtn")))
    {
        if (selIndex < itemsColl->GetCount() - 1)
        {
            itemsColl->SwapItem(selIndex, selIndex + 1);
        }
    }

    SetModified();
    GetResListBox()->ScrollIntoView(selObj);
    GetResListBox()->UpdateLayout();
}

void ThemeEditorWindow::OnAddResButton(Element* sender, RoutedEventArg* e)
{
    String strPath("/mpfuid;/resource/uidesign/layout/editor/ResSelectorWindow.xaml");
    ResSelectorWindow* resWnd = new ResSelectorWindow();
    WindowPtr wnd = resWnd;

    e->SetHandled(true);
    Builder::BuildUri(resWnd, strPath);

    if (NULL != resWnd)
    {
        Popup popup;
        popup.setAutoDelete(false);
        popup.SetPlacementTarget(sender);
        popup.SetPlacement(PlacementMode::pmRight);
        popup.SetChild(wnd.get());
        popup.TrackingPopup();

        ResTypeItem* resItem = resWnd->GetSelectedItem();
        TypeItem* ctrlItem = resWnd->GetSelectedControl();

        if (NULL != resItem)
        {
            AddResourceItem(resItem, ctrlItem, resWnd->IsFromTypeChecked());
        }
    }
}
