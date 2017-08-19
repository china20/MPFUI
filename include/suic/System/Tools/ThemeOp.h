
#ifndef _UITHEMEOP_H_
#define _UITHEMEOP_H_

#include <System/Tools/Array.h>
#include <System/Tools/Collection.h>
#include <System/Windows/ResourceUri.h>
#include <Framework/Controls/Window.h>

namespace suic
{ 

enum ResReadMask
{
    ReadDefMask = 1,
    ReadSysMask = 2,
    ReadAppMask = 4,
    ReadResMask = 8,
    ResReadAllMask = ReadDefMask | ReadSysMask | ReadAppMask | ReadResMask,
};

class SUICORE_API ThemeOp
{
public:

    /// <summary>
    ///  ����ϵͳ��Դ��
    /// </summary>
    /// <param name="name">��Դ����</param>
    /// <param name="dir">Ŀ¼</param>
    /// <returns>�ɹ�����true;ʧ�ܷ���false</returns>
    static void AddDir(const String& name, const String& dir);

    /// <summary>
    ///  ����ϵͳ��Դ��
    /// </summary>
    /// <param name="path">��Դ·��</param>
    /// <param name="pwd">��Դ������</param>
    /// <returns>�ɹ�����true;ʧ�ܷ���false</returns>
    static void AddTheme(const String& name, const String& path, const String& pwd);
    static void AddMemory(const String& name, ByteStream& data, const String& pwd);

    static void AddThemeInfo(const String& name, IThemeInfo* themeInfo);
    
    static void RemoveTheme(const String& name);
    static IThemeInfo* FindTheme(const String& name);

    static int ReadRes(const String& name, ISStream* buff);
    static int ReadRes(const ResourceUri* resUri, ISStream* buff);
    static int ReadRes(const String& resid, const String& name, ISStream* buff);

    static IThemeInfo* GetSystemDir();
    static void SetSystemDir(const String& dir);

    static IThemeInfo* GetSystemTheme();
    static void SetSystemTheme(IThemeInfo* themeInfo);
    static void SetSystemTheme(const String& path, const String& pwd);

    static ResContext ReadResContext(const String& name, ISStream* buff);
    static ResContext ReadResContext(const ResourceUri* resUri, ISStream* buff);

    static suic::Uint16 GetReadResourceMask();
    static void SetReadResourceMask(suic::Uint16 uMask);

private:

    static IThemeInfo* GetDefaultRes();

    static ResContext ReadResCommon(const String& name, ISStream* buff);
    static int ReadDefRes(ResContext& resCtx, const String& name, ISStream* buff);
    static int ReadResById(ResContext& resCtx, const String& themeName, const String& strId, ISStream* buff);

private:

    ThemeOp();

    static suic::Uint16 _readMask;
 
    static StringAutoDic<IThemeInfo> _reses;

    static IThemeInfo* _systemDir;
    static IThemeInfo* _systemRes;

    friend class CoreHelper;
    friend class CoreSystem;
};

inline suic::Uint16 ThemeOp::GetReadResourceMask()
{
    return _readMask;
}

}

#endif
