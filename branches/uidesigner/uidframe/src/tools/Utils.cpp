
#include "stdafx.h"

#include <time.h>
#include <locale>

#include <main/Encrypt.h>
#include <tools/Utils.h>
#include <Core/ResNode.h>
#include <Core/FileFinder.h>

time_t Utils::_startRunTime = time(NULL);

bool Utils::CheckUseTimeout(const String& info)
{
    return false;
    const time_t cst_timeout = 20 * 60;
    time_t curTime = time(NULL);

    if (curTime - _startRunTime > cst_timeout)
    {
        if (!info.Empty())
        {
            InfoBox::Show(info, _U("提示"));
        }
        return true;
    }
    else
    {
        return false;
    }
}

static FILE* __TiFOpen(const char* filename, const char* mode)
{
#if defined(_MSC_VER) && (_MSC_VER >= 1400)
    FILE* fp = 0;
    errno_t err = fopen_s(&fp, filename, mode);

    if (!err && fp)
    {
        return fp;
    }
    else
    {
        return 0;
    }
#else
    return fopen(filename, mode);
#endif
}

int Utils::ReadResFromFile(const char* filename, const char* mode, Mulstr& buff)
{
    FILE* f = __TiFOpen(filename, mode);

    if (f)
    {
        long length = 0;
        fseek(f, 0, SEEK_END);
        length = ftell(f);
        fseek(f, 0, SEEK_SET);

        buff.Resize(length);

        if (fread(buff.c_str(), 1, length, f) != 1) 
        {
        }
        else
        {
            ;
        }

        fclose(f);
        return length;
    }
    else
    {
        return 0;
    }
}

String Utils::CheckUIXmlRoot(const String& strPath)
{
    String strName;
    XmlDoc xmlDoc;
    XmlNode* root = NULL;
    suic::Mulstr filename(strPath.c_str());
    suic::Mulstr buff;

    ReadResFromFile(filename.c_str(), "rb", buff);

    try
    {
        xmlDoc.parse<0>(buff.c_str());
        root = xmlDoc.first_node();

        if (NULL != root)
        {
            strName = root->name();
        }
    }
    catch (...)
    {
    }

    return strName;
}

bool Utils::ZipFolder(const String& strZip, const String& strPath, const String& zipExt, const String& pwd)
{
    XRZip xrZip;
    if (xrZip.OpenFromFile((TCHAR*)(strZip.c_str())))
    {
        ZipFileFolder(&xrZip, strPath, strPath, zipExt);
        xrZip.Close();

        // 加密
        if (!pwd.Empty())
        {
            UIEncrypt::Encrypt(pwd, strZip);
        }

        return true;
    }
    else
    {
        return false;
    }
}

