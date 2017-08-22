// 华勤科技版权所有 2010-2012
// 
// 文件名：Expression.h
// 功  能：依赖属性的基类，完成依赖属性实现等逻辑。
// 
// 作  者：MPF开发组
// 时  间：2011-05-01
// 
// ============================================================================

#ifndef _UIEXPRESSION_H_
#define _UIEXPRESSION_H_

#include <System/Windows/DpObject.h>

namespace suic
{

class SUICORE_API Expression : public Object
{
public:

    static Object* NoValue;

    RTTIOfClass(Expression)

    Expression();
    ~Expression();

    bool IsEnabled() const;

    DpObject* GetSource();
    void SetSource(DpObject* d);

public:

    virtual void OnAttach(DpObject* d, DpProperty* dp);
    virtual void OnDetach(DpObject* d, DpProperty* dp);

    virtual bool GetValue(DpObject* d, DpProperty* dp, ObjectPtr& val); 
    virtual bool SetValue(DpObject* d, DpProperty* dp, Object* value);
    
    virtual bool OnUpdateProp(DpObject* d, DpProperty* dp);

protected:

    void SetEnabled(bool val);

private:

    bool _enabled;
    WeakRef _source;

    friend class DpObject;
    friend class DependentList;
};

inline bool Expression::IsEnabled() const
{
    return _enabled;
}

inline void Expression::SetEnabled(bool val)
{
    _enabled = val;
}

} 

#endif
