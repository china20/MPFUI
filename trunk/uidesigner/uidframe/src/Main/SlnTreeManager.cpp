/////////////////////////////////////////////////////
// SlnTreeManager.cpp

#include "stdafx.h"
#include "SlnTreeManager.h"
#include "EncryResource.h"
#include "Solution.h"

SlnTreeManager::SlnTreeManager()
{
    _slnTree = NULL;
    _solution = new Solution(this, _U("solution"));
    _solution->ref();
    _elemTreeMana = new ObjTreeManager();
    _elemTreeMana->ref();
}

SlnTreeManager::~SlnTreeManager()
{
    _elemTreeMana->unref();
    _solution->unref();
}

void SlnTreeManager::InitSolutionTree(FrameworkElement* mainWnd)
{
    _slnTree = mainWnd->FindElem<ProjectTree>(_U("SolutionTree"));

    DataTemplate* dataTemp = suic::RTTICast<DataTemplate>(mainWnd->FindRes(_U("RootItemTemplate")));
    if (NULL != dataTemp)
    {
        _slnTree->SetItemTemplate(dataTemp);
    }

    _slnTree->InitTreeDoc(this, _solution);

    _elemTreeMana->InitElementTree(mainWnd);
}

bool SlnTreeManager::HasModified()
{
    for (int i = 0; i < GetProjectCount(); ++i)
    {
        if (GetProject(i)->HasModified())
        {
            return true;
        }
    }

    return false;
}

void SlnTreeManager::SaveAll()
{
    for (int i = 0; i < GetProjectCount(); ++i)
    {
        GetProject(i)->SaveAll();
    }
}

int SlnTreeManager::GetProjectCount()
{
    return _solution->GetCount();
}

Project* SlnTreeManager::GetProject(int index)
{
    return RTTICast<Project>(_solution->GetItem(index));
}

Project* SlnTreeManager::OpenProject(const suic::String& path)
{
    String strDir = FileDir::SplitToDir(path);

    if (ExistProject(strDir))
    {
        InfoBox::Show(_U("工程目录已经存在，请重新选择！"), _U("提示"));
        return NULL;
    }

    Project* pPrj = Project::OpenPrj(this, path);

    if (NULL != pPrj)
    {
        _solution->AddItem(pPrj);
        pPrj->unref();
    }

    return pPrj;
}

void SlnTreeManager::RefleshProject(Project* pPrj)
{
    if (NULL != pPrj)
    {
        SwitchToRootElement(NULL, pPrj->GetMainWindow());
        UpdateProjectTree(pPrj);
    }
}

void SlnTreeManager::ReloadProject(Project* pPrj)
{
    if (pPrj != NULL)
    {
        pPrj->Reload();
    }
}

bool SlnTreeManager::CloseProject(Project* pPrj, bool bTipIfChanged)
{
    if (bTipIfChanged && pPrj->HasModified())
    {
        int iResult = InfoBox::Show(_U("文件已经修改，是否保存？"), _U("提示"), InfoBox::IB_YesNoCancel);
        if (iResult == InfoBox::Cancel)
        {
            return false;
        }
        else if (iResult == InfoBox::Yes)
        {
            pPrj->SaveAll();
        }
    }

    pPrj->Close();

    return true;
}

bool SlnTreeManager::RemoveProject(Project* pPrj, bool bTipIfChanged)
{
    bool bSucc = false;

    for (int i = 0; i < GetProjectCount(); ++i)
    {
        if (GetProject(i) == pPrj)
        {
            if (CloseProject(pPrj, bTipIfChanged))
            {
                _solution->RemoveItemAt(i);
                bSucc = true;
            }
            break;
        }
    }

    return bSucc;
}

Project* SlnTreeManager::FindProject(const suic::String& prjDir)
{
    suic::String strDir = prjDir;
    strDir.Trim();

    for (int i = 0; i < GetProjectCount(); ++i)
    {
        suic::String tmDir = GetProject(i)->GetProjectDir();
        tmDir.Trim();

        if (tmDir.CompareI(strDir) == 0)
        {
            return GetProject(i);
        }
    }

    return NULL;
}

bool SlnTreeManager::ExistProject(const suic::String& prjDir)
{
    return (FindProject(prjDir) != NULL);
}

Project* SlnTreeManager::CreateProject(const suic::String& name, const suic::String& path)
{
    if (ExistProject(path))
    {
        InfoBox::Show(_U("工程目录已经存在，请重新选择！"), _U("提示"));
        return NULL;
    }

    Project* pPrj = Project::CreatePrj(this, name, path);

    if (NULL != pPrj)
    {
        _solution->AddItem(pPrj);
        pPrj->unref();
    }

    return pPrj;
}

void SlnTreeManager::SwitchToRootElement(DesignElement* resElem, ElementRootItem* rootElem)
{
    _elemTreeMana->SetTargetElement(resElem, rootElem);
    _elemTreeMana->UpdateElementTree();

    _slnTree->SetSelectedItem(rootElem);
    _slnTree->ScrollIntoView(rootElem, false);
}

void SlnTreeManager::UpdateSolutionTree()
{
    if (_slnTree != NULL)
    {
        _slnTree->UpdateLayout();
    }
}

void SlnTreeManager::UpdateProjectTree(Project* pPrj)
{
    if (NULL != _elemTreeMana)
    {
        _elemTreeMana->SetProject(pPrj);
    }

    if (_slnTree != NULL)
    {
        _slnTree->UpdateLayout();
    }
}
