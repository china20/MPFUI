// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����ICommandSource.h
// ��  �ܣ�����ӿڶ���
// 
// ��  �ߣ�Sharpui������
// ʱ  �䣺2010-08-31
// 
// =============================================================

#ifndef _ICOMMANDSOURCE_H_
#define _ICOMMANDSOURCE_H_

#include <System/Windows/Object.h>
#include <System/Windows/EventHandler.h>

namespace suic
{

class SUICORE_API ICommand
{
public:
    
    EventHandler CanExecuteChanged;

    virtual ~ICommand() {}

    virtual bool CanExecute(Object* target, Object* parameter) = 0;
    virtual void Execute(Object* target, Object* parameter) = 0;
};

class SUICORE_API ICommandSource
{
public:

    virtual ~ICommandSource() {}

    virtual ICommand* GetCommand() = 0;
    virtual Object* GetCommandParameter() = 0;
    virtual Object* GetCommandTarget() = 0;

    virtual void SetCommand(ICommand* command) = 0;
    virtual void SetCommandTarget(Object* target) = 0;
    virtual void SetCommandParameter(Object* parameter) = 0;
};

/// <summary>
/// ����ִ���¼�������
/// </summary>
class SUICORE_API ExecutedRoutedEventArg : public RoutedEventArg
{
public:

    ExecutedRoutedEventArg(DpObject* sender, Object* params)
        : RoutedEventArg(sender)
        , _params(params)
        , _command(NULL)
    {
    }

    ExecutedRoutedEventArg(DpObject* sender, Object* params, ICommand* command)
        : RoutedEventArg(sender)
        , _params(params)
        , _command(command)
    {
    }

    Object* GetParams();
    ICommand* GetCommand();
    void SetCommand(ICommand* value);

    void CallEventHandler(Object* handler, Object* target);

protected:

    Object* _params;
    ICommand* _command;
};

inline Object* ExecutedRoutedEventArg::GetParams()
{
    return _params;
}

inline ICommand* ExecutedRoutedEventArg::GetCommand()
{
    return _command;
}

inline void ExecutedRoutedEventArg::SetCommand(ICommand* value)
{
    _command = value;
}

typedef delegate<void (Object*, ExecutedRoutedEventArg*)> ExecutedRoutedEventHandler;

}

#endif
