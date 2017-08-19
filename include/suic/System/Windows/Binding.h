// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����Binding.h
// ��  �ܣ�������Ŀ�İ󶨻��ࡣ
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2011-05-02
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
/// Bindingת��������
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
/// �Ӵ˽ӿڼ̳еľ���˫��ͨ������
/// </Summary>
class SUICORE_API NotifyPropChanged : public Object
{
public:

    RTTIOfClass(NotifyPropChanged)

    NotifyPropChanged();
    virtual ~NotifyPropChanged();
    
    // �˷����̰߳�ȫ
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
    /// <summary>����Ϊһ�γ�ʼ��</summary> 
    OneTime,
    /// <summary>�������ݶ������UI��ʾ</summary>
    OneWay, 
    /// <summary>Դ��Ŀ��ͬʱ�䶯</summary>
    TwoWay, 
    /// <summary>�������ͬ�������ݶ�����ʾ</summary> 
    OneWayToSource,
};

class SUICORE_API BindingBase : public Extension
{
public:

    enum BindingSource
    {
        Unknown,
        // Ĭ�ϰ�
        Default,
        // ָ��Ԫ������
        ElementName,
        // ���ģ�游Ԫ��
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
    // ԴĿ����������
    DpProperty* _prop;
    // ��ԴĿ������
    String _target;

    SourceRef* _refSource;
    String _convertName;
    String _convertParam;
    IBindConverter* _converter;

    // ͨ��ָ�����ư󶨵�Ŀ��Ԫ��
    String _elemSource;
    // ͨ��IDataField��ȡ�û�����
    String _xpath;
    // �󶨵�Ԫ�����Զ���
    PropertyPath _ppath;

    // ��ģʽ
    BindingMode _bindMode;
    // ��Դ����
    BindingSource _sourceType;
    // ��������
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
