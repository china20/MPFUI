// 华勤科技版权所有 2010-2011
// 
// 文件名：uiinvokerpriorityqueue.h
// 功  能：实现带有优先级的队列。
// 
// 作  者：MPF开发组
// 时  间：2010-07-02
// 
// ============================================================================

#ifndef _UIINVOKERPRIORITYQUEUE_H_
#define _UIINVOKERPRIORITYQUEUE_H_

#include <System/Windows/AssignerInvoker.h>
#include <System/Types/Structure.h>
#include <System/Tools/Locker.h>

namespace suic
{

typedef Array<AssignerInvoker*> AssignerInvokerArray;

class SUICORE_API InvokerPriorityQueue  : public Object
{
public:

    InvokerPriorityQueue();
    ~InvokerPriorityQueue();

    // 取出有数据的最大优先级
    int GetMaxPriority();

    // 弹出优先级最高的操作项
    AssignerInvoker* Dequeue();
    AssignerInvoker* GetSenderItem();

    void Enqueue(AssignerInvoker*, int Priority);
    void RemoveItem(AssignerInvoker* item);

protected:

    void CalculateMaxPriority();

protected:

    Array<AssignerInvoker*> _sendQueue;
    Array<AssignerInvokerArray*> _queue;

    int _maxPriority;
    Mutex _queueLock;
};

}

#endif
