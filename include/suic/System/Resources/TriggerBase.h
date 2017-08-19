// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����TriggerBase.h
// ��  �ܣ�TriggerBase�ķ���ʵ�֡�
// 
// ��  �ߣ�����
// ʱ  �䣺2011-08-02
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

    // �洢�������������ֵ
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
