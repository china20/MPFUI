// 华勤科技版权所有 2008-2022
// 
// 文件名：PropertyFactory.h
// 功  能：读取、解析资源、属性值。
// 
// 作  者：MPF开发组
// 时  间：2011-08-02
// 
// ============================================================================

#ifndef _PROPERTYFACTORY_H_
#define _PROPERTYFACTORY_H_

#include <System/Tools/Collection.h>
#include <System/Resources/Style.h>
#include <System/Windows/FrameworkElement.h>

namespace suic
{

class IPropReader : public Object
{
public:

    virtual ~IPropReader() {}

    virtual void ReadProp(RTTIOfInfo* owner, const String& prop, IXamlNode* node, ObjectPtr& obj) = 0;
};

struct AttachDpInfo
{
    DpProperty* dp;
    RTTIOfInfo* rttiInfo;
};

class PropFactory
{
public:

    static void ReadProp(RTTIOfInfo* owner, const String& prop, String val, ObjectPtr& obj);
    static void ReadProp(RTTIOfInfo* owner, const String& prop, IXamlNode* node, ObjectPtr& obj);

    static void RegisterPropReader(const String& prop, IPropReader* val);
    static IPropReader* FindPropReader(const String& prop);

    static bool ConvertAttachProp(const String& name, AttachDpInfo& attachDb);

    static void Clear();
    
private:

    PropFactory();

    static StringAutoDic<Object> _props;
};

}

#endif
