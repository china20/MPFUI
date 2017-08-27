// 华勤科技版权所有 2008-2022
// 
// 文件名：Setter.h
// 功  能：Setter的访问实现。
// 
// 作  者：汪荣
// 时  间：2011-08-02
// 
// ============================================================================

#ifndef _UISETTER_H_
#define _UISETTER_H_

#include <System/Windows/Exception.h>
#include <System/Windows/DpObject.h>
#include <System/Windows/DpProperty.h>

namespace suic
{

class Setter;
typedef shared<Setter> SetterPtr;

class SUICORE_API Setter : public DpObject
{
private:

    DpProperty* _property;
    Object* _value; 
    String _target;
    String _unresolvedProperty;

public:

    Setter();
    ~Setter();

    RTTIOfClass(Setter)

    void Seal();

    DpProperty* GetProp();
    void SetProperty(DpProperty* prop);

    Object* GetValue();
    void SetValue(Object* val);

    Object* GetRawValue() { return _value; }

    String GetTargetName();
    void SetTargetName(String val);

    void SetUnresolvedProperty(String prop);
    String GetUnresolvedProperty() const;

protected:

    void CheckSealed();
    void Initialize(DpProperty* property, Object* value, String target);
    void CheckValidProperty(DpProperty* prop);
};

inline DpProperty* Setter::GetProp()
{ 
    return _property; 
}

inline void Setter::SetUnresolvedProperty(String prop)
{
    _unresolvedProperty = prop;
}

inline String Setter::GetUnresolvedProperty() const
{
    return _unresolvedProperty;
}

inline Object* Setter::GetValue()
{
    return _value; 
}

inline String Setter::GetTargetName()
{ 
    return _target;
}

inline void Setter::SetTargetName(String val)
{ 
    CheckSealed();
    _target = val; 
}

}

#endif
