/////////////////////////////////////////////////////
// ProjectTreeMenu.cpp

#include "stdafx.h"
#include <shellapi.h>
#include "ProjectTreeMenu.h"
#include "AddNameWindow.h"
#include "DesignPanel.h"
#include "SlnTreeManager.h"

#include <tools/Utils.h>
#include <System/Tools/EventHelper.h>

#include <Editor/ResApplyWindow.h>
#include <Editor/CreateStyleWindow.h>
#include <Editor/ThemeEditorWindow.h>

#include <Main/MainWindow.h>
#include <Main/ImageSelectorWindow.h>

#pragma comment(lib, "Shell32.lib")

static void OpenAndSelectFile(const String& strPath)
{
    String strSelect;
    String strTemp = strPath;

    strTemp.Replace(_U("/"), _U("\\"));
    strTemp.Replace(_U("\\\\"), _U("\\"));

    strSelect.Format(_U("/select,%s"), strTemp.c_str()) ;

    ShellExecute(NULL, _U("open"), _U("explorer.exe"), strSelect.c_str(), NULL, SW_NORMAL) ;
}

PrjTreeMenu::PrjTreeMenu()
{
    images = NULL;
    rootElem = NULL;
    filter = NULL;
    slnMana = NULL;
    treeView = NULL;
}

PrjTreeMenu::~PrjTreeMenu()
{
    FREEREFOBJ(images);
    FREEREFOBJ(rootElem);
    FREEREFOBJ(filter);
}

SlnTreeManager* PrjTreeMenu::GetSlnTree()
{
    return slnMana;
}

ObjTreeManager* PrjTreeMenu::GetObjTree()
{
    return GetSlnTree()->GetObjTreeManager();
}

void PrjTreeMenu::UpdatePrjTree()
{
    treeView->UpdateLayout();
}

void PrjTreeMenu::SelectTreeItem(Object* pItem)
{
    treeView->SetSelectedItem(pItem);
    treeView->ScrollIntoView(pItem, false);
}

void PrjTreeMenu::Initialize(SlnTreeManager* stm, TreeView* tv)
{
    slnMana = stm;
    treeView = tv;

    if (NULL == images)
    {
        images = new suic::ObservableCollection();
        images->ref();

        images->AddItem(new ImageNodeCmd(this, _U("打开图像")));
        images->AddItem(new ImageNodeCmd(this, _U("删除")));
        images->AddItem(new ImageNodeCmd(this, _U("重命名")));
        images->AddItem(new suic::Separator());
        images->AddItem(new ImageNodeCmd(this, _U("打开目录")));
    }

    if (NULL == rootElem)
    {
        rootElem = new suic::ObservableCollection();
        rootElem->ref();
        rootElem->AddItem(new RootItemCmd(this, _U("保存")));
        rootElem->AddItem(new RootItemCmd(this, _U("关闭")));
        rootElem->AddItem(new RootItemCmd(this, _U("删除")));
        rootElem->AddItem(new RootItemCmd(this, _U("重命名")));
        
        rootElem->AddItem(new RootItemCmd(this, _U("设为启动")));
        rootElem->AddItem(new RootItemCmd(this, _U("应用资源")));

        rootElem->AddItem(new suic::Separator());
        rootElem->AddItem(new RootItemCmd(this, _U("打开目录")));
    }


    if (NULL == filter)
    {
        filter = new suic::ObservableCollection();
        filter->ref();
        filter->AddItem(new FilterNodeCmd(this, _U("移除工程")));
        filter->AddItem(new FilterNodeCmd(this, _U("增加布局")));
        filter->AddItem(new FilterNodeCmd(this, _U("增加主题")));
        filter->AddItem(new FilterNodeCmd(this, _U("增加图像")));
        filter->AddItem(new FilterNodeCmd(this, _U("增加路径")));
        filter->AddItem(new FilterNodeCmd(this, _U("删除")));
        filter->AddItem(new FilterNodeCmd(this, _U("重命名")));
        filter->AddItem(new suic::Separator());
        filter->AddItem(new FilterNodeCmd(this, _U("打开目录"))); 
    }
}

