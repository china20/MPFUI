// 华勤科技版权所有 2008-2022
// 
// 文件名：DragEvent.h
// 功  能：拖动事件。
// 
// 作  者：MPF开发组
// 时  间：2011-07-02
// 
// ============================================================================


#ifndef _UIDRAGEVENT_H_
#define _UIDRAGEVENT_H_

#include <System/Windows/Element.h>
#include <System/Windows/RoutedEventArg.h>

namespace suic
{

class DragStartedEventArg : public RoutedEventArg
{
public:

    DragStartedEventArg(Float horizontalOffset, Float verticalOffset)
        : RoutedEventArg(NULL)
    {
        _horizontalOffset = horizontalOffset; 
        _verticalOffset = verticalOffset;
    } 

    Float HorizontalOffset() const
    { 
        return _horizontalOffset;
    } 

    Float VerticalOffset() const
    {
        return _verticalOffset;
    }

    void CallEventHandler(Object* handler, Object* target)
    {
        InternalCall<Element, RoutedEventHandler, DragStartedEventArg>(handler, target);
    }

protected:

    Float _horizontalOffset; 
    Float _verticalOffset;
};

class DragDeltaEventArg : public RoutedEventArg
{
public:

    DragDeltaEventArg(Float horiChange, Float vertChange, Float horiDelta, Float vertDelta)
        : RoutedEventArg(NULL)
    {
        _horiChange = horiChange;
        _vertChange = vertChange;
        _horiDelta = horiDelta;
        _vertDelta = vertDelta;
    } 

    Float GetHorizontalChange() const
    { 
        return _horiChange;
    } 

    Float GetVerticalChange() const
    {
        return _vertChange;
    }

    Float GetHorizontalDelta() const
    { 
        return _horiDelta;
    } 

    Float GetVerticalDelta() const
    {
        return _vertDelta;
    }

    void CallEventHandler(Object* handler, Object* target)
    {
        InternalCall<Element, RoutedEventHandler, DragDeltaEventArg>(handler, target);
    }

protected:

    Float _horiChange;
    Float _vertChange;
    Float _horiDelta;
    Float _vertDelta; 
};

class DragCompletedEventArg : public RoutedEventArg
{ 
public:

    DragCompletedEventArg(Float horizontalChange, Float verticalChange, bool canceled)
        : RoutedEventArg(NULL)
    {
        _horizontalChange = horizontalChange;
        _verticalChange = verticalChange; 
        _wasCanceled = canceled;
    } 

    Float HorizontalChange() const 
    {
        return _horizontalChange;
    } 

    Float VerticalChange() const
    {
        return _verticalChange;
    } 

    bool Canceled() const
    {
        return _wasCanceled;
    }

    void CallEventHandler(Object* handler, Object* target)
    {
        InternalCall<Element, RoutedEventHandler, DragCompletedEventArg>(handler, target);
    }

protected:

    Float _horizontalChange; 
    Float _verticalChange; 
    bool _wasCanceled;
};

}

#endif
