//////////////////////////////////////////////////////////////////////////////
// Project.cpp

#include "stdafx.h"
#include "Project.h"
#include "SlnTreeManager.h"

#include <main/MainWindow.h>
#include <main/DesignPanel.h>
#include <System/Tools/CoreHelper.h>

Project* Project::_currentProject = NULL;

static suic::String SYSBACKUPDIR = _U(".backup\\");
static suic::String DEFAULTRESPATH = _U(".system\\default.xaml");

static suic::String TARGET_APP_NAME = _U("Application.xaml");
static suic::String TARGET_THEME_NAME = _U("theme.xaml");
static suic::String TARGET_IMAGES_NAME = _U("images");
static suic::String TARGET_MAINWINDOW_NAME = _U("MainWindow.xaml");

static suic::String TEMPLATE_APP_PATH = _U("resource\\uidesign\\Template\\Application.template");
static suic::String TEMPLATE_IMAGES_PATH = _U("resource\\uidesign\\Template\\images");
static suic::String TEMPLATE_WINDOW_PATH = _U("resource\\uidesign\\Template\\Window.template");
static suic::String TEMPLATE_THEME_PATH = _U("resource\\uidesign\\Template\\theme.template");

ImplementRTTIOfClass(Project, FilterNode)

Project::Project()
    : _docMana(NULL)
    , _appRoot(NULL)
{
    InitEditNodes();
}

Project::Project(SlnTreeManager* docMana)
    : _docMana(docMana)
    , _appRoot(NULL)
{
    InitEditNodes();
}

Project::~Project()
{
    FREEREFOBJ(_appRoot);
}

void Project::InitEditNodes()
{
}

Brush* Project::GetIcon() const
{
    return NULL;
}

SlnTreeManager* Project::GetSlnTreeManager() const
{
    return _docMana;
}

ApplicationRootItem* Project::GetApplicationRootItem() const
{
    return _appRoot;
}

ElementRootItem* Project::GetMainWindow() const
{
    return _appRoot->GetMainRootItem();
}

void Project::UpdateSlnTree(bool bUpdateElementTree)
{
    if (NULL != _docMana)
    {
        _docMana->UpdateSolutionTree();
        if (bUpdateElementTree)
        {
            _docMana->GetObjTreeManager()->UpdateElementTree();
        }
    }
}

void Project::RemoveRootItem(RootItem* rootItem)
{
    if (GetMainWindow() != rootItem && NULL != rootItem)
    {
        XamlNodeItem* pParent = rootItem->GetParent();
        String strPath = rootItem->GetFullPath();

        CheckAndCloseRootElement(rootItem);
        rootItem->Close();
        FileDir::RemoveFile(strPath);
        pParent->RemoveItem(rootItem);
        UpdateSlnTree(true);
    }
}

