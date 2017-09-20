// 华勤科技版权所有 2010-2011
// 
// 文件名：DragEventArg.h
// 功  能：定义核心库的拖动事件。
// 
// 作  者：Sharpui开发组
// 时  间：2010-07-02
// 
// ============================================================================

#ifndef _UIDRAGEVENTARG_H_
#define _UIDRAGEVENTARG_H_

#include <System/Windows/DpObject.h>
#include <System/Windows/RoutedEventArg.h>

namespace suic
{

class Element;

enum DragDropEffects
{
    eDragAll = -23645,
    eDragCopy = 1,
    eDragLink = 4,
    eDragMove = 2,
    eDragNone = 0,
    eDragScroll = -23646,
};

class SUICORE_API GiveFeedbackEventArg : public suic::RoutedEventArg
{
private:

    DragDropEffects _effects;
    bool _useDefaultCursors;

public:

    GiveFeedbackEventArg(DragDropEffects effects, bool useDefaultCursors)
    {
    }

    DragDropEffects GetEffects()
    {
        return _effects;
    }

    bool GetUseDefaultCursors() const
    {
        return _useDefaultCursors;
    }

    void SetUseDefaultCursors(bool val)
    {
        _useDefaultCursors = val;
    }
};

enum DragAction
{
    Continue,
    Drop,
    Cancel
};

enum DragDropKeyStates
{
    eDragKeyNone = 0,
    eDragKeyLeftMouseButton = 1,
    eDragKeyRightMouseButton = 2,
    eDragKeyShiftKey = 4,
    eDragKeyControlKey = 8,
    eDragKeyMiddleMouseButton = 0x10,
    eDragKeyAltKey = 0x20,
};

class SUICORE_API QueryContinueDragEventArg : public RoutedEventArg
{
private:

    DragAction _action;
    DragDropKeyStates _dragDropKeyStates;
    bool _escapePressed;

public:

    QueryContinueDragEventArg(bool escapePressed, DragDropKeyStates dragDropKeyStates)
    {
    }

    DragAction GetAction() const
    {
        return _action;
    }

    void SetAction(DragAction val)
    {
        _action = val;
    }

    bool IsEscapePressed() const
    {
        return _escapePressed;
    }

    DragDropKeyStates GetKeyStates() const
    {
        return _dragDropKeyStates;
    }
};

class SUICORE_API FileDragInfo : public Object
{
public:

    FileDragInfo(String* files, int count);
    ~FileDragInfo();

    int GetCount() const;
    String GetFilePath(int index) const;

private:

    int _count;
    String* _files;
};

class IDataStore : public Object
{
public:

    virtual ~IDataStore() {}

    virtual Object* GetData(String format) = 0;
    virtual Object* GetData(String format, bool autoConvert) = 0;
    virtual bool GetDataPresent(String format) = 0;
    virtual bool GetDataPresent(String format, bool autoConvert) = 0;
    virtual int GetFormatsCount() = 0;
    virtual String GetFormats(int index, bool autoConvert) = 0;
    virtual void SetData(Object* data) = 0;
    virtual void SetData(String format, Object* data) = 0;
    virtual void SetData(String format, Object* data, bool autoConvert) = 0;
};

class SUICORE_API DragEventArg : public RoutedEventArg
{
private:
    
    suic::Point _dropPoint;
    suic::DpObject* _target;
    IDataStore* _data;
    DragDropEffects _effects;
    DragDropEffects _allowedEffects;
    DragDropKeyStates _dragDropKeyStates;

    suic::eDropType _dropType;

public:

    DragEventArg(IDataStore* data, DragDropKeyStates dragDropKeyStates, 
        DragDropEffects allowedEffects, DpObject* target, Point point);
    DragEventArg(IDataStore* data, DragDropKeyStates dragDropKeyStates, 
        DragDropEffects allowedEffects, DpObject* target, Point point, suic::eDropType t);

    ~DragEventArg();

    suic::eDropType GetDropType() const;
    suic::Point GetPosition(suic::Element* relativeTo);
    DragDropEffects GetAllowedEffects() const;
    IDataStore* GetData() const;
    DragDropEffects GetEffects() const;
    void SetEffects(DragDropEffects val);
    DragDropKeyStates GetKeyStates() const;
};

inline suic::eDropType DragEventArg::GetDropType() const
{
    return _dropType;
}

}

#endif
