/////////////////////////////////////////////////////
// Encry.cpp

#include "Encrypt.h"
#include "Decrypt.h"
#include <System/Tools/CoreHelper.h>

typedef struct tagResourceHeader
{
    // 本结构大小
    WORD dwSize;
    // 资源加密算法
    WORD dwEncry;
    // 文件长度
    DWORD dwLen;
    // 版本号
    DWORD dwVersion;
    // 资源创建日期
    DWORD dwDate;
    // 资源更新日期
    DWORD dwUpdateDate;

    // 资源文件密码
    char pwd[20];
    // 作者
    char author[16];
    // 资源描述
    char comment[64];
}ResourceHeader;

struct DesignLicense
{
    WORD dwSize;
    WORD dwUseTimes;
    WORD dwMaxUseTimes;
    // 第一次启动时间
    DWORD dwStartTime;
    // 最长使用时间
    DWORD dwMaxUseTime;
    char szLicense[32];
    char szTag[16]; 
    char szUseCode[32];
    char szCompany[128];
    char szDesc[128];
};

UIEncrypt::UIEncrypt()
{
}

UIDecrypt::UIDecrypt()
{
}

static __int64 GetCryPwd(suic::Mulstr pwd)
{
	__int64 mc= 9757735233305;

    for(int i = 0; i < (int)pwd.Length(); i++)
	{
		mc ^= pwd[i] | 128;
	}

	return mc;
}

int ReadFileData(const suic::String& strPath, suic::Mulstr& data)
{
    suic::String strFile(suic::FileDir::CalculatePath(strPath));
    int iSize = 0;

    HANDLE hFile = CreateFile(strFile.c_str(), GENERIC_READ, FILE_SHARE_READ, 
        NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);

    if (hFile != INVALID_HANDLE_VALUE)
    {
        DWORD readBytes = 0;
        iSize = (int)GetFileSize(hFile, NULL);

        data.Resize(iSize);

        ReadFile(hFile, (LPVOID)data.c_str(), iSize, &readBytes, NULL);

        if (iSize != readBytes)
        {
            data = "";
            iSize = 0;
        }

        CloseHandle(hFile);
    }

    return iSize;
}

int UIDecrypt::Decrypt(const suic::String& pwd, suic::String path, suic::String outFile)
{
    suic::Mulstr data;
    if (ReadFileData(path, data) <= 0)
    {
        return 0;
    }
    else
    {
        BYTE* out = NULL;
        int iOutSize = Decrypt(pwd, &out, (suic::Byte*)data.c_str(), data.Length());

        if (iOutSize > 0)
        {
            HANDLE hFile = CreateFile(outFile.c_str(), GENERIC_WRITE, FILE_SHARE_WRITE, 
                NULL, CREATE_ALWAYS, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
            if (hFile != INVALID_HANDLE_VALUE)
            {
                DWORD writeBytes = 0;
                WriteFile(hFile, (LPCVOID)out, iOutSize, &writeBytes, NULL);
                CloseHandle(hFile);
            }
            delete out;
        }

        return data.Length();
    }
}

int UIDecrypt::Decrypt(const suic::String& pwd, BYTE** out, suic::Byte* data, int iSize)
{
    int iDataSize = 0;
    ResourceHeader* pRes = (ResourceHeader*)data;

    if (pRes->dwEncry == 0 
        && pRes->dwSize == sizeof(ResourceHeader)
        && pRes->dwLen == iSize - sizeof(ResourceHeader))
    {
        int i = 0;
        iDataSize = iSize - sizeof(ResourceHeader);

        for (i = 0; i < sizeof(pRes->pwd); i++)
        {
            pRes->pwd[i] ^= iDataSize;
        }

        if (pwd.Equals(suic::String(pRes->pwd, 0)))
        {
            __int64 pwdid = GetCryPwd(pRes->pwd);
            *out = new BYTE[iDataSize];
            memcpy(*out , data + sizeof(ResourceHeader), iDataSize);

            for (i = 0; i < iDataSize; i++)
            {
                *out[i] ^= pwdid;
                *out[i] ^= iDataSize;
            }
        }
    }

    return iDataSize;
}

int UIEncrypt::Encrypt(const suic::String& strPwd, suic::String strFile)
{
    if (strFile.Empty())
    {
        // 加密文件为空
        return -1;
    }

    __int64 pwdid = GetCryPwd(strPwd.c_str());

    BYTE* data = NULL;
    int dwSize = 0;
    bool bSucc = false;

    HANDLE hFile = CreateFile(strFile.c_str(), GENERIC_READ, FILE_SHARE_READ, 
        NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);

    if (hFile != INVALID_HANDLE_VALUE)
    {
        DWORD readBytes = 0;
        dwSize = (int)GetFileSize(hFile, NULL);
        data = new BYTE[dwSize];
        ReadFile(hFile, (LPVOID)data, dwSize, &readBytes, NULL);

        if (dwSize != readBytes)
        {
            delete data;
            data = NULL;
        }
        else
        {
            bSucc = true;
        }

        CloseHandle(hFile);
    }

    if (bSucc)
    {
        BYTE* out = NULL;
        int iOutSize = UIDecrypt::Decrypt(strPwd, &out, data, dwSize);
        hFile = CreateFile(strFile.c_str(), GENERIC_WRITE, FILE_SHARE_WRITE, 
            NULL, CREATE_ALWAYS, FILE_FLAG_SEQUENTIAL_SCAN, NULL);

        if (iOutSize > 0)
        {
            dwSize = iOutSize;
            delete data;
            data = out;
        }

        if (hFile != INVALID_HANDLE_VALUE)
        {
            DWORD writeBytes = 0;
            ResourceHeader resHead;

            memset(&resHead, 0, sizeof(ResourceHeader));
            resHead.dwLen = dwSize;
            resHead.dwSize = sizeof(ResourceHeader);
            strncpy(resHead.pwd, suic::Mulstr(strPwd.c_str()).c_str(), sizeof(resHead.pwd));

            int i = 0;

            for (i = 0; i < dwSize; i++)
            {
                data[i] ^= pwdid;
                data[i] ^= dwSize;
            }

            for (i = 0; i < sizeof(resHead.pwd); i++)
            {
                resHead.pwd[i] ^= dwSize;
            }

            WriteFile(hFile, (LPCVOID)&resHead, sizeof(ResourceHeader), &writeBytes, NULL);
            WriteFile(hFile, (LPCVOID)data, dwSize, &writeBytes, NULL);

            CloseHandle(hFile);
        }
        else
        {
            bSucc = false;
        }

        delete[] data;
    }

    return (bSucc ? 0 : -2);
}
