// 华勤科技版权所有 2008-2022
// 
// 文件名：FrameElementFactory.h
// 功  能：实现控件模板工厂。
// 
// 作  者：MPF开发组
// 时  间：2011-08-02
// 
// ============================================================================

#ifndef _UIFRAMEELEMENTFACTORY_H_
#define _UIFRAMEELEMENTFACTORY_H_

#include <System/Windows/Exception.h>
#include <System/Windows/Element.h>
#include <System/Windows/Binding.h>
#include <System/Windows/PropertyValue.h>

#include <System/Types/Interface.h>
#include <System/Tools/CollectionX.h>

namespace suic 
{

class FrameworkTemplate;
class FrameworkElement;

struct TemplateReadInfo
{
    FrameworkElement* owner;
    ResourceDictionary* resDic;
};

class CommandProp : public Object
{
public:

    String val;
    CommandProp(String v)
    {
        val = v;
    }
};

class SUICORE_API FEFactory
{
public:

    FEFactory();
    FEFactory(String text);
    FEFactory(RTTIOfInfo* type);
    FEFactory(RTTIOfInfo* type, String name);

    ~FEFactory();

    static FEFactory* Create();

    RTTIOfInfo* GetType() const;
    void SetType(RTTIOfInfo* type);

    void Seal();
    bool IsSealed() const;

    String GetText();
    void SetText(String text);

    String GetName();
    void SetName(String name);

    int GetPropCount() const;
    PropValue* GetPropValue(int index) const;

    FEFactory* GetParent();
    FEFactory* GetFirstChild();
    FEFactory* GetNextSibling();
    FrameworkTemplate* GetFrameworkTemplate();

    void AddChild(FEFactory* child);

    Object* GetValue(DpProperty* dp);
    int SetValue(DpProperty* dp, Object* value);

    void SetBinding(DpProperty* dp, Binding* binding);

    void ReadElementProperties(FrameworkElement* elem);
    void SetResourceReference(DpProperty* dp, Object* name);
    
    int GetPropValueCount() const;
    PropValue* GetPropValue(int index);

    void AutoDoProperties();
    void MappingChildName(DpProperty* dp, const String& val);

    void BeginInit(DpObject* container, DpObject* parent, FrameworkElement* elem);
    void EndInit(DpObject* container, FrameworkElement* elem);

    void BindCountTemplateElement(XArray* bindColl, DpObject* elem);

protected:

    bool IsDefaultValue(DpProperty* dp) const;
    void UpdatePropertyValueList(DpProperty* dp, PropValType valueType, Object* value);
    void AppendChild(FEFactory* child);
    bool LoadChildFromNode(const TemplateReadInfo* info, IXamlNode* node, bool bFromDt);

    void ClearChildren();

private:

    String _text;
    String _childName;
    RTTIOfInfo* _type;

    bool _sealed;
    int _childIndex;
    // 存储经过处理的属性值
    PropValueArray _propValues;

    FrameworkTemplate* _frameworkTemplate;

    FEFactory* _parent;
    FEFactory* _firstChild; 
    FEFactory* _lastChild;
    FEFactory* _nextSibling;

    friend class FrameworkTemplate;
};

inline RTTIOfInfo* FEFactory::GetType() const
{
    return _type; 
}

inline String FEFactory::GetText()
{
    return _text;
}

inline String FEFactory::GetName()
{
    return _childName; 
}

inline FEFactory* FEFactory::GetParent()
{ 
    return _parent;
}

inline FEFactory* FEFactory::GetFirstChild()
{
    return _firstChild;
}

inline FEFactory* FEFactory::GetNextSibling()
{ 
    return _nextSibling;
} 

inline FrameworkTemplate* FEFactory::GetFrameworkTemplate()
{
    return _frameworkTemplate;
}

}

#endif
