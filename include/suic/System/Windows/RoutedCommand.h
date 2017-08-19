// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����uiroutedcommand.h
// ��  �ܣ�������Ŀ��·��������ࡣ
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2010-07-02
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
    ///  ִ��ע�������
    /// </summary>
    /// <returns>��</returns>
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
