// ======================================================================
//
// Copyright (c) 2008-2022 汪荣(视点UI), Inc. All rights reserved.
//
// MPF界面库遵循指定开源协议，团体或公司商用请根据协议购买授权，
// 任何个人、团体或公司不能居于此库衍生任何商业性质的库或代码。
//
// ======================================================================

/////////////////////////////////////////////////////////////////////////
// RoutedCommand.cpp

#include <System/Windows/Element.h>
#include <System/Windows/Delegate.h>
#include <System/Windows/RoutedCommand.h>

#include <System/Tools/CommandOp.h>

namespace suic
{

//ImplementRTTIOfClass(SystemCommand, Object)
//Array<SystemCommand*> SystemCommand::_systemCmds;

RoutedCommand::RoutedCommand(const String& name, RTTIOfInfo* ownerType)
{
    _name = name;
    _ownerType = ownerType;
}

RTTIOfInfo* RoutedCommand::GetOwnerType()
{
    return _ownerType;
}

String RoutedCommand::GetName()
{
    return _name;
}

bool RoutedCommand::CanExecute(Object* target, Object* parameter)
{
    if (NULL == target)
    {
        return false;
    }
    else
    {
        return true;
    }
}

void RoutedCommand::Execute(Object* target, Object* params)
{
    ElementPtr source(target);

    if (target)
    {
        ExecutedRoutedEventArg e(source.get(), params, this);

        e.SetRoutedEvent(Element::PreExecutedEvent);
        source->RaiseEvent(&e);

        e.SetRoutedEvent(Element::ExecutedEvent);
        source->RaiseEvent(&e);
    }
}

// ==============================================================
//
//SystemCommand::SystemCommand()
//    : RoutedCommand(L"", NULL)
//{
//
//}
//
//SystemCommand::SystemCommand(const String& name, RTTIOfInfo* handlerType)
//    : RoutedCommand(name, handlerType)
//{
//
//}
//
//bool SystemCommand::StaticInit()
//{
//    if (_systemCmds.GetCount() == 0)
//    {
//        _systemCmds.Add(new SystemCommand(L"Cut", RTTIType()));
//        _systemCmds.Add(new SystemCommand(L"Copy", RTTIType()));
//        _systemCmds.Add(new SystemCommand(L"Paste", RTTIType()));
//        _systemCmds.Add(new SystemCommand(L"SelectAll", RTTIType()));
//    }
//
//    return true;
//}
//
//SystemCommand* SystemCommand::GetCommand(const String& strCmd)
//{
//    for (int i = 0; i < _systemCmds.GetCount(); ++i)
//    {
//        if (_systemCmds[i]->GetName().Equals(strCmd))
//        {
//            return _systemCmds[i];
//        }
//    }
//    
//    return NULL;
//}

// ============================================================
//

CommandBinding::CommandBinding(RoutedCommand* command, ExecutedRoutedEventHandler executed)
    : _command(command)
    , _executed(executed)
{
    ;
}

RoutedCommand* CommandBinding::GetCommand()
{
    return _command;
}

ExecutedRoutedEventHandler& CommandBinding::GetCommandExecutedHandler()
{
    return _executed;
}

ExecutedRoutedEventHandler& CommandBinding::GetCommandPreExecutedHandler()
{
    return _preexecuted;
}

}