bool Project::EqualCurrentDesignRootElement(RootItem* rootElem)
{
    DesignPanel* designPanel = _docMana->GetObjTreeManager()->GetDesignPanel();
    if (designPanel->GetRootElement() == rootElem)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool Project::CheckAndCloseRootElement(RootItem* rootElem)
{
    ObjTreeManager* objTree = _docMana->GetObjTreeManager();
    DesignPanel* designPanel = objTree->GetDesignPanel();

    if (designPanel->GetRootElement() == rootElem)
    {
        objTree->SetTargetElement(NULL, NULL);
        return true;
    }
    else
    {
        return false;
    }
}

void Project::SwitchRootElement(DesignElement* resElem, ElementRootItem* rootElem)
{
    if (!rootElem->IsLoaded())
    {
        rootElem->Load(false);
    }

    if (NULL != rootElem->GetUIElement())
    {
        ObjTreeManager* objTree = _docMana->GetObjTreeManager();
        ProjectTree* prjTree = _docMana->GetProjectTree();

        prjTree->SetSelectedItem(rootElem);
        prjTree->ScrollIntoView(rootElem, false);
        objTree->SetTargetElement(resElem, rootElem);

        UpdateSlnTree(false);
    }
}

String Project::GetProjectName() const
{
    return _prjName;
}

void Project::SetProjectName(String val)
{
    _prjName = val;
}

String Project::GetProjectDir() const
{
    return _prjDir;
}

String Project::GetProjectPath() const
{
    return _prjPath;
}

bool Project::ReadApp(const String& strFilename)
{
    if (NULL == _appRoot)
    {
        _appRoot = new ApplicationRootItem();
        _appRoot->ref();
    }

    _appRoot->SetProject(this);

    // 设置文件名
    _appRoot->SetFileName(strFilename);
    
    if (_appRoot->Load(false))
    {
        return true;
    }
    else
    {
        return false;
    }
}

void Project::CopyFilesUnderDir(const String& oriDir, const String& destDir, const String& fileExt, bool bFailIfExist)
{
    FileFinder fileFinder;
    if (!fileFinder.Open(oriDir, _U("*.*")))
    {
        return;
    }

    bool bSearch = true;
    while (bSearch)
    {
        if (fileFinder.FindNext())
        {
            if (fileFinder.IsDot())
            {
                continue;
            }

            String strOriNew;
            String strDestNew;

            strOriNew.Format(_U("%s\\%s"), oriDir.c_str(), fileFinder.GetFileName().c_str());
            strDestNew.Format(_U("%s\\%s"), destDir.c_str(), fileFinder.GetFileName().c_str());

            // 目录
            if (fileFinder.IsDir())
            {
                strDestNew += _U("\\");

                strOriNew.Replace(_U("\\\\"), _U("\\"));
                strDestNew.Replace(_U("\\\\"), _U("\\"));

                FileDir::DupCreateDir(strDestNew);

                // 读取子目录
                CopyFilesUnderDir(strOriNew, strDestNew, fileExt, bFailIfExist);
            }
            else
            {
                String strFilename(fileFinder.GetFileName());
                strFilename.Trim();
                int iPos = strFilename.IndexOf(_U("."));
                if (iPos != -1)
                {
                    String strExt = strFilename.Substring(iPos);
                    strExt.ToLower();

                    if (fileExt.IndexOf(strExt) != -1)
                    {
                        FileDir::CopyFileTo(strOriNew, strDestNew, bFailIfExist);
                    }
                }
            }
        }
        else
        {
            if (GetLastError() == ERROR_NO_MORE_FILES)
            {
                bSearch = false;
            }
            else 
            {
                return;
            }
        }
    }
}

bool Project::ReadFilterFromDir(const String& strDir, const String& strExt, FilterNode* parent)
{
    FileFinder fileFinder;
    if (!fileFinder.Open(strDir, _U("*.*")))
    {
        return false;
    }

    bool bSearch = true;
    while (bSearch)
    {
        if (fileFinder.FindNext())
        {
            if (fileFinder.IsDot())
            {
                continue;
            }

            // 目录
            if (fileFinder.IsDir())
            {
                TCHAR RelativePathNewDirFound[MAX_PATH] = {0};
                _tcscat(RelativePathNewDirFound, fileFinder.GetFilePath().c_str());

                String strFilename(fileFinder.GetFileName());
                strFilename.Trim();

                {
                    if (strFilename.IndexOf(_U(".")) == -1)
                    {
                        FilterNode* newNode = new FilterNode();
                        parent->AddItem(newNode);
                        newNode->SetName(strFilename);

                        // 读取子目录
                        ReadFilterFromDir(RelativePathNewDirFound, strExt, newNode);
                    }
                }
            }
            else
            {
                String strFilename(fileFinder.GetFileName());
                strFilename.Trim();
                int iPos = strFilename.IndexOf(_U("."));
                if (iPos != -1)
                {
                    RootItem* newNode = NULL;
                    String fileExt = strFilename.Substring(iPos);

                    fileExt.ToLower();

                    if (fileExt.Equals(_U(".xaml")) || fileExt.Equals(_U(".xml")))
                    {
                        String strFullPath = parent->GetFullPath() + strFilename;
                        String strNodeName = Utils::CheckUIXmlRoot(strFullPath);

                        if (strNodeName.Equals(_U("Application")))
                        {
                            _appRoot = new ApplicationRootItem();
                            _appRoot->ref();
                            newNode = _appRoot;
                        }
                        else if (strNodeName.Equals(_U("ResourceDictionary")))
                        {
                            newNode = new ResourceDicRootItem();
                        }
                        else
                        {
                            newNode = new ElementRootItem(new DesignElement());
                        }

                        newNode->SetFileName(strFilename);
                    }
                    else if (strExt.IndexOf(fileExt) != -1)
                    {
                        newNode = new ImageRootItem();
                        String strFilepath = parent->GetRelativePath() + strFilename;
                        newNode->SetFileName(strFilename);
                    }

                    if (NULL != newNode)
                    {
                        parent->AddItem(newNode);
                        //newNode->SetProject(this);
                    }
                }
            }
        }
        else
        {
            if (GetLastError() == ERROR_NO_MORE_FILES)
            {
                bSearch = false;
            }
            else 
            {
                return false;
            }
        }
    }

    return true;
}

Project* Project::OpenPrj(SlnTreeManager* docMana, const suic::String& path)
{
    Project* pPrj = new Project(docMana);
    pPrj->ref();

    if (!pPrj->LoadProject(path))
    {
        pPrj->unref();
        pPrj = NULL;
    }
    else
    {
        if (pPrj != _currentProject)
        {
            docMana->GetProjectTree()->SetSelectedItem(pPrj);
        }
    }

    return pPrj;
}

bool Project::InternalLoadPrj()
{
    String strPath;

    ReadFilterFromDir(GetProjectDir(), RES_FILTER, this);

    if (_appRoot == NULL)
    {
        return false;
    }

    strPath = _appRoot->GetFullPath();

    if (ReadApp(strPath))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool Project::Reload()
{
    if (_appRoot != NULL)
    {
        return true;
    }

    XmlDoc xmlDoc;
    XmlNode* root = NULL;

    try
    {
        suic::Mulstr strPath(_prjPath.c_str());
        suic::Mulstr buff;

        Utils::ReadResFromFile(Mulstr(strPath.c_str()).c_str(), "rb", buff);
        buff = buff.FromUtf8(buff.c_str(), buff.Length());

        xmlDoc.parse<0>(buff.c_str());
        root = xmlDoc.first_node();

        if (NULL == root)
        {
            return false;
        }

        XmlNode* childNode = root->first_node();
        while (childNode != NULL)
        {
            String strName(childNode->name());
            if (strName.Equals(_U("Name")))
            {
                XmlAttr* attr = childNode->first_attribute("Value");

                if (NULL != attr)
                {
                    SetProjectName(attr->value());
                }
            }
            else if (strName.Equals(_U("Application")))
            {
                ;
            }
            else if (strName.Equals(_U("MainWindow")))
            {
                ;
            }

            childNode = childNode->next_sibling();
        }
    }
    catch (char* err)
    {
        ;
    }
    catch (...)
    {
    }

    return InternalLoadPrj();
}

void Project::Close()
{
    ObjTreeManager* objTree = _docMana->GetObjTreeManager();
    DesignPanel* designPanel = objTree->GetDesignPanel();
    ElementRootItem* rootItem = designPanel->GetRootElement();

    if (rootItem != NULL && rootItem->GetProject() == this)
    {
        objTree->SetTargetElement(NULL, NULL);
    }

    if (_appRoot != NULL)
    {
        _appRoot->unref();
        _appRoot = NULL;
    }

    Clear();
}

bool Project::LoadProject(const String& path)
{
    _prjPath = path;

    for (int i = path.Length() - 1; i >= 0; --i)
    {
        if (path[i] == _U('\\') || path[i] == _U('/'))
        {
            _prjDir = path.Substring(0, i + 1);
            break;
        }
    }

    CreateBackup();

    return Reload();
}

suic::String Project::GetBackupDir()
{
    suic::String strPath = _prjDir + SYSBACKUPDIR;
    return strPath;
}

suic::String Project::GeDefaultResPath()
{
    suic::String strPath = _prjDir + DEFAULTRESPATH;
    return strPath;
}

void Project::CreateBackup()
{
    suic::String strPath = GetBackupDir();
    FileDir::DupCreateDir(strPath);
}

void Project::BackupRootItem(RootItem* rootItem)
{
    // 保存到备份目录
    String strBackPath = GetBackupDir() + rootItem->GetRelativePath();

    FileDir::DupCreateDir(strBackPath);
    FileDir::CopyFileTo(rootItem->GetFullPath(), strBackPath, true);
}

Project* Project::CreatePrj(SlnTreeManager* docMana, const suic::String& name, const suic::String& path)
{
    Project* pPrj = new Project(docMana);

    pPrj->SetProjectName(name);
    pPrj->_prjDir = path;
    pPrj->ref();

    pPrj->_prjDir.Trim();
    if (pPrj->_prjDir[pPrj->_prjDir.Length() - 1] != _U('\\'))
    {
        pPrj->_prjDir += _U("\\");
    }

    FileDir::DupCreateDir(pPrj->_prjDir);

    // 创建备份目录
    pPrj->CreateBackup();

    if (!pPrj->InitDefaultProject())
    {
        pPrj->unref();
        pPrj = NULL;
    }
    else
    {
        if (pPrj != _currentProject)
        {
            docMana->GetProjectTree()->SetSelectedItem(pPrj);
        }
    }

    return pPrj;
}

bool Project::CopyTemplateFile(const String& tempPath, const String& shortPath)
{
    Mulstr buff;
    Mulstr strPath;
    String strFlag;
    String destPath;

    destPath.Format(_U("%s%s"), _prjDir.c_str(), shortPath.c_str());

    strPath = tempPath.c_str();
    strFlag.Format(_U("%s"), GetProjectName().c_str());

    Utils::ReadResFromFile(strPath.c_str(), "rb", buff);
    buff = buff.Replace("@template_flag@", strFlag.c_str());

    if (buff.Length() < 16)
    {
        return false;
    }

    FileDir::DupCreateDir(destPath);
    strPath = destPath.c_str();

    FILE* f = fopen(strPath.c_str(), "wb");
    if (NULL != f)
    {
        fwrite(buff.c_str(), 1, buff.Length(), f);
        fclose(f);

        // 保存到备份目录
        String strBackPath = GetBackupDir() + shortPath;

        FileDir::DupCreateDir(strBackPath);
        FileDir::CopyFileTo(destPath, strBackPath, true);

        return true;
    }
    else
    {
        return false;
    }
}

//void CreateDefaultTheme(const suic::String& strSucDir)
//{
//    suic::String tempFile = FileDir::CalculatePath(TEMPLATE_THEME_PATH);
//    if (!CopyTemplateFile(tempFile, strSucDir))
//    {
//        return false;
//    }
//}

bool Project::InitDefaultProject()
{
    String tempFile;
    String destFile;
    String strPath;

    //String strFilename = _U("Application.xaml");

    _prjPath.Format(_U("%s%s.uiproj"), _prjDir.c_str(), _prjName.c_str());

    // 拷贝Application.xml文件
    tempFile = FileDir::CalculatePath(TEMPLATE_APP_PATH);
    destFile = TARGET_APP_NAME;//_U("Application.xaml");
    if (!CopyTemplateFile(tempFile, destFile))
    {
        return false;
    }

    // 拷贝Window.xml文件
    tempFile = FileDir::CalculatePath(TEMPLATE_WINDOW_PATH);
    destFile = TARGET_MAINWINDOW_NAME;// _U("MainWindow.xaml");
    if (!CopyTemplateFile(tempFile, destFile))
    {
        return false;
    }

    // 拷贝default.xml文件
    /*tempFile = FileDir::CalculatePath(_U("resource\\uidesign\\Template\\theme.xaml"));
    destFile = DEFAULTRESPATH;
    if (!CopyTemplateFile(tempFile, destFile))
    {
        return false;
    }*/

    // 拷贝images目录
    tempFile = FileDir::CalculatePath(TEMPLATE_IMAGES_PATH);
    destFile.Format(_U("%s%s\\"), _prjDir.c_str(), TARGET_IMAGES_NAME.c_str());
    FileDir::DupCreateDir(destFile);
    CopyFilesUnderDir(tempFile, destFile, RES_FILTER, true);

    if (InternalLoadPrj())
    {
        SaveProject();
        return true;
    }
    else
    {
        return false;
    }
}

String Project::ToString()
{
    return _prjName;
}

void Project::SaveProject()
{
    int i = 0;
    suic::String strXml = _U("<?xml version=\"1.0\" encoding=\"utf-8\"?>\n");

    strXml += _U("<UIProject>\n");

    strXml += ResNode::OFFSET1 + _U("<Name Value=\"");
    strXml += _prjName;
    strXml += _U("\"/>\n");
    strXml += ResNode::OFFSET1 + _U("<Author Value=\"Created by MPF\"/>\n");
    strXml += ResNode::OFFSET1 + _U("<Version Value=\"0.1.0.0\"/>\n");

    strXml += _U("</UIProject>\n");

    FileWriter fs;
    String strFile = GetProjectPath();

    if (fs.Open(strFile))
    {
        fs.Write(strXml);
        fs.Close();
    }
}

bool Project::Zip(const String& strZip, const String& pwd)
{
    String strPath = GetProjectDir();
    String zipExt = RES_FILTER;

    return Utils::ZipFolder(strZip, strPath, zipExt, pwd);
}

void Project::SaveAll()
{
    SaveProject();
    SaveFilterNode(this);
}

void Project::Reflesh(bool onlyReadAppRes)
{
    if (NULL != _appRoot)
    {
        SlnTreeManager* slnMana = GetSlnTreeManager();
        ObjTreeManager* objTree = slnMana->GetObjTreeManager();
        DesignPanel* pDesignPanel = objTree->GetDesignPanel();
        ElementRootItem* rootItem = pDesignPanel->GetRootElement();

        if (onlyReadAppRes)
        {
            _appRoot->ClearOnlyRes();
        }
        else
        {
            _appRoot->Close();
        }
        _appRoot->Load(onlyReadAppRes);
        //objTree->SetTargetElement(NULL);
        RefleshFilterNode(this, rootItem);

        //DesignElement* focused = rootItem->GetFocusElement();

        //pDesignPanel->SwitchRootElement(rootItem);
        //SwitchRootElement(rootItem);
        //pDesignPanel->SetFocusedElement(focused, true);
    }
}

void Project::RefleshFilterNode(FilterNode* pNode, RootItem* currShow)
{
    for (int i = 0; i < pNode->GetCount(); ++i)
    {
        ElementRootItem* rootElem = RTTICast<ElementRootItem>(pNode->GetItem(i));

        if (NULL != rootElem)
        {
            if (rootElem->IsLoaded())
            {
                /*if (rootElem->IsModified())
                {
                    rootElem->Save();
                }
                rootElem->Reset();
                rootElem->Load(false);*/
                if (rootElem == currShow)
                {
                    rootElem->SetRefreshFlag(true);
                    rootElem->Refresh();
                }
                else
                {
                    rootElem->SetRefreshFlag(true);
                }
            }

            continue;
        }

        FilterNode* subNode = RTTICast<FilterNode>(pNode->GetItem(i));
        if (NULL != subNode)
        {
            RefleshFilterNode(subNode, currShow);
        }
    }
}

void Project::SaveFilterNode(FilterNode* pNode)
{
    for (int i = 0; i < pNode->GetCount(); ++i)
    {
        RootItem* rootElem = RTTICast<RootItem>(pNode->GetItem(i));

        if (NULL != rootElem)
        {
            if (rootElem->IsModified())
            {
                rootElem->Save();
            }

            continue;
        }

        FilterNode* subNode = RTTICast<FilterNode>(pNode->GetItem(i));
        if (NULL != subNode)
        {
            SaveFilterNode(subNode);
        }
    }
}

bool Project::HasModified()
{
    if (HasModified(this))
    {
        return true;
    }

    return false;
}

bool Project::HasModified(FilterNode* pNode)
{
    for (int i = 0; i < pNode->GetCount(); ++i)
    {
        RootItem* rootElem = RTTICast<RootItem>(pNode->GetItem(i));

        if (NULL != rootElem && rootElem->IsModified())
        {
            return true;
        }

        FilterNode* subNode = RTTICast<FilterNode>(pNode->GetItem(i));
        if (NULL != subNode)
        {
            if (HasModified(subNode))
            {
                return true;
            }
        }
    }

    return false;
}

String Project::GetRootItemsAsString(const String& offset, FilterNode* pNode)
{
    String strXml;

    for (int i = 0; i < pNode->GetCount(); ++i)
    {
        RootItem* rootElem = RTTICast<RootItem>(pNode->GetItem(i));
        if (NULL != rootElem)
        {
            if (rootElem != _appRoot && rootElem != _appRoot->GetMainRootItem())
            {
                strXml += offset + _U("<File Name=\"");
                strXml += rootElem->GetRelativePath();
                strXml += _U("\" />\n");
            }
        }
        else
        {
            FilterNode* subNode = RTTICast<FilterNode>(pNode->GetItem(i));
            if (NULL != subNode)
            {
                String subXml = GetRootItemsAsString(offset + ResNode::OFFSET1, subNode);
                if (subXml.Empty())
                {
                    strXml += offset + _U("<Path Name=\"");
                    strXml += subNode->GetName();
                    strXml += _U("\" />\n");
                }
                else
                {
                    strXml += offset + _U("<Path Name=\"");
                    strXml += subNode->GetName();
                    strXml += _U("\" >\n");
                    strXml += subXml;
                    strXml += offset + _U("</Path>\n");
                }
            }
        }
    }

    return strXml;
}

String Project::ComputeWindowPath(const String& shortPath)
{
    // 计算新建窗口的路径
    String strWndPath;
    int iWndIndex = 1;

    for (;;)
    {
        strWndPath.Format(_U("%sWindow%d.xaml"), shortPath.c_str(), iWndIndex);

        String strPath = GetProjectDir() + strWndPath;

        if (FileDir::GetFileProp(strPath.c_str()) == FileDir::fpNone)
        {
            break;
        }
        ++iWndIndex;
    }

    return strWndPath;
}

Project* Project::GetCurrentProject()
{
    return _currentProject;
}

void Project::SetMainWindowTitle()
{
    if (suic::Application::Current() != NULL)
    {
        suic::String strTitle;
        MainWindow* mainWnd = dynamic_cast<MainWindow*>(Application::Current()->GetMainWindow());
        strTitle.Format(_U("MPF界面设计(%s)"), GetProjectPath().c_str());
        mainWnd->SetMainTitle(strTitle);
    }
}

void Project::SetMainWindowTitle(suic::String strTitle)
{
    if (suic::Application::Current() != NULL)
    {
        Application::Current()->GetMainWindow()->SetTitle(strTitle);
    }
}

void Project::SwitchToCurrent()
{
    ApplicationRootItem* appItem = NULL;
    if (_currentProject != this)
    {
        _currentProject = this;
        appItem = _currentProject->GetApplicationRootItem();
        if (NULL != appItem)
        {
            DesignHelper::SetDesignApplication(appItem->GetApplicationNode()->GetApplication());
        }

        SetMainWindowTitle();
    }
}

bool Project::OpenRootElement(ElementRootItem* mainElem)
{
    String strWndPath = GetProjectDir() + mainElem->GetFileName();
    XamlLoader xamlLoader;

    if (NULL != mainElem->GetRootElement())
    {
        return true;
    }

    //mainElem->SetProject(this);
    xamlLoader.LoadElementRootXaml(mainElem, strWndPath);

    if (mainElem->GetRootElement() != NULL)
    {
        return true;
    }
    else
    {
        return false;
    }
}

/*ResNode* Project::FindResItem(const String& strName)
{
    ResNode* resNode = NULL;
    if (NULL != _appRoot)
    {
        resNode = _appRoot->GetApplicationNode()->GetResourceDictionary()->SearchResItem(strName);
    }
    return resNode;
}*/

RootItem* Project::FindRootItem(const String& strPath)
{
    String strFilename;
    RootItem* rootItem = NULL;
    FilterNode* filterNode = CreateFilterByPath(strPath, strFilename);

    if (NULL != filterNode)
    {
        FilterNode* newNode = filterNode->FindItemOnChild(strFilename);
        if (NULL != newNode)
        {
            rootItem = RTTICast<RootItem>(newNode);
        }
        return rootItem;
    }
    else
    {
        return NULL;
    }
}

/*XamlRootItem* Project::FindAndCreateRootItem(const String& strPath)
{
    String strFilename;
    XamlRootItem* rootItem = NULL;
    FilterNode* filterNode = CreateFilterByPath(strPath, strFilename);

    if (NULL != filterNode)
    {
        FilterNode* newNode = filterNode->FindItemOnChild(strFilename);
        if (NULL == newNode)
        {
            rootItem = new XamlRootItem();
            rootItem->SetFileName(strFilename);
            filterNode->AddItem(rootItem);
        }
        else
        {
            rootItem = RTTICast<XamlRootItem>(newNode);
        }
        return rootItem;
    }
    else
    {
        return NULL;
    }
}*/

void Project::FindAllResourceDicRootItems(suic::Array<ResourceDicRootItem*>& resRootItems)
{
    FindAllResourceDicRootItems(this, resRootItems);
}

void Project::FindAllResourceDicRootItems(FilterNode* pParent, suic::Array<ResourceDicRootItem*>& resRootItems)
{
    for (int i = 0; i < pParent->GetCount(); ++i)
    {
        FilterNode* filterNode = RTTICast<FilterNode>(pParent->GetItem(i));
        if (NULL != filterNode)
        {
            ResourceDicRootItem* resRootItem = suic::RTTICast<ResourceDicRootItem>(filterNode);
            if (NULL != resRootItem)
            {
                resRootItems.Add(resRootItem);
            }
            else
            {
                FindAllResourceDicRootItems(filterNode, resRootItems);
            }
        }
    }
}

ElementRootItem* Project::AddRootElement(FilterNode* pParent, const String& fileName)
{
    String strName = fileName;

    if (strName.IndexOf(_U(".")) == -1)
    {
        strName += _U(".xaml");
    }

    String strPath = pParent->GetFullPath() + strName;
    String strOriPath = FileDir::CalculatePath(TEMPLATE_WINDOW_PATH);

    FileDir::CopyFileTo(strOriPath, strPath, true);

    ElementRootItem* rootItem = new ElementRootItem(new DesignElement());

    rootItem->SetFileName(strName);
    pParent->AddItem(rootItem);

    // 保存到备份目录
    BackupRootItem(rootItem);

    return rootItem;
}

ResourceDicRootItem* Project::AddResourceDicRootElement(FilterNode* pParent, const String& fileName)
{
    String strName = fileName;

    if (strName.IndexOf(_U(".")) == -1)
    {
        strName += _U(".xaml");
    }

    String strPath = pParent->GetFullPath() + strName;
    String strOriPath = FileDir::CalculatePath(TEMPLATE_THEME_PATH);

    FileDir::CopyFileTo(strOriPath, strPath, true);

    ResourceDicRootItem* rootItem = new ResourceDicRootItem();

    rootItem->SetFileName(strName);
    pParent->AddItem(rootItem);

    // 保存到备份目录
    BackupRootItem(rootItem);

    return rootItem;
}

FilterNode* Project::CreateFilterByDir(const String& strDir)
{
    FilterNode* addNode = NULL;
    String strPath = strDir;
    strPath.Replace(_U("\\"), _U("/"));
    strPath.Replace(_U("//"), _U("/"));
    StringArray vec;

    vec.SplitString(strPath, _U("/"));

    // 为1表示增加到根节点下
    if (vec.GetCount() == 0)
    {
        addNode = this;
    }
    else
    {
        addNode = this;

        for (int i = 0; i < vec.GetCount(); ++i)
        {
            String strSubDir = vec[i];
            FilterNode* newNode = NULL;
            strSubDir.Trim();

            if (!strSubDir.Empty())
            {
                newNode = addNode->FindItemOnChild(strSubDir);
                if (NULL == newNode)
                {
                    newNode = new FilterNode();
                    newNode->SetName(strSubDir);
                    addNode->AddItem(newNode);
                }
                addNode = newNode;
            }
        }
    }

    return addNode;
}

FilterNode* Project::CreateFilterByPath(const String& strUri, String& strName)
{
    FilterNode* addNode = NULL;
    String strPath = ResourceUri(strUri).GetResourcePath();
    strPath.Replace(_U("\\"), _U("/"));
    StringArray vec;

    vec.SplitString(strPath, _U("/"));

    // 为1表示增加到根节点下
    if (vec.GetCount() == 1)
    {
        addNode = this;
        strName = vec[0];
    }
    else
    {
        addNode = this;
        strName = vec[vec.GetCount() - 1];

        for (int i = 0; i < vec.GetCount() - 1; ++i)
        {
            String strSubDir = vec[i];
            FilterNode* newNode = NULL;
            strSubDir.Trim();

            if (!strSubDir.Empty())
            {
                newNode = addNode->FindItemOnChild(strSubDir);
                if (NULL == newNode)
                {
                    newNode = new FilterNode();
                    newNode->SetName(strSubDir);
                    addNode->AddItem(newNode);
                }
                addNode = newNode;
            }
        }
    }

    return addNode;
}

void Project::RemoveRootElement(ElementRootItem* pItem)
{
    RemoveRootItem(pItem);
}

