// 华勤科技版权所有 2010-2012
// 
// 文件名：ResourceReferenceExpression.h
// 功  能：动态资源对象。
// 
// 作  者：MPF开发组
// 时  间：2011-05-01
// 
// ============================================================================

#ifndef _UIRESOURCEREFERENCEEXPRESSION_H_
#define _UIRESOURCEREFERENCEEXPRESSION_H_

#include <System/Windows/Expression.h>

namespace suic
{

class RRExpression : public Expression
{
public:

    RTTIOfAbstractClass(RRExpression)

    RRExpression(const String& name);
    ~RRExpression();

    void SetResourceKey(const String& val);
    String GetResourceKey() const;

    RRExpression* Clone();

    bool GetValue(DpObject* d, DpProperty* dp, ObjectPtr& val);
    bool SetValue(DpObject* d, DpProperty* dp, Object* value);

    friend class DpObject;

private:

    String _resourceKey;
};

inline void RRExpression::SetResourceKey(const String& val)
{
    _resourceKey = val;
}

inline String RRExpression::GetResourceKey() const
{
    return _resourceKey;
}

class ARRExpression : public RRExpression
{
public:

    ARRExpression(const String& name);
    ~ARRExpression();

    Object* GetValue(DpObject* d, DpProperty* dp);
};

} 

#endif
