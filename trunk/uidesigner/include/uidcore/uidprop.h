// 华勤科技版权所有 2010-2011
// 
// 文件名：uidwgx.h
// 功  能：设计器控件属性导出接口定义。
// 
// 作  者：汪荣
// 时  间：2012-07-02
// 
// ============================================================================

#ifndef _UIDWGX_H_
#define _UIDWGX_H_

#include <suic/framework/uielement.h>

class IUIDProp : public suic::Object
{
public:

    virtual ~IUIDProp() {}

    virtual suic::String GetName() = 0;
    virtual suic::Object* GetValue() = 0;
};

class IUIDPropEvent : public suic::Object
{
public:

    virtual ~IUIDPropEvent() {}

    virtual OnPropChanged(const suic::String& name, suic::Object* val) = 0;
};

class IUIDWgx : public suic::Object
{
public:

    virtual ~IUIDWgx() {}

    virtual void ShowProp(Element* parent) = 0;
    virtual int GetCount() = 0;
    virtual IUIDProp* GetProp(int index) = 0;

    virtual void SetUIDPropEvent(IUIDPropEvent* evt) = 0;
};

typedef suic::shared<IUIDWgx> UIDWgxPtr;

extern "C" void UIDGetWgx(const suic::String& elemtype, UIDWgxPtr& frame);

#endif
