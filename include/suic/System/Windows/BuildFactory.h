// 华勤科技版权所有 2008-2022
// 
// 文件名：BuildFactory.h
// 功  能：管理元素的创建。
// 
// 作  者：MPF开发组
// 时  间：2010-07-02
// 
// ============================================================================

#ifndef _UIBUILDFACTORY_H_
#define _UIBUILDFACTORY_H_

#include <System/Windows/Object.h>
#include <System/Windows/Binding.h>
#include <System/Tools/Collection.h>

namespace suic
{

class IAddType
{
public:

    virtual ~IAddType() {}

    virtual RTTIOfInfo* AddType(const String& strType) = 0;
};

class SUICORE_API BuildFactory
{
public:

    static BuildFactory* Ins();

    RTTIOfInfo* Lookup(const String& strType);

    bool Contains(const String& strType);
    bool Contains(RTTIOfInfo* cls);

    void Remove(const String& strType);
    void Remove(RTTIOfInfo* cls);
    void Add(RTTIOfInfo* cls);

    void RemoveAddType(IAddType* addType);
    void RegisterAddType(IAddType* addType);

protected:

    BuildFactory();

    RTTIOfInfo* AddType(const String& strType);
    RTTIOfInfo* AddSystmeType(const String& strType);

private:

    static BuildFactory* _ins;

    Array<IAddType*> _addTypes;
    StringDic<RTTIOfInfo*> _classes;
    StringDic<RTTIOfInfo*> _sysClasses;
};

class SUICORE_API LocalTypes
{
public:

    static LocalTypes* Ins();

    RTTIOfInfo* Lookup(const String& strType);

    bool Contains(const String& strType);
    bool Contains(RTTIOfInfo* cls);

    void Remove(const String& strType);
    void Remove(RTTIOfInfo* cls);
    void Add(RTTIOfInfo* cls);

protected:

    LocalTypes();

private:

    static LocalTypes* _ins;
    StringDic<RTTIOfInfo*> _classes;
};

class SUICORE_API BindConverter
{
public:

    static BindConverter* Ins();

    IBindConverter* Lookup(const String& strName);

    bool Contains(const String& strName);
    void Remove(const String& strName);
    void Clear(const String& strType);
    void Add(const String& strName, IBindConverter* val);

protected:

    BindConverter();

private:

    static BindConverter* _ins;

    StringAutoDic<Object> _converters;
    StringAutoDic<Object> _sysConverters;
};

}

#endif
