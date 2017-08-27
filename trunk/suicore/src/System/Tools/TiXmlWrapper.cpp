
#include <System/Tools/TiXmlWrapper.h>
#include <System/Tools/ThemeOp.h>
#include <System/Interop/InternalOper.h>
#include <System/Interop/System.h>

namespace suic
{

TiXmlWrapper::TiXmlWrapper()
    : _root(NULL)
{
}

TiXmlWrapper::~TiXmlWrapper()
{
    Close();
}

bool IsUtfpEncode(char* pData)
{
    static unsigned char UTF8_C[3] = {0xEF,0xBB,0xBF};
    if (memicmp(pData, &UTF8_C[0], sizeof(UTF8_C)) == 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

FILE* TiFOpen(const char* filename, const char* mode)
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

bool TiXmlWrapper::LoadAsXml(const Byte* xml, int iSize)
{
    bool isXml = false;

    try
    {
        if (xml != NULL)
        {
            _xmlDoc.parse<0>(const_cast<char*>((char*)(xml)), iSize);
            _root = _xmlDoc.first_node();
        }
        _root = _xmlDoc.first_node();
    }
    catch (char* err)
    {
        SystemLog()->Warn(err);
    }
    catch (...)
    {
    }

    return (_root != NULL);
}

bool TiXmlWrapper::LoadMemory(const Byte* data, int size)
{
    bool bRet = true;
    int iSize = 0;

    //_content.Clear();
    //_content.Write((Byte*)xml, strlen(xml));

    //
    // 判断xml类型：字符串、路径、资源ID？
    //
    bRet = LoadAsXml(data, size);

    return bRet;
}

bool TiXmlWrapper::LoadUri(ResContext& resCtx, const String& uri)
{
    bool bRet = false;
    int iSize = 0;

    resCtx = ThemeOp::ReadResContext(uri, &_content);
    if (_content.GetBuffer() != NULL)
    {
        bRet = LoadAsXml(_content.GetBuffer(), _content.GetSize());
    }

    return bRet;
}

XmlNode* TiXmlWrapper::GetRoot()
{
    return _root;
}

bool TiXmlWrapper::Save(String path)
{
    return false;
}

void TiXmlWrapper::Close()
{
    _root = NULL;
    _content.Clear();
    _xmlDoc.clear();
}

}

