
#include <System/Windows/CommandSource.h>
#include <System/Windows/FrameworkElement.h>

namespace suic
{

CommandSource::CommandSource()
{
    _command = NULL;
    _commandTarget = NULL;
    _commandParameter = NULL;
}

CommandSource::~CommandSource()
{
    if (_commandParameter)
    {
        _commandParameter->unref();
    }
}

ICommand* CommandSource::GetCommand()
{
    return _command;
}

Object* CommandSource::GetCommandParameter()
{
    return _commandParameter;
}

Object* CommandSource::GetCommandTarget()
{
    return _commandTarget;
}

void CommandSource::SetCommand(ICommand* command)
{
    _command = command;
}

void CommandSource::SetCommandTarget(Object* target)
{
    _commandTarget = target;
}

void CommandSource::SetCommandParameter(Object* parameter)
{
    if (_commandParameter)
    {
        _commandParameter->unref();
    }
    _commandParameter = parameter;
    if (_commandParameter)
    {
        _commandParameter->ref();
    }
}

}