ElemTreeMenu::ElemTreeMenu()
{
    _elemMenu = new suic::ObservableCollection();
    _elemMenu->ref();
    _elemMenu->AddItem(new ElementMenuItemCmd(_U("删除")));
    _elemMenu->AddItem(new ElementMenuItemCmd(_U("锁定")));
    _elemMenu->AddItem(new ElementMenuItemCmd(_U("编辑资源")));

    suic::MenuItem* pItem = new suic::MenuItem();

    pItem->SetHeader(new suic::OString(_U("更改布局类型")));
    _elemMenu->AddItem(pItem);

    pItem->AddChild(new ElementMenuItemCmd(_U("Grid")));
    pItem->AddChild(new ElementMenuItemCmd(_U("Canvas")));    
    pItem->AddChild(new ElementMenuItemCmd(_U("StackPanel")));
    pItem->AddChild(new ElementMenuItemCmd(_U("DockPanel")));    
    pItem->AddChild(new ElementMenuItemCmd(_U("WrapPanel")));
    pItem->AddChild(new ElementMenuItemCmd(_U("HyperPanel")));
    pItem->AddChild(new ElementMenuItemCmd(_U("UniformGrid")));
    pItem->AddChild(new ElementMenuItemCmd(_U("VirtualizingStackPanel")));
}

ElemTreeMenu::~ElemTreeMenu()
{
    _elemMenu->unref();
}

void ElemTreeMenu::TrackElementMenu(ElementRootItem* rootElem, DesignElement* elem, Element* target)
{
    if (rootElem != NULL)
    {
        suic::ContextMenu* elemMenu = new suic::ContextMenu();
        ElementMenuItemCmd* pItem = RTTICast<ElementMenuItemCmd>(_elemMenu->GetItem(2));

        elemMenu->ref();

        if (rootElem->IsTemplate())
        {
            pItem->SetEnabled(false);
        }
        else
        {
            pItem->SetEnabled(true);
        }

        pItem = RTTICast<ElementMenuItemCmd>(_elemMenu->GetItem(1));
        if (elem->GetLockedSelf())
        {
            pItem->SetName(_U("解锁"));
        }
        else
        {
            pItem->SetName(_U("锁定"));
        }

        MenuItem* pMenuItem = RTTICast<MenuItem>(_elemMenu->GetItem(3));
        if (elem->IsCanReplacePanel())
        {
            pMenuItem->Enable(true);
        }
        else
        {
            pMenuItem->Enable(false);
        }

        elemMenu->SetItemsSource(_elemMenu);

        elemMenu->SetTag(elem);
        elemMenu->SetUserTag(0, rootElem);
        elemMenu->SetPlacementTarget(target);
        elemMenu->TrackContextMenu();

        elemMenu->unref();
    }
}

//=================================================
// ElementMenuItemCmd

ElementMenuItemCmd::ElementMenuItemCmd(String name)
{
    _name = name;
}

bool ElementMenuItemCmd::CanExecute(Object* target, Object* parameter)
{
    return true;
}

void ElementMenuItemCmd::OnMainReturnClick(suic::Object* sender, suic::EventArg* e)
{
}

