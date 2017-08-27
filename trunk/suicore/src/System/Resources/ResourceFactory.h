// 华勤科技版权所有 2010-2011
// 
// 文件名：ResourceFactory.h
// 功  能：资源工厂实实现，加载不同的资源。
// 
// 作  者：Sharpui开发组
// 时  间：2011-08-02
// 
// ============================================================================

#ifndef _UIRESOURCEFACTORY_H_
#define _UIRESOURCEFACTORY_H_

#include <System/Windows/Object.h>
#include <System/Types/Interface.h>
#include <System/Resources/SetterCollection.h>
#include <System/Windows/FrameworkElement.h>

namespace suic
{

class ResourceFactory : public Object
{
public:

    ResourceFactory(ResourceDictionary* resDic);

    static SetterPtr HandleSetter(FrameworkElement* pElem, const String& property, const String& val);

    String ReadNodeAsString(IXamlNode* pNode);
    String BuildResource(FrameworkElement* pElem, IXamlNode* pNode, RefBasePtr& obj, bool bCall);    

    void ReadSetter(SetterCollection* pColl, FrameworkElement* pElem, IXamlNode* pNode, bool bCall);
    TriggerPtr ReadTrigger(const String& name, FrameworkElement* pElem, IXamlNode* pNode, int index, bool bCall);

protected:

    ResourceDictionary* GetResDic() const;

    void ReadTriggerGroup(TriggerCollection* trgColl, FrameworkElement* pElem, IXamlNode* pNode, bool bCall);
    String ReadStyleResource(FrameworkElement* pElem, IXamlNode* pNode, RefBasePtr& obj, bool bCall);

protected:

    ResourceDictionary* _resDic;
};

inline ResourceDictionary* ResourceFactory::GetResDic() const
{
    return _resDic;
}

class PropertyFactory : public Object
{
public:

    static RefBasePtr GetProperty(String prop, String val);
};

}

#endif
