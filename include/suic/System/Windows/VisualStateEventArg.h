// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����uivisualstateeventarg.h
// ��  �ܣ�������Ŀ��״̬�¼����ࡣ
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2010-07-02
// 
// ============================================================================

#ifndef _UIVISUALSTATEEVENTARG_H_
#define _UIVISUALSTATEEVENTARG_H_

#include <System/Windows/VisualStates.h>
#include <System/Windows/EventArg.h>
#include <suic/framework/uiframeview.h>

namespace suic
{

/// <summary>
/// ����Ԫ�ص��¼����������н���Ԫ�صĻ��࣬�����Ľ���Ԫ�ض���ֱ�ӻ�
/// ���ӵ�������
/// </summary>
class SUICORE_API ViewStateEventArg : public EventArg
{
public:

    ViewStateEventArg();

protected:

    VisualState _oldState;
    VisualState _newState; 
    FrameworkElementPtr _control; 
};

}

#endif