void ElementMenuItemCmd::Execute(Object* target, Object* parameter)
{
    ElementRootItem* rootItem = NULL;
    DesignElement* item = NULL;
    suic::FrameworkElement* elem = suic::RTTICast<suic::FrameworkElement>(target);

    while (elem != NULL)
    {
        elem = elem->GetParent();
        if (NULL != elem && elem->GetRTTIType()->InheritFrom(suic::MenuBase::RTTIType()))
        {
            item = suic::RTTICast<DesignElement>(elem->GetTag());
            elem->SetTag(NULL);
            rootItem = suic::RTTICast<ElementRootItem>(elem->GetUserTag(0));
            elem->SetUserTag(0, NULL);
            break;
        }
    }

    if (NULL != item && NULL != rootItem)
    {
        if (_name.Equals(_U("删除")))
        {
            if (item->GetParent() != NULL)
            {
                DesignElement* pParent = suic::RTTICast<DesignElement>(item->GetParent());
                rootItem->RemoveChileElement(pParent, item);
            }
        }
        else if (_name.Equals(_U("解锁")))
        {
            item->SetLockElement(false);
        }
        else if (_name.Equals(_U("锁定")))
        {
            item->SetLockElement(true);
        }
        else if (_name.Equals(_U("编辑资源")))
        {
            if (!item->IsTemplate())
            {
                /*const String strPath = "/mpfuid;/resource/uidesign/layout/ThemeEditor.xaml";

                if (NULL != rootItem && !rootItem->GetRootElement()->IsLocked())
                {
                    ThemeEditorWindow* themeWnd = new ThemeEditorWindow(rootItem, item->GetResourceDictionary());

                    themeWnd->ref();
                    themeWnd->SetHideMainReturnButton();
                    themeWnd->SetResourceElement(item);
                    themeWnd->ShowDialog(strPath);
                    themeWnd->unref();
                }*/

                MainWindow* mainWnd = dynamic_cast<MainWindow*>(suic::Application::Current()->GetMainWindow());
                if (NULL != mainWnd)
                {
                    ThemeEditorWindow* themeWnd = new ThemeEditorWindow(rootItem, item->GetResourceDictionary());
                    themeWnd->ref();
                    themeWnd->SetResourceElement(item);
                    themeWnd->SetMainReturnEvent(suic::EventHandler(this, &ElementMenuItemCmd::OnMainReturnClick));
                    mainWnd->SwitchToThemeView(themeWnd, String().Format(_U("编辑资源 - %s(%s)"), 
                        item->GetDisplayName().c_str(), rootItem->GetRelativePath().c_str()));
                    themeWnd->unref();
                }
            }
        }
        else if (!_name.Equals(_U("更改布局类型")))
        {
            suic::Panel* newPanel = NULL;

            if (_name.Equals(_U("Grid")))
            {
                newPanel = new suic::Grid();
            }
            else if (_name.Equals(_U("UniformGrid")))
            {
                newPanel = new suic::UniformGrid();
            }
            else if (_name.Equals(_U("Canvas")))
            {
                newPanel = new suic::Canvas();
            }
            else if (_name.Equals(_U("WrapPanel")))
            {
                newPanel = new suic::WrapPanel();
            }
            else if (_name.Equals(_U("DockPanel")))
            {
                newPanel = new suic::DockPanel();
            }
            else if (_name.Equals(_U("StackPanel")))
            {
                newPanel = new suic::StackPanel();
            }
            else if (_name.Equals(_U("HyperPanel")))
            {
                newPanel = new suic::HyperPanel();
            }

            if (NULL != newPanel)
            {
                newPanel->ref();
                if (newPanel->GetRTTIType() != item->GetUIElement()->GetRTTIType())
                {
                    rootItem->ReplacePanel(item, newPanel);
                }
                newPanel->unref();

                if (NULL != rootItem->GetProject() && rootItem->GetProject()->GetSlnTreeManager() != NULL)
                {
                    ObjTreeManager* objMana = rootItem->GetProject()->GetSlnTreeManager()->GetObjTreeManager();

                    if (NULL != objMana && NULL != objMana->GetDesignPanel())
                    {
                        objMana->GetDesignPanel()->UpdateModified();
                    }
                }
            }
        }
    }
}

void ElementMenuItemCmd::SetName(String name)
{
    _name = name;
}

String ElementMenuItemCmd::ToString()
{
    return _name;
}

//=================================================
// RootItemCmd

RootItemCmd::RootItemCmd(PrjTreeMenu* tree, String name)
{
    _tree = tree;
    _name = name;
}

void RootItemCmd::UpdateTreeUI(bool objMana)
{
    _tree->UpdatePrjTree();
}

bool RootItemCmd::CanExecute(Object* target, Object* parameter)
{
    return true;
}

void RootItemCmd::ApplyResourceTo(FileRootItem* resRoot)
{
    ResApplyWindow tempWnd(resRoot);
    const String strPath = "/mpfuid;/resource/uidesign/layout/Editor/ApplyResWindow.xaml";
    tempWnd.setAutoDelete(false);

    if (0 == tempWnd.ShowDialog(strPath))
    {
    }
}

