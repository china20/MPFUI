// 华勤科技版权所有 2010-2011
// 
// 文件名：Binding.h
// 功  能：定义核心库的绑定基类。
// 
// 作  者：MPF开发组
// 时  间：2011-05-02
// 
// ============================================================================

#ifndef _UIBINDING_H_
#define _UIBINDING_H_

#include <System/Windows/SourceRef.h>
#include <System/Windows/Extension.h>
#include <System/Windows/EventHandler.h>
#include <System/Tools/CollectionX.h>

namespace suic
{

class Element;
class FrameworkElement;

/// <summary>
/// Binding转换器声明
/// </summary>
class IBindConverter : public Object
{
public:

    virtual ~IBindConverter() {}

    virtual AutoObj Convert(Object* inVal, String param) = 0;
    virtual AutoObj ConvertBack(Object* inVal, String param) = 0;
};

class PropertyChangedEventArg : public EventArg
{
public:
    
    PropertyChangedEventArg(String name)
    {
        _propName = name;
    }

    String GetPropertyName() const
    {
        return _propName;
    }

private:

    String _propName;
};

struct PropListener
{
    WeakRef target;
    PropListener* next;
};

/// <Summary>
/// 从此接口继承的具有双向通信能力
/// </Summary>
class SUICORE_API NotifyPropChanged : public Object
{
public:

    RTTIOfClass(NotifyPropChanged)

    NotifyPropChanged();
    virtual ~NotifyPropChanged();
    
    // 此方法线程安全
    void NotifyChanged(const String& name);
    void ClearListeners();

private:

    void AddListener(Object* target);
    void RemoveListener(Object* target);

private:

    suic::Array<WeakRef> _listeners;

    friend class Object;
    friend class FrameworkElement;
};

enum BindingStatus
{ 
    Active = 0,
    Inactive,
    Unattached, 
    Detached,
    PathError,
    UpdateTargetError,
    UpdateSourceError, 
};

enum UpdateSourceTrigger
{ 
    ustDefault = 0,
    ustPropertyChanged,
    ustLostFocus,
    ustExplicit, 
};

enum BindingMode
{
    /// <summary>仅作为一次初始化</summary> 
    OneTime,
    /// <summary>根据数据对象更新UI显示</summary>
    OneWay, 
    /// <summary>源和目标同时变动</summary>
    TwoWay, 
    /// <summary>界面更新同步到数据对象显示</summary> 
    OneWayToSource,
};

class SUICORE_API BindingBase : public Extension
{
public:

    enum BindingSource
    {
        Unknown,
        // 默认绑定
        Default,
        // 指定元素名称
        ElementName,
        // 相对模版父元素
        RelativeTemplatedParent,
    };

    virtual ~BindingBase() {}

    RTTIOfClass(BindingBase)
};

struct SUICORE_API PropertyPath
{
    String Path;

    PropertyPath();
    PropertyPath(String val);
};

class BindingExpression;

class SUICORE_API Binding : public BindingBase
{
public:

    Binding();
    Binding(String path);
    ~Binding();

    RTTIOfClass(Binding)

    void ProvideValue(DpObject* d, DpProperty* dp, ObjectPtr& obj);

    String GetXPath();
    void SetXPath(const String& path);

    PropertyPath GetPath();
    void SetPath(const PropertyPath& path);

    DpProperty* GetProp();
    void SetProp(DpProperty* prop);

    SourceRef* GetSourceRef();
    void SetSourceRef(SourceRef* d);

    DpObject* GetSource();
    void SetSource(DpObject* d);

    BindingMode GetBindMode();
    void SetBindMode(BindingMode mode);

    UpdateSourceTrigger GetUpdateSourceTrigger();
    void SetUpdateSourceTrigger(UpdateSourceTrigger mode);

    String GetElementName() const;
    void SetElementName(const String& val);

    void SetConverter(IBindConverter* val);
    IBindConverter* GetConverter() const;

    void SetConverterParam(const String& val);
    String GetConverterParam() const;

    void SetConverterName(const String& val);
    String GetConverterName() const;

    BindingExpression* CreateBindingExpression();

protected:

    void ParseKeyValue(const String& keyval);

protected:

    WeakRef _source;
    // 源目标属性名称
    DpProperty* _prop;
    // 绑定源目标名称
    String _target;

    SourceRef* _refSource;
    String _convertName;
    String _convertParam;
    IBindConverter* _converter;

    // 通过指定名称绑定的目标元素
    String _elemSource;
    // 通过IDataField获取用户数据
    String _xpath;
    // 绑定到元素属性对象
    PropertyPath _ppath;

    // 绑定模式
    BindingMode _bindMode;
    // 绑定源类型
    BindingSource _sourceType;
    // 触发类型
    UpdateSourceTrigger _triggerType;

    friend class BindingExpression;
};

inline IBindConverter* Binding::GetConverter() const
{
    return _converter;
}

inline BindingMode Binding::GetBindMode()
{
    return _bindMode;
}

inline void Binding::SetBindMode(BindingMode mode)
{
    _bindMode = mode;
}

inline UpdateSourceTrigger Binding::GetUpdateSourceTrigger()
{
    return _triggerType;
}

inline void Binding::SetUpdateSourceTrigger(UpdateSourceTrigger trgType)
{
    _triggerType = trgType;
}

inline DpProperty* Binding::GetProp()
{
    return _prop;
}

inline void Binding::SetProp(DpProperty* prop)
{
    _prop = prop;
}

inline String Binding::GetXPath()
{
    return _xpath;
}

inline void Binding::SetXPath(const String& path)
{
    _xpath = path;
}

}

#endif
