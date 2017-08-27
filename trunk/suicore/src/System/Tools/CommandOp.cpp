
#include <System/Tools/CommandOp.h>
#include <Framework/Controls/Control.h>
#include <System/Input/Keyboard.h>

#include <System/Windows/CommandManager.h>

namespace suic
{

void CommandOp::
RegisterClassCommandHandler(RTTIOfInfo* type, RoutedCommand* command
                            , ExecutedRoutedEventHandler handler)
{
    CommandManager::RegisterClassCommandHandler(type, command, handler);
}

void CommandOp::ExecuteCommand(ICommandSource* commandSource)
{
    if (commandSource != NULL)
    {
        Object* target = commandSource->GetCommandTarget();
        // 找到命令处理委托
        ICommand* cmd(commandSource->GetCommand());

        if (NULL == target)
        {
            target = Keyboard::GetFocusedElement();
        }

        Element* doTarget = RTTICast<Element>(target);

        if (cmd == NULL && NULL != doTarget)
        {
            cmd = DynamicCast<ICommand>(doTarget->GetDataContext());
        }

        if (cmd && cmd->CanExecute(target, commandSource->GetCommandParameter()))
        {
            cmd->Execute(target, commandSource->GetCommandParameter());
        }
    }
}

}