void RootItemCmd::Execute(Object* target, Object* parameter)
{
    RootItem* item = NULL;
    suic::FrameworkElement* elem = suic::RTTICast<suic::FrameworkElement>(target);

    if (elem != NULL)
    {
        elem = elem->GetParent();
        if (NULL != elem)
        {
            item = suic::RTTICast<RootItem>(elem->GetTag());
            elem->SetTag(NULL);
        }
    }

    if (NULL != item)
    {
        if (_name.Equals(_U("保存")))
        {
            item->Save();
            UpdateTreeUI(true);
        }
        else if (_name.Equals(_U("关闭")))
        {
            item->Close();
            UpdateTreeUI(true);
        }
        else if (_name.Equals(_U("删除")))
        {
            if (suic::InfoBox::Show(suic::String().Format(_U("是否确定删除 '%s'?"), item->GetName().c_str()), _U("提示"), suic::InfoBox::ButtonType::IB_YesNo) == suic::InfoBox::Result::Yes)
            {
                item->GetProject()->RemoveRootItem(item);
                UpdateTreeUI(false);
            }
        }
        else if (_name.Equals(_U("设为启动")))
        {
            ElementRootItem* eleRoot = suic::RTTICast<ElementRootItem>(item);
            if (NULL != eleRoot)
            {
                eleRoot->SetStartElement();
            }
        }
        else if (_name.Equals(_U("移除启动")))
        {
            ElementRootItem* eleRoot = suic::RTTICast<ElementRootItem>(item);
            if (NULL != eleRoot)
            {
                eleRoot->RemoveStartElement();
            }
        }
        else if (_name.Equals(_U("应用资源")))
        {
            FileRootItem* resRoot = suic::RTTICast<FileRootItem>(item);
            if (NULL != resRoot)
            {
                ApplyResourceTo(resRoot);
            }
        }
        else if (_name.Equals(_U("打开目录")))
        {
            OpenAndSelectFile(item->GetFullPath());
        }
    }
}

String RootItemCmd::ToString()
{
    return _name;
}

//=================================================
// ImageNodeCmd

ImageNodeCmd::ImageNodeCmd(PrjTreeMenu* tree, String name)
{
    _tree = tree;
    _name = name;
}

bool ImageNodeCmd::CanExecute(Object* target, Object* parameter)
{
    return true;
}

void ImageNodeCmd::Execute(Object* target, Object* parameter)
{
    ImageRootItem* item = NULL;
    suic::FrameworkElement* elem = suic::RTTICast<suic::FrameworkElement>(target);

    if (elem != NULL)
    {
        elem = elem->GetParent();
        if (NULL != elem)
        {
            item = suic::RTTICast<ImageRootItem>(elem->GetTag());
            elem->SetTag(NULL);
        }
    }

    if (NULL != item)
    {
        if (_name.Equals(_U("打开图像")))
        {
            ;
        }
        else if (_name.Equals(_U("删除")))
        {
            FilterNode* pParent = RTTICast<FilterNode>(item->GetParent());

            if (FileDir::RemoveFile(item->GetFullPath()))
            {
                pParent->RemoveFilterItem(item);
                _tree->GetSlnTree()->UpdateSolutionTree();
                _tree->UpdatePrjTree();
            }
        }
        else if (_name.Equals(_U("打开目录")))
        {
            OpenAndSelectFile(item->GetFullPath());
        }
    }
}

String ImageNodeCmd::ToString()
{
    return _name;
}

//=================================================
// FilterNodeCmd

FilterNodeCmd::FilterNodeCmd()
{
}

FilterNodeCmd::FilterNodeCmd(PrjTreeMenu* tree, String name)
{
    _tree = tree;
    _name = name;
}

void FilterNodeCmd::UpdateTreeUI(bool objMana)
{
    _tree->UpdatePrjTree();
}

bool FilterNodeCmd::CanExecute(Object* target, Object* parameter)
{
    return true;
}

