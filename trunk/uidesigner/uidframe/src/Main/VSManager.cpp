/////////////////////////////////////////////////////
// VSManager.cpp

#include "stdafx.h"
#include "VSManager.h"

#include <tools/Utils.h>
#include <zip/XRUnzip.h>

static suic::Mulstr VSTEMPLATENAME = "MPFTemplate";

void VSManager::UnzipTo(const suic::String& strZipPath, const suic::String& strDir, const suic::String& name)
{
	XRUnzip xrZip;
    //suic::Byte* prjData = new suic::Byte[data.Length()];
    //memcpy(prjData, data.c_str(), data.Length());
    //suic::Mulstr data;
    //Utils::ReadResFromFile(suic::Mulstr(strZipPath.c_str()).c_str(), "rb", data);

    if (xrZip.OpenFromFile(strZipPath.c_str()))
	{
        int index = 0;

		for (;;)
		{
			suic::String shortFile;
			suic::ByteStream unzipData;
			int iSize = xrZip.GetZipItemData(index, unzipData, shortFile);
			if (iSize < 0)
			{
				break;
			}

            FileWriter fs;
			suic::String strPath;
			
			if (!name.Empty())
			{
                shortFile.Replace(VSTEMPLATENAME.c_str(), name);
                strPath.Format(_U("%s\\%s\\%s"), strDir.c_str(), name.c_str(), shortFile.c_str());
			}
            else
            {
                strPath.Format(_U("%s\\%s"), strDir.c_str(), shortFile.c_str());
            }
            
            if (0 == iSize)
            {
                strPath += _U("\\");
            }

            FileDir::DupCreateDir(strPath);
	
			if (iSize > 0 && fs.Open(strPath))
			{
                fs.WriteByte(unzipData.GetBuffer(), unzipData.GetSize());
				fs.Close();
			}

            index++;
		}
	}
}

bool VSManager::CreateVSProject(const suic::String& strVer, const suic::String& name, const suic::String& strDir)
{
	// 获取对应的VS工程数据
    suic::String strPath;
    suic::String strDll;
    suic::String strCommon;
    FileReader fReader;

    strCommon = FileDir::CalculatePath(String().Format(_U("resource\\uidesign\\VSTemplate\\common\\common.zip")));
    strPath = FileDir::CalculatePath(String().Format(_U("resource\\uidesign\\VSTemplate\\%s\\trunk.zip"), strVer.c_str()));

    if (!FileDir::FileExist(strPath) || !FileDir::FileExist(strPath))
    {
        return false;
    }

	// 解压到指定目录
	UnzipTo(strPath, strDir, name);
	
	// 用name替换模板的工程项目名称
	ReplacePrjName(name, strDir + name);

	// 解压到指定目录
	UnzipTo(strCommon, strDir, name);

    strPath.Format(_U("%s\\%s\\bin\\suicoreu.dll"), strDir.c_str(), name.c_str());
    FileDir::CopyFileTo(FileDir::CalculatePath(_U("suicoreu.dll")), strPath, true);

    strPath.Format(_U("%s\\%s\\bin\\suicoreud.dll"), strDir.c_str(), name.c_str());
    FileDir::CopyFileTo(FileDir::CalculatePath(_U("suicoreud.dll")), strPath, true);

    strPath.Format(_U("%s\\%s\\bin\\suiwgxu.dll"), strDir.c_str(), name.c_str());
    FileDir::CopyFileTo(FileDir::CalculatePath(_U("suiwgxu.dll")), strPath, true);

    strPath.Format(_U("%s\\%s\\bin\\suiwgxud.dll"), strDir.c_str(), name.c_str());
    FileDir::CopyFileTo(FileDir::CalculatePath(_U("suiwgxud.dll")), strPath, true);

    return true;
}

void VSManager::ReplacePrjName(const suic::String& name, const suic::String& strDir)
{
	FileFinder fileFinder;
    if (!fileFinder.Open(strDir, _U("*.*")))
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

            suic::String strPath = fileFinder.GetFilePath();

            // 目录
            if (fileFinder.IsDir())
            {
                ReplacePrjName(name, strPath);
			}
            else
            {
                FileWriter fWriter;
                suic::Mulstr data;

                Utils::ReadResFromFile(suic::Mulstr(strPath.c_str()).c_str(), "rb", data);
                FileDir::RemoveFile(strPath);

                data.Replace(VSTEMPLATENAME.c_str(), name.c_str());

                fWriter.Open(strPath);
                fWriter.WriteByte((suic::Byte*)data.c_str(), data.Length());
            }
		}
        else
        {
            break;
        }
	}
}
