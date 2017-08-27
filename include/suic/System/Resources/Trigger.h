// 华勤科技版权所有 2008-2022
// 
// 文件名：Trigger.h
// 功  能：Trigger的访问实现。
// 
// 作  者：汪荣
// 时  间：2011-08-02
// 
// ============================================================================

#ifndef _UITRIGGER_H_
#define _UITRIGGER_H_

#include <System/Tools/Array.h>
#include <System/Tools/Collection.h>
#include <System/Resources/TriggerBase.h>

namespace suic
{

struct SUICORE_API Condition
{
    DpProperty* prop;
    String sourceName;

    Condition()
    {
        prop = NULL;
        val = DpProperty::UnsetValue();
        val->ref();
    }

    Condition(const Condition& Other)
    {
        prop = Other.prop;
        val = Other.val;
        val->ref();
        sourceName = Other.sourceName;
    }

    Condition& operator=(const Condition& Other)
    {
        if (&Other != this)
        {
            prop = Other.prop;
            SETREFOBJ(val, Other.val);
            sourceName = Other.sourceName;
        }

        return (*this);
    }

    ~Condition()
    {
        FREEREFOBJ(val);
    }

    Object* GetValue()
    {
        return val;
    }

    void SetValue(Object* v)
    {
        SETREFOBJ(val, v);
    }

    void Seal()
    {

    }

private:

    Object* val;
};

typedef Array<Condition> ArrayCondition;
typedef PointerDic<DpProperty*, Condition> DicCondition;

class SUICORE_API Trigger : public TriggerBase
{
private:

    DpProperty* _prop;
    String _sourceName;
    Object* _value;
    SetterCollection* _setters;

    friend class Style;

public:

    RTTIOfClass(Trigger)

    Trigger();
    ~Trigger();

    DpProperty* GetProp();
    void SetProperty(DpProperty* val); 

    Object* GetValue();
    void SetValue(Object* val);

    String GetSourceName();
    void SetSourceName(String val);

    SetterCollection* GetSetters();
    void AddSetter(Setter* value);

    void Seal();

    bool IsMatch(DpObject* d);
    bool CheckProperty(DpProperty* dp);
    void MatchValue(DpObject* d, DpProperty* dp, Object* oldVal, Object* newVal, bool& bOld, bool& bNew);
};

typedef shared<Trigger> TriggerPtr;

inline DpProperty* Trigger::GetProp()
{
    return _prop;
}

inline Object* Trigger::GetValue()
{
    return _value;
}

inline String Trigger::GetSourceName()
{
    return _sourceName;
}

class SUICORE_API MultiTrigger : public TriggerBase
{
private:

    enum ConditionOper {
        coAnd,
        coOr,
    };

    ArrayCondition _conds;
    SetterCollection* _setters;

    friend class Style;

public:

    RTTIOfClass(MultiTrigger)

    MultiTrigger();
    ~MultiTrigger();

    void Seal();

    void SetConditionOper(MultiTrigger::ConditionOper oper);
    MultiTrigger::ConditionOper GetConditionOper() const;

    ArrayCondition* GetConditions();
    SetterCollection* GetSetters();

    bool IsMatch(DpObject* d);
    bool CheckProperty(DpProperty* dp);
    void MatchValue(DpObject* d, DpProperty* dp, Object* oldVal, Object* newVal, bool& bOld, bool& bNew);

private:

    MultiTrigger::ConditionOper _oper;
};

}

#endif
