// 华勤科技版权所有 2010-2011
// 
// 文件名：DataTrigger.h
// 功  能：DataTrigger的访问实现。
// 
// 作  者：汪荣
// 时  间：2011-08-02
// 
// ============================================================================

#ifndef _UIDATATRIGGER_H_
#define _UIDATATRIGGER_H_

#include <System/Tools/Array.h>
#include <System/Tools/Collection.h>
#include <System/Resources/TriggerBase.h>

namespace suic
{

class SUICORE_API DataTrigger : public TriggerBase
{
private:

    Object* _value;
    BindingBase* _binding;
    SetterCollection* _setters;

    friend class Style;

public:

    RTTIOfClass(DataTrigger)

    DataTrigger();
    ~DataTrigger();

    BindingBase* GetBinding();
    void SetBinding(BindingBase* val); 

    Object* GetValue();
    void SetValue(Object* val);

    SetterCollection* GetSetters();

    void Seal();
};

}

#endif
