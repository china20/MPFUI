/////////////////////////////////////////////////////
// DriverItem.cpp

#include "stdafx.h"
#include "DriverItem.h"
#include "MainWindow.h"

static const suic::String PIC_FILTER = ".jpg;.jpeg;.bmp;.tif;.png";

DriverItem::DriverItem(suic::String name, suic::String path, bool subInited)
{
    _name = name;
    _path = path;
    _subInited = subInited;
}

DriverItem::DriverItem(suic::String name, suic::String path)
{
    _name = name;
    _path = path;
    _subInited = false;
}

void DriverItem::OnSetExpanded(bool val)
{
    suic::HierarchicalItem::OnSetExpanded(val);
    if (val)
    {
        InitSubFolders();
    }
}

void DriverItem::InitSubFolders()
{
    if (!_subInited)
    {
        _subInited = true;

        if (!_path.Empty())
        {
            FileFinder ff;

            if (ff.Open(_path, _U("*.*")))
            {
                while (ff.FindNext())
                {
                    if (!ff.IsDot())
                    {
                        if (ff.IsDir())
                        {
                            GetChildren()->AddItem(new DriverItem(ff.GetFileName(), ff.GetFilePath()));
                        }
                        else
                        {
                            suic::String strExt;
                            suic::String strName = ff.GetFileName();
                            int iPos = strName.IndexOf(".");

                            if (iPos > 0)
                            {
                                strExt = strName.Substring(iPos);
                                strExt.ToLower();
                            }
                        }
                    }
                }

                ff.Close();
            }
        }
    }
}

//=========================================================
// DriverGroup

void DriverGroup::InitRootItems()
{
    LPSHELLFOLDER lsfDesk = NULL;

    if (SUCCEEDED(::SHGetDesktopFolder(&lsfDesk)))
    {
        InitMyFolder(lsfDesk, CSIDL_DESKTOP);
        InitMyFolder(lsfDesk, CSIDL_MYPICTURES);
        InitMyFolder(lsfDesk, CSIDL_MYDOCUMENTS);
        InitMyComputer(lsfDesk);
    }

    if (NULL != lsfDesk)
    {
        lsfDesk->Release();
    }
}

void DriverGroup::InitMyFolder(LPSHELLFOLDER lsfDesk, int clsid)
{
    LPITEMIDLIST itemIdl = NULL;

    if (SUCCEEDED(SHGetSpecialFolderLocation(NULL, clsid, &itemIdl)))
    {
        suic::String strName = GetName(itemIdl);
        suic::String strPath = GetPath(lsfDesk, itemIdl);

        DriverItem* pItem = new DriverItem(strName, strPath);
        AddItem(pItem);
    }
}

void DriverGroup::InitMyComputer(LPSHELLFOLDER lsfDesk)
{
    LPSHELLFOLDER lsfChild = NULL;
    LPITEMIDLIST itemIdl = NULL;
    LPENUMIDLIST lpe = NULL;

    UINT uFlags = SHCONTF_FOLDERS | SHCONTF_NONFOLDERS;

    if (NULL != lsfDesk)
    {
        SHGetSpecialFolderLocation(NULL, CSIDL_DRIVES, &itemIdl);

        if (SUCCEEDED(lsfDesk->BindToObject(itemIdl, 0, IID_IShellFolder, (LPVOID *)&lsfChild)))
        {
            if (SUCCEEDED(lsfChild->EnumObjects(NULL, uFlags, &lpe)) && NULL != lpe)
            {
                suic::String strName = GetName(itemIdl);
                suic::String strPath = GetPath(lsfDesk, itemIdl);
                DriverItem* pItem = new DriverItem(strName, strPath, true);

                AddItem(pItem);

                InitChildDrivers(pItem, lsfChild, itemIdl, lpe);
                lpe->Release();
            }
        }
    }

    if (NULL != lsfChild)
    {
        lsfChild->Release();
    }
}

void DriverGroup::InitChildDrivers(DriverItem* pParent, LPSHELLFOLDER lpsf, LPITEMIDLIST itemIdl, LPENUMIDLIST lpe)
{
    ULONG ulFetched = 0;
    ShellMalloc pMalloc;
    LPITEMIDLIST lpi = NULL;
    UINT ulSFGAOFlags = SFGAO_HASSUBFOLDER | SFGAO_FOLDER | SFGAO_DISPLAYATTRMASK | SFGAO_REMOVABLE | SFGAO_COMPRESSED | SFGAO_ENCRYPTED;

    while (lpe->Next(1, &lpi, &ulFetched) == S_OK)
    {
        LPITEMIDLIST lptItem = NULL;
        ULONG ulAttrs = ShellHelper::GetAttributes(lpsf, lpi, ulSFGAOFlags);

        if ((ulAttrs & SFGAO_FOLDER) != 0)
        {
            lptItem = ShellHelper::ConcatPidls(pMalloc, itemIdl, lpi);
            suic::String strName = GetName(lptItem);
            suic::String strPath = GetPath(lpsf, lptItem);

            DriverItem* pChild = new DriverItem(strName, strPath);
            pParent->GetChildren()->AddItem(pChild);

            ShellHelper::FreePidl(lptItem);
        }
    }
}

suic::String DriverGroup::GetName(LPITEMIDLIST itemIdl)
{
    suic::String strName;
    SHFILEINFO fileInfo;

    ::ZeroMemory(&fileInfo, sizeof(fileInfo));
    ::SHGetFileInfo((LPCTSTR)itemIdl, NULL, &fileInfo, sizeof(fileInfo), SHGFI_PIDL | SHGFI_DISPLAYNAME);
    strName = fileInfo.szDisplayName;

    return strName;
}

suic::String DriverGroup::GetPath(LPSHELLFOLDER lpsf, LPITEMIDLIST itemIdl)
{
    suic::String strPath;
    ULONG ulAttrs = SFGAO_FILESYSTEM;

    if (lpsf != NULL)
    {
        lpsf->GetAttributesOf(1, (const struct _ITEMIDLIST **)&itemIdl, &ulAttrs);
        if (ulAttrs & SFGAO_FILESYSTEM)
        {
            TCHAR szFolderPath[_MAX_PATH];
            if (::SHGetPathFromIDList(itemIdl, szFolderPath))
            {
                strPath = szFolderPath;
            }
        }
    }

    return strPath;
}