// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����ThemeInfo.h
// ��  �ܣ�������Ŀ�����������
// 
// ��  �ߣ�Sharpui������
// ʱ  �䣺2010-07-02
// 
// ============================================================================

#ifndef _UITHEMEINFO_H_
#define _UITHEMEINFO_H_

#include <System/Types/Structure.h>
#include <System/Types/StreamDef.h>
#include <System/Graphics/Resource.h>

namespace suic 
{

class IResDecryption;

class SUICORE_API IThemeInfo : public Object
{
public:

    virtual ~IThemeInfo() {}

    virtual int ReadRes(const String& key, ISStream* data) = 0;
};

class SUICORE_API ThemeLoader
{
public:

    /// <summary>
    ///  ����һ��Ŀ¼����������ﷵ��IThemeInfo�Ѿ����������ü���
    ///  �ⲿʹ�ú�������unref�ͷ�
    /// </summary>
    /// <param name="dir">Ŀ¼</param>
    /// <returns>�����������</returns>
    static IThemeInfo* LoadDir(const String& dir);
    static IThemeInfo* LoadUri(const String& path, const String& pwd, IResDecryption* pDecrypt);
    static IThemeInfo* LoadMemory(ByteStream& data, const String& pwd, IResDecryption* pDecrypt);

private:

    ThemeLoader();
};

enum ResReadFrom
{
    rrfDef,
    rrfApp,
    rrfRes,
    rrfFile,
    rrfSysDir,
    rrfSysRes,
};

/// 
/// <Summary>
///  ��ȡ��Դʱ����������Ϣ
/// </Summary>
/// 
struct ResContext
{
    String resPath;
    ResReadFrom rftType;    
    IThemeInfo* resInfo;

    ResContext() : resInfo(NULL), rftType(ResReadFrom::rrfDef) {}
};

}

#endif
