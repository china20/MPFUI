// 华勤科技版权所有 2008-2022
// 
// 文件名：CommandSource.h
// 功  能：实现默认的命令接口
// 
// 作  者：MPF开发组
// 时  间：2010-08-31
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
