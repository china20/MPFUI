// 华勤科技版权所有 2010-2011
// 
// 文件名：system.h
// 功  能：负责绘制层叠窗口(带有alpha通道)
// 
// 作  者：MPF开发组
// 时  间：2010-07-12
// 
// ============================================================================

#ifndef _UIRESOURCEINFO_H_
#define _UIRESOURCEINFO_H_

#include <System/Tools/Log.h>
#include <System/Tools/XRUnzip.h>
#include <System/Tools/TreeBuilder.h>
#include <System/Types/Interface.h>

namespace suic
{

class EmptyThemeInfo : public IThemeInfo
{
public:

    EmptyThemeInfo()
    {
        setAutoDelete(false);
    }

    static EmptyThemeInfo* Ins()
    {
        static EmptyThemeInfo _ins;
        return &_ins;
    }

    int ReadRes(const String& key, ISStream* data)
    {
        return 0;
    }
};

class MemThemeInfo : public IThemeInfo
{
public:

    MemThemeInfo(IResDecryption* pDecrypt)
        : _decrypt(pDecrypt)
    {
        _resInit = false;
        _resHandle = NULL;

        if (NULL != _decrypt)
        {
            _decrypt->ref();
        }
    }

    ~MemThemeInfo()
    {
        if (NULL != _decrypt)
        {
            _decrypt->unref();
        }

        if (_resHandle != NULL)
        {
            ::FreeLibrary(_resHandle);
        }
        _unzip.Close();
    }

    int ReadRes(const String& name, ISStream* data)
    {
        int iSize = 0;

        if (_unzip.GetHZip() != NULL)
        {
            iSize = ReadZipRes(name, data);
        }
        else
        {
            iSize = ReadFileRes(name, data);

            if (0 == iSize)
            {
                iSize = ReadZipRes(name, data);
            }
        }

        return iSize;
    }

    bool InitRes(ByteStream& data, const String& pwd)
    {
        _data.Clear();
        data.Seek(0);
        StreamOp::WriteToStream(&_data, &data);
        return ReadResData(pwd);
    }

protected:

    bool ReadResData(const String& pwd);

private:

    int ReadFileRes(const String& strPath, ISStream* data);
    void HandleResName(LPTSTR lpResName);
    int ReadZipRes(const String& name, ISStream* data);

protected:

    ByteStream _data;
    XRUnzip _unzip;
    bool _resInit;
    String _path;
    HMODULE _resHandle; 
    IResDecryption* _decrypt;
};

}

#endif