void Utils::ZipFileFolder(XRZip* pZip, const String& strRoot, const String& strPath, const String& zipExt)
{
    FileFinder fileFinder;

    if (!fileFinder.Open(strPath, _U("*.*")))
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

            // 目录
            if (fileFinder.IsDir())
            {
                String strSubPath;
                String strFilename(fileFinder.GetFileName());
                strFilename.Trim();

                if (strFilename.IndexOf(_U(".")) == -1)
                {
                    strSubPath.Format(_U("%s%s\\"), strPath.c_str(), strFilename.c_str());
                    ZipFileFolder(pZip, strRoot, strSubPath, zipExt);
                }
            }
            else
            {
                String strFilePath;
                String strSection;
                String strFilename(fileFinder.GetFileName());
                strFilename.Trim();

                int iPos = strFilename.IndexOf(_U("."));
                if (iPos != -1)
                {
                    String strExt = strFilename.Substring(iPos);
                    strExt.ToLower();

                    if (zipExt.IndexOf(strExt) != -1)
                    {
                        strFilePath.Format(_U("%s%s"), strPath.c_str(), strFilename.c_str());
                        strSection = strFilePath.Substring(strRoot.Length());

                        strSection.Replace(_U("\\"), _U("/"));

                        pZip->AddFile((TCHAR*)(strSection.c_str()), (TCHAR*)(strFilePath.c_str()));
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

void Utils::RemoveFiles(const String& strDir, bool bRemoveDir)
{
    InternalRemoveFiles(strDir, bRemoveDir);
    if (bRemoveDir)
    {
        ::RemoveDirectory(strDir.c_str());
    }
}

void Utils::InternalRemoveFiles(const String& strDir, bool bRemoveSubDir)
{
    FileFinder fileFinder;
    bool bSearch = true;

    if (!fileFinder.Open(strDir, _U("*.*")))
    {
        return;
    }

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
                String strCurPath;
                strCurPath.Format(_U("%s\\%s\\"), strDir.c_str(), fileFinder.GetFileName().c_str());

                // 读取子目录
                if (bRemoveSubDir)
                {
                    InternalRemoveFiles(strCurPath, bRemoveSubDir);
                    ::RemoveDirectory(strCurPath.c_str());
                }
            }
            else
            {
                String strFilename(fileFinder.GetFilePath());
                ::DeleteFile(strFilename.c_str());
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

/*
// 检测文件关联情况

// strExt: 要检测的扩展名(例如: ".txt")

// strAppKey: ExeName扩展名在注册表中的键值(例如: "txtfile")

// 返回TRUE: 表示已关联，FALSE: 表示未关联

BOOL CheckFileRelation(const char *strExt, const char *strAppKey)

{

    int nRet=FALSE;

    HKEY hExtKey;

    char szPath[_MAX_PATH];

    DWORD dwSize=sizeof(szPath);

    if(RegOpenKey(HKEY_CLASSES_ROOT,strExt,&hExtKey)==ERROR_SUCCESS)

    {

        RegQueryValueEx(hExtKey,NULL,NULL,NULL,(LPBYTE)szPath,&dwSize);

        if(_stricmp(szPath,strAppKey)==0)

        {

            nRet=TRUE;

        }

        RegCloseKey(hExtKey);

        return nRet;

    }

    return nRet;

}

//---------------------------------------------------------------------------

// 注册文件关联

// strExe: 要检测的扩展名(例如: ".txt")

// strAppName: 要关联的应用程序名(例如: "C:\MyApp\MyApp.exe")

// strAppKey: ExeName扩展名在注册表中的键值(例如: "txtfile")

// strDefaultIcon: 扩展名为strAppName的图标文件(例如: "C:\MyApp\MyApp.exe,0")

// strDescribe: 文件类型描述

void RegisterFileRelation(char *strExt, char *strAppName, char *strAppKey, char *strDefaultIcon, char *strDescribe)

{

    char strTemp[_MAX_PATH];

    HKEY hKey;

   

    RegCreateKey(HKEY_CLASSES_ROOT,strExt,&hKey);

    RegSetValue(hKey,"",REG_SZ,strAppKey,strlen(strAppKey)+1);

    RegCloseKey(hKey);   

    RegCreateKey(HKEY_CLASSES_ROOT,strAppKey,&hKey);

    RegSetValue(hKey,"",REG_SZ,strDescribe,strlen(strDescribe)+1);

    RegCloseKey(hKey);

    sprintf(strTemp,"%s\\DefaultIcon",strAppKey);

    RegCreateKey(HKEY_CLASSES_ROOT,strTemp,&hKey);

    RegSetValue(hKey,"",REG_SZ,strDefaultIcon,strlen(strDefaultIcon)+1);

    RegCloseKey(hKey);

    sprintf(strTemp,"%s\\Shell",strAppKey);

    RegCreateKey(HKEY_CLASSES_ROOT,strTemp,&hKey);

    RegSetValue(hKey,"",REG_SZ,"Open",strlen("Open")+1);

    RegCloseKey(hKey);

    sprintf(strTemp,"%s\\Shell\\Open\\Command",strAppKey);

    RegCreateKey(HKEY_CLASSES_ROOT,strTemp,&hKey);

    sprintf(strTemp,"%s \"%%1\"",strAppName);

    RegSetValue(hKey,"",REG_SZ,strTemp,strlen(strTemp)+1);

    RegCloseKey(hKey);

}

 

//使用这两个函数进行关联的示例代码

char strExt[10] = ".REC";

char strAppKey[20] = "WellTest.REC.1.0";         

BOOL relationExists = CheckFileRelation(strExt, strAppKey);

if(!relationExists)

{
　　char strAppName[MAX_PATH + 1] = "C:\\WellTest\\trunk\\bin\\WellTest.exe";                        

    char strDefaultIcon[MAX_PATH + 1] = "C:\\WellTest\\trunk\\bin\\WellTest.exe,0";                                     

    char strDescribe[100] = "WellTest Interpretation Files";

    RegisterFileRelation(strExt, strAppName, strAppKey, strDefaultIcon, strDescribe);

}


void WatchDirectory(LPTSTR lpDir)  
{  
   DWORD dwWaitStatus;  
   HANDLE dwChangeHandles[3];  
   TCHAR lpDrive[4];//存储磁盘符  
   TCHAR lpFile[_MAX_FNAME];//用于存储文件名  
   TCHAR lpExt[_MAX_EXT];//用于存储对应文件的后缀  
   
   _tsplitpath(lpDir, lpDrive,NULL, lpFile, lpExt);  
   lpDrive[2] = (TCHAR)'\\';  
   lpDrive[3] = (TCHAR)'\0';  
     
   //分别监控文件名，路径名，文件内容的修改   
   dwChangeHandles[0] =FindFirstChangeNotification(   
      lpDir,                         
      TRUE,                         
     FILE_NOTIFY_CHANGE_FILE_NAME); //文件名     
   if (dwChangeHandles[0] ==INVALID_HANDLE_VALUE)   
         ExitProcess(GetLastError());   
     
   dwChangeHandles[1] =FindFirstChangeNotification(   
      lpDrive,                     
      TRUE,  
     FILE_NOTIFY_CHANGE_DIR_NAME); //路径名    
   if (dwChangeHandles[1] ==INVALID_HANDLE_VALUE)   
    ExitProcess(GetLastError());  
   
   dwChangeHandles[2] =FindFirstChangeNotification(   
      lpDir,  
      TRUE,  
     FILE_NOTIFY_CHANGE_LAST_WRITE); //文件内容/或者说最后保存时间    
   if (dwChangeHandles[2] ==INVALID_HANDLE_VALUE)   
                   ExitProcess(GetLastError());   
     
   //改变通知已经设置完成，现在只需等待这些通知被触发，然后做相应处理  
   while (TRUE)   
   {   
         dwWaitStatus= WaitForMultipleObjects(3, dwChangeHandles, FALSE, INFINITE);   
         time(&ChangeTime);  
              
         switch(dwWaitStatus)  
         {  
         caseWAIT_OBJECT_0:   
                   //执行某对应操作  
                   RefreshDirectory(lpDir); //某操作接口  
                   if( FindNextChangeNotification(dwChangeHandles[0]) == FALSE )   
                            ExitProcess(GetLastError());   
                   break;  
         caseWAIT_OBJECT_0 + 1:  
                   //执行某对应操作  
                   RefreshTree(lpDrive);//某操作接口  
                   if(FindNextChangeNotification(dwChangeHandles[1]) == FALSE)   
                            ExitProcess(GetLastError());  
                   break;  
         caseWAIT_OBJECT_0 + 2:  
                   //执行某对应操作  
                   RefreshFile(lpDrive);//某操作接口  
                   if(FindNextChangeNotification(dwChangeHandles[2]) == FALSE)   
                            ExitProcess(GetLastError());  
                   break;       
         default:  
                   ExitProcess(GetLastError());  
         }  
   }  
}  
*/