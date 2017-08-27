/////////////////////////////////////////////////////////////////////////
// uioperationpriorityqueue.cpp

#include <System/Windows/InvokerPriorityQueue.h>

namespace suic
{

InvokerPriorityQueue::InvokerPriorityQueue()
{
    _queue.Add(new AssignerInvokerArray());
    _queue.Add(new AssignerInvokerArray());
    _queue.Add(new AssignerInvokerArray());
    _queue.Add(new AssignerInvokerArray());
    _queue.Add(new AssignerInvokerArray());
    _queue.Add(new AssignerInvokerArray());

    _maxPriority = 5;
}

InvokerPriorityQueue::~InvokerPriorityQueue()
{
    for (int i = 0; i < 6; ++i)
    {
        AssignerInvokerArray* enumPtr(_queue[i]);
        for (int j = 0; j < enumPtr->GetCount(); ++j)
        {
            enumPtr->GetItem(j)->unref();
        }
        delete enumPtr;
    }
}

int InvokerPriorityQueue::GetMaxPriority()
{
    Mutex::Locker sync(_queueLock);

    if (_sendQueue.Length() > 0)
    {
        return eSend;
    }

    AssignerInvokerArray* maxPtr(_queue[_maxPriority]);

    if (maxPtr->GetCount() > 0)
    {
        return _maxPriority;
    }
    else
    {
        return eInvalid;
    }
}

void InvokerPriorityQueue::CalculateMaxPriority()
{
    for (int i = 0; i < 6; ++i)
    {
        AssignerInvokerArray* enumPtr(_queue[i]);

        if (enumPtr->GetCount() > 0)
        {
            _maxPriority = enumPtr->GetItem(0)->GetPriority();

            break;
        }
    }
}

AssignerInvoker* InvokerPriorityQueue::GetSenderItem()
{
    {
        Mutex::Locker sync(_queueLock);

        if (_sendQueue.Length() > 0)
        {
            AssignerInvoker* ret(_sendQueue[0]);

            _sendQueue.RemoveAt(0);

            return ret;
        }
        else
        {
            return NULL;
        }
    }
}

AssignerInvoker* InvokerPriorityQueue::Dequeue()
{
    {
        Mutex::Locker sync(_queueLock);

        if (_sendQueue.Length() > 0)
        {
            AssignerInvoker* ret(_sendQueue[0]);

            _sendQueue.RemoveAt(0);

            return ret;
        }
    }

    AssignerInvokerArray* maxPtr(_queue[_maxPriority]);

    if (maxPtr->GetCount() == 0)
    {
        Mutex::Locker sync(_queueLock);

        CalculateMaxPriority();

        maxPtr = _queue[_maxPriority];
    }

    if (maxPtr->GetCount() > 0)
    {
        Mutex::Locker sync(_queueLock);
        AssignerInvoker* ret(maxPtr->GetItem(0));

        maxPtr->RemoveAt(0);
        return ret;
    }
    else
    {
        return NULL;
    }
}

void InvokerPriorityQueue::Enqueue(AssignerInvoker* ptr, int iPriority)
{
    Mutex::Locker sync(_queueLock);

    if (iPriority < 0)
    {
        ptr->ref();
        _sendQueue.Add(ptr);
    }
    else
    {
        AssignerInvokerArray* enumPtr(_queue[iPriority]);

        ptr->ref();
        enumPtr->Add(ptr);
        CalculateMaxPriority();
    }
}

void InvokerPriorityQueue::RemoveItem(AssignerInvoker* item)
{
    Mutex::Locker sync(_queueLock);

    for (int i = 0; i < _sendQueue.Length(); ++i)
    {
        if (_sendQueue[i] == item)
        {
            _sendQueue.RemoveAt(i);
            item->unref();
            break;
        }
    }

    AssignerInvokerArray* enumPtr(_queue[item->GetPriority()]);

    for (int i = 0; i < enumPtr->Length(); ++i)
    {
        if (enumPtr->GetItem(i) == item)
        {
            enumPtr->RemoveAt(i);
            item->unref();
            break;
        }
    }
}

}