void FilterNodeCmd::Execute(Object* target, Object* parameter)
{
    FilterNode* item = NULL;
    suic::FrameworkElement* elem = suic::RTTICast<suic::FrameworkElement>(target);

    if (elem != NULL)
    {
        elem = elem->GetParent();
        if (NULL != elem)
        {
            item = suic::RTTICast<FilterNode>(elem->GetTag());
            elem->SetTag(NULL);
        }
    }

    if (NULL != item)
    {
        if (_name.Equals(_U("增加布局")))
        {
            AddLayoutNode(item);
        }
        else if (_name.Equals(_U("增加路径")))
        {
            AddSubFilter(item);
        }
        else if (_name.Equals(_U("增加主题")))
        {
            AddThemeNode(item);
        }
        else if (_name.Equals(_U("增加图像")))
        {
            AddImageNode(item);
        }
        else if (_name.Equals(_U("移除工程")))
        {
            Project* pPrj = RTTICast<Project>(item);
            if (pPrj != NULL)
            {
                _tree->GetSlnTree()->RemoveProject(pPrj, true);
            }
        }
        else if (_name.Equals(_U("删除")))
        {
            ;
        }
        else if (_name.Equals(_U("打开目录")))
        {
            OpenPathOnNode(item);
        }

        _tree->UpdatePrjTree();
    }
}

String FilterNodeCmd::ToString()
{
    return _name;
}

String FilterNodeCmd::GetAddName(const String& title, const String& name)
{
    AddNameWindow addWnd(title, name, -1);
    addWnd.setAutoDelete(false);

    suic::Builder::BuildUri(&addWnd, "/mpfuid;/resource/uidesign/layout/AddNameWindow.xaml");
    addWnd.ShowDialog();

    return addWnd.GetName();
}

void FilterNodeCmd::OpenPathOnNode(FilterNode* filterNode)
{
    if (NULL != filterNode)
    {
        String strDir = filterNode->GetFullPath();
        ShellExecute(NULL, _U("open"), strDir.c_str(), NULL, NULL, SW_SHOW);
    }
}

bool FilterNodeCmd::IsAlpha(Char c) const
{
    if (c >= 'a' && c <= 'z')
    {
        return true;
    }
    
    if (c >= 'A' && c <= 'Z')
    {
        return true;
    }

    if (c == '_' || c == '.')
    {
        return true;
    }

    return false;
}

bool FilterNodeCmd::IsDigital(Char c) const
{
    if (c >= '0' && c <= '9')
    {
        return true;
    }

    return false;
}

bool FilterNodeCmd::CheckPath(const String& strPath)
{
    Char c = '0';

    if (strPath.Empty())
    {
        return false;
    }

    for (int i = 0; i < strPath.Length(); ++i)
    {
        c = strPath[i];

        if (!IsAlpha(c) && !IsDigital(c))
        {
            return false;
        }
    }

    return true;
}

void FilterNodeCmd::AddLayoutNode(FilterNode* pNode)
{
    String shortPath = pNode->GetRelativePath();
    Project* pPrj = pNode->GetProject();
    ObjTreeManager* objTree = _tree->GetObjTree();
    ElementRootItem* rootElem = NULL;

    shortPath.Trim();
    if (shortPath[shortPath.Length() - 1] != _U('\\') && 
        shortPath[shortPath.Length() - 1] != _U('/'))
    {
        shortPath += _U("/");
    }

    String strFilename = GetAddName(_U("增加布局"), _U("名称："));

    if (!strFilename.Empty())
    {
        strFilename.Trim();

        if (!CheckPath(strFilename))
        {
            suic::InfoBox::Show(_U("名称必须为英文字符和数字组合且首字必须为字符！"), _U("提示"));
            return;
        }

        if (pNode->FindItemOnChild(strFilename) != NULL)
        {
            suic::InfoBox::Show(_U("布局已经存在，请重新输入！"), _U("提示"));
            return;
        }

        rootElem = pPrj->AddRootElement(pNode, strFilename);

        if (NULL != rootElem)
        {
            pPrj->SwitchRootElement(NULL, rootElem);
        }
    }
}

void FilterNodeCmd::AddSubFilter(FilterNode* pNode)
{
    String strPath;
    String shortPath = pNode->GetRelativePath();

    strPath = GetAddName(_U("增加路径"), _U("名称："));

    if (!strPath.Empty())
    {
        strPath.Trim();

        if (!CheckPath(strPath))
        {
            suic::InfoBox::Show(_U("名称必须为英文字符和数字组合且首字必须为字符！"), _U("提示"));
            return;
        }

        if (pNode->FindItemOnChild(strPath) != NULL)
        {
            suic::InfoBox::Show(_U("路径已经存在，请重新输入！"), _U("提示"));
            return;
        }

        pNode->AddSubFilter(strPath);
        UpdateTreeUI(false);
    }
}

