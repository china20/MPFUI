// 华勤科技版权所有 2010-2011
// 
// 文件名：uiroutedcommand.h
// 功  能：定义核心库的路由命令基类。
// 
// 作  者：MPF开发组
// 时  间：2010-07-02
// 
// ============================================================================

#ifndef _UIROUTEDCOMMAND_H_
#define _UIROUTEDCOMMAND_H_

#include <System/Windows/Delegate.h>
#include <System/Windows/EventHandler.h>
#include <System/Windows/ICommandSource.h>

#include <System/Tools/Array.h>

namespace suic
{

class SUICORE_API RoutedCommand : public ICommand
{
public:

    RoutedCommand(const String& name, RTTIOfInfo* handlerType);
    RTTIOfInfo* GetOwnerType();
    String GetName();

    bool CanExecute(Object* target, Object* parameter);

    /// <summary>
    ///  执行注册的命令
    /// </summary>
    /// <returns>无</returns>
    void Execute(Object* target, Object* params);

protected:

    String _name;
    RTTIOfInfo* _ownerType;
};

class SUICORE_API CommandBinding : public Object
{
public:

    CommandBinding(RoutedCommand* command, ExecutedRoutedEventHandler executed);

    RoutedCommand* GetCommand();
    ExecutedRoutedEventHandler& GetCommandExecutedHandler();
    ExecutedRoutedEventHandler& GetCommandPreExecutedHandler();

protected:

    RoutedCommand* _command;
    ExecutedRoutedEventHandler _executed;
    ExecutedRoutedEventHandler _preexecuted;
};

}

#endif
