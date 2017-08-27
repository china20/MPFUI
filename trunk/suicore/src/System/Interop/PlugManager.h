// 华勤科技版权所有 2008-2022
// 
// 文件名：plugmanager.h
// 功  能：插件管理
// 
// 作  者：MPF开发组
// 时  间：2010-07-12
// 
// ============================================================================

#ifndef _UIPLUGMANAGER_H_
#define _UIPLUGMANAGER_H_

#include <System/Windows/FrameworkElement.h>
#include <System/Types/Interface.h>
#include <System/Tools/Array.h>
#include <System/Tools/Collection.h>

namespace suic
{

class PlugManager
{
public:

    static PlugManager* Ins();

    PlugManager();
    ~PlugManager();

    void AddPlug(const String& path);
    void RemovePlug(const String& path);
    bool ContainsPlug(const String& path);

    void LoadPlugXml(const String& xml);
    void InitPlugs();
    void ClearPlugs();

protected:

    bool _isInit;
    StringDic<HMODULE> _mapPlug;
};

}

#endif
