// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����CommandSource.h
// ��  �ܣ�ʵ��Ĭ�ϵ�����ӿ�
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2010-08-31
// 
// =============================================================

#ifndef _COMMANDSOURCE_H_
#define _COMMANDSOURCE_H_

#include <System/Windows/ICommandSource.h>

namespace suic
{

class SUICORE_API CommandSource : public ICommandSource
{
public:

    CommandSource();
    virtual ~CommandSource();

    ICommand* GetCommand();
    Object* GetCommandParameter();
    Object* GetCommandTarget();

    void SetCommand(ICommand* command);
    void SetCommandTarget(Object* target);
    void SetCommandParameter(Object* parameter);

private:

    ICommand* _command;
    Object* _commandTarget;
    Object* _commandParameter;
};

}

#endif
