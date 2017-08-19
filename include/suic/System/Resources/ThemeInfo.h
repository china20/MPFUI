// 华勤科技版权所有 2010-2011
// 
// 文件名：ThemeInfo.h
// 功  能：定义核心库的主题操作。
// 
// 作  者：Sharpui开发组
// 时  间：2010-07-02
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
    ///  创建一个目录主题对象，这里返回IThemeInfo已经增加了引用计数
    ///  外部使用后必须调用unref释放
    /// </summary>
    /// <param name="dir">目录</param>
    /// <returns>返回主题对象</returns>
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
///  读取资源时的上下文信息
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
