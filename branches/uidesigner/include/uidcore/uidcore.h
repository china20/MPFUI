// 华勤科技版权所有 2010-2011
// 
// 文件名：uidcore.h
// 功  能：设计器设计面板导出接口定义。
// 
// 作  者：汪荣
// 时  间：2012-07-02
// 
// ============================================================================

#ifndef _UIDCORE_H_
#define _UIDCORE_H_

#include <uidcore/uidres.h>
#include <uidcore/uidwgx.h>

class IUIDElement : public suic::RefBase
{
public:

    virtual ~IUIDElement() {}

    virtual UIDResPtr GetRes() = 0;
    virtual UIDWgxPtr GetWgx() = 0;
    virtual Element* GetElement() = 0;
    virtual void SetElement(Element* elem) = 0;

    virtual IUIDElement* GetParent() = 0;
    virtual int GetElementCount() = 0;
    virtual IUIDElement* GetElement(int index) = 0;
    virtual void RemoveElement(int index) = 0;
    virtual void RemoveAll() = 0;
};

typedef suic::shared<IUIDElement> UIDElementPtr;

class IUIDCoreEvent : public suic::RefBase
{
public:

    virtual ~IUIDCoreEvent() {}

    virtual void OnSetRootElement(IUIDElement* elem) = 0;
    virtual void OnFocused(IUIDElement* elem) = 0;
    virtual void OnMoveElement(suic::Rect rect) = 0;
    virtual void OnDeleteElement(IUIDElement* elem) = 0;
    virtual void OnAddElement(IUIDElement* elem) = 0;
};

class IUIDPanel : public suic::RefBase
{
public:

    virtual ~IUIDPanel() {}

    virtual int Load(suic::String path) = 0;
    virtual void Save() = 0;
    virtual void SaveAs(suic::String path) = 0;

    virtual IUIDElement* GetRootElement() = 0;
    virtual void SetRoot(suic::String elem) = 0;

    virtual void SetUIDCoreEvent(IUIDCoreEvent* evt) = 0;

    virtual void SetFocus(IUIDElement* elem) = 0;
    virtual void SetFocusByIndex(int index) = 0;
    virtual void SetFocusByName(suic::String name) = 0;
    virtual IUIDElement* GetElementByName(suic::String name) = 0;
    virtual void RemoveAll() = 0;

    virtual IUIDElement* AddElement(suic::String elem) = 0;
};

typedef suic::shared<IUIDPanel> UIDPanelPtr;

extern "C" void UIDGetPanel(UIDPanelPtr& frame);

#endif