void FilterNodeCmd::AddThemeNode(FilterNode* pNode)
{
    String strFilename = GetAddName(_U("增加主题"), _U("名称："));
    if (!strFilename.Empty())
    {
        strFilename.Trim();

        if (!CheckPath(strFilename))
        {
            suic::InfoBox::Show(_U("名称必须为英文字符和数字组合且首字必须为字符！"), _U("提示"));
            return;
        }

        if (pNode->FindItemOnChild(strFilename) != NULL)
        {
            suic::InfoBox::Show(_U("主题已经存在，请重新输入！"), _U("提示"));
            return;
        }

        Project* pPrj = pNode->GetProject();
        ResourceDicRootItem* resElem = pPrj->AddResourceDicRootElement(pNode, strFilename);
        if (NULL != resElem)
        {
        }
    }
}

void FilterNodeCmd::AddImageNode(FilterNode* pNode)
{
    suic::FileBrowser fb;
    if (fb.Open(NULL, _U("All Files(*.*)\0*.*\0\0")))
    {
        String strPath = fb.GetFilePath();
        String shortPath = pNode->GetRelativePath();
        String strFilename;
        String strFileext;

        String extFilter = RES_IMGFILTER;
        suic::FileDir::SplitToPath(strPath, strFilename, strFileext);

        strFileext = _U(".") + strFileext;
        strFileext = strFileext.ToLower();

        if (extFilter.IndexOf(strFileext) != -1)
        {
            strFilename.Trim();
            strFileext.Trim();

            String strShortName = strFilename + strFileext;

            if (!CheckPath(strShortName))
            {
                suic::InfoBox::Show(_U("名称必须为英文字符和数字组合且首字必须为字符！"), _U("提示"));
                return;
            }

            if (pNode->FindItemOnChild(strShortName) != NULL)
            {
                suic::InfoBox::Show(_U("图像已经存在，请重新输入！"), _U("提示"));
                return;
            }

            ImageRootItem* imgNode = new ImageRootItem();
            pNode->AddItem(imgNode);
            imgNode->SetFileName(strFilename + strFileext);
            suic::FileDir::CopyFileTo(strPath, imgNode->GetFullPath(), true);

            _tree->SelectTreeItem(imgNode);
        }
    }
}

//=================================================
//

RootElemMenuCmd::RootElemMenuCmd(PrjTreeMenu* tree, String name)
{
    _tree = tree;
    _name = name;
}

bool RootElemMenuCmd::CanExecute(Object* target, Object* parameter)
{
    return true;
}

void RootElemMenuCmd::Execute(Object* target, Object* parameter)
{
    Project* item = NULL;
    suic::FrameworkElement* elem = suic::RTTICast<suic::FrameworkElement>(target);

    if (elem != NULL)
    {
        elem = elem->GetParent();
        if (NULL != elem)
        {
            item = suic::RTTICast<Project>(elem->GetTag());
            elem->SetTag(NULL);
        }
    }

    if (NULL != item)
    {
        if (_name.Equals(_U("全部保存")))
        {
            item->SaveAll();
            _tree->GetSlnTree()->UpdateSolutionTree();
        }
        else if (_name.Equals(_U("重命名")))
        {
            /*const Mulstr strPath = "/mpfuid;/resource/uidesign/layout/ImagesSelector.xaml";
            ObservableCollection* obserItems = RTTICast<ObservableCollection>(item->GetImagesNode()->GetItemsSource());
            ImageSelectorWindow imgWindow(item->GetProjectDir(), obserItems);

            imgWindow.setAutoDelete(false);
            suic::LoadComponent(NULL, &imgWindow, strPath);
            
            imgWindow.ShowDialog();*/
        }
        else if (_name.Equals(_U("打开目录")))
        {
            String strPath = item->GetProjectDir();
            for (int i = strPath.Length() - 1; i >= 0; --i)
            {
                if (strPath[i] == _U('\\') || strPath[i] == _U('/'))
                {
                    String strDir = strPath.Substring(0, i);
                    ShellExecute(NULL, _U("open"), strDir.c_str(), NULL, NULL, SW_SHOW);
                    break;
                }
            }
        }
    }
}

String RootElemMenuCmd::ToString()
{
    return _name;
}
