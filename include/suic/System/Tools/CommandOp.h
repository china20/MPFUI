// 华勤科技版权所有 2010-2011
// 
// 文件名：CommandOp.h>
// 功  能：核心框架命令工具类。
// 
// 作  者：MPF开发组
// 时  间：2011-07-02
// 
// ============================================================================

#ifndef _UICOMMANDOP_H_
#define _UICOMMANDOP_H_

#include <System/Windows/Object.h>
#include <System/Windows/Element.h>
#include <System/Windows/RoutedCommand.h>

namespace suic
{

class SUICORE_API CommandOp
{
public:

    static void RegisterClassCommandHandler(RTTIOfInfo* type
        , RoutedCommand* command, ExecutedRoutedEventHandler handler);
    static void ExecuteCommand(ICommandSource* commandSource);

protected:

    CommandOp() {}
};

};

#endif
