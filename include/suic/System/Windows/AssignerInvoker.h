// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����uiassignerinvoker.h
// ��  �ܣ�������Ŀ���¼�����
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2010-07-02
// 
// ============================================================================

#ifndef _UIASSIGNERINVOKER_H_
#define _UIASSIGNERINVOKER_H_

#include <System/Windows/EventHandler.h>

namespace suic
{

enum InvokerPriority
{
    // ���ȼ����
    eSend = -1,
    // �������
    eRender = 0,
    eNormal = 1,
    eInvalid = 2,
    eInput = 3,
    eInactive = 4,
    eBackgound = 5,
};

enum AssignerInvokerStatus
{
    eCompleted = 1,
    eAborted = 2,
};

class Assigner;
typedef shared<Assigner> AssignerPtr;

class AssignerInvoker;
typedef shared<AssignerInvoker> AssignerInvokerPtr;

class SUICORE_API AssignerInvoker : public Object
{
public:

    EventHandler Aborted; 
    EventHandler Completed;

    AssignerInvoker();
    ~AssignerInvoker();

    Assigner* GetAssigner();
    InvokerPriority GetPriority();
    void SetPriority(InvokerPriority value);

    AssignerInvokerStatus GetStatus();
    
    bool Abort();
    AssignerInvokerStatus Wait(); 
    AssignerInvokerStatus Wait(Int32 timeout); 

    virtual void OnComplete();
    virtual void Invoke() = 0;

protected:

    Handle _signal;
    InvokerPriority _priority;
    AssignerInvokerStatus _status;

    friend class Assigner;
};

typedef delegate<void()> InvokerOne;

class SUICORE_API AssignerOneInvoker : public AssignerInvoker
{
public:

    AssignerOneInvoker(InvokerOne val)
    {
        _invoke = val;
    }

    void Invoke()
    {
        if (_invoke)
        {
            _invoke();
        }
    }

protected:

    InvokerOne _invoke;
};

};

#endif

