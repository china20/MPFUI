// 华勤科技版权所有 2010-2011
// 
// 文件名：TriggerBase.h
// 功  能：TriggerBase的访问实现。
// 
// 作  者：汪荣
// 时  间：2011-08-02
// 
// ============================================================================

#ifndef _UITRIGGERBASE_H_
#define _UITRIGGERBASE_H_

#include <System/Windows/PropertyValue.h>
#include <System/Resources/SetterCollection.h>

namespace suic
{

class SUICORE_API TriggerBase : public DpObject
{
private:

    // 存储经过处理的属性值
    PropValueArray _propValues;

    friend class Style;
    friend class ControlTemplate;

public:

    RTTIOfClass(TriggerBase)

    TriggerBase();
    ~TriggerBase();

    PropValueArray* GetPropValues();

    void Seal();

    virtual SetterCollection* GetSetters();
    virtual bool IsMatch(DpObject* d);
    virtual bool CheckProperty(DpProperty* dp);
    virtual void MatchValue(DpObject* d, DpProperty* dp, Object* oldVal, Object* newVal, bool& bOld, bool& bNew);

protected:

    void ProcessSettersCollection(SetterCollection* setters);
    void AddToPropertyValues(String childName, DpProperty* dp, Object* value, PropValType valueType);
};

typedef shared<TriggerBase> TriggerBasePtr;

inline PropValueArray* TriggerBase::GetPropValues()
{
    return &_propValues;
}

}

#endif
