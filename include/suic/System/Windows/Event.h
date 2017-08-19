// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����UIEvent.h
// ��  �ܣ�������Ŀ���¼����ࡣ
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2010-07-02
// 
// ============================================================================

#ifndef _UIEVENT_H_
#define _UIEVENT_H_

#include <System/Windows/Object.h>
#include <System/Windows/Delegate.h>
#include <System/Windows/EventArg.h>

namespace suic
{

class SUICORE_API CancelEventArg  : public EventArg
{
public:

    CancelEventArg(bool cancel);

    bool IsCancel();
    void SetCancel(bool cancel);

protected:

    bool _cancel;
};

};

#endif
