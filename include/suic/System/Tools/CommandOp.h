// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����CommandOp.h>
// ��  �ܣ����Ŀ��������ࡣ
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2011-07-02
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
