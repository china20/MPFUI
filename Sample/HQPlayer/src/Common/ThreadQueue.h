// 华勤科技版权所有 2010-2021
// 
// 文件名：ThreadQueue.h
// 功  能：线程安全队列。
// 
// 作  者：MPF开发组
// 时  间：2017-01-02
// 
// ============================================================================

#ifndef _THREADQUEUE_H_
#define _THREADQUEUE_H_

#include "ThreadCond.h"

#include <list>
#include <vector>

namespace core
{

class MulSync
{
public:
    typedef suic::Mutex Mutex;
    typedef ThreadCond Condition;
};

template <typename Element>
class NormalContainer
{
public:

	typedef Element _MyElement;

	NormalContainer() 
        : maxCount_(100), nLocCount_(0)
	{
	}

	NormalContainer(int nMax) 
        : maxCount_(nMax), nLocCount_(0)
	{
	}

    void SetMaxCount(int nMax)
	{
		if (nMax > 0) 
        {
            maxCount_ = nMax;
        }
	}

	size_t Size()
	{
		return eleContainer_.size();
	}

	int Pop(Element& ele)
	{
		if (eleContainer_.size() == 0) 
        {
            return 0;
        }

		ele = eleContainer_.front();
		eleContainer_.pop_front();

		return 1;
	}

    int Get(std::vector<Element>& eleArr, int nCount)
	{
		if (eleContainer_.size() == 0) 
        {
            return 0;
        }

		int nReal = min(nCount, (int)eleContainer_.size());
		list<Element>::iterator iter = eleContainer_.begin();
		int iNum = 0;
		for (;;)
		{
			eleArr.push_back(*iter);
			if ((++iNum) >= nReal)
			{
				eleContainer_.erase(eleContainer_.begin(), ++iter);
				break;
			}
			++iter;
		}
		return nReal;
	}

	void Push(Element ele)
	{
		Add(ele);
	}

	void Add(Element& ele)
	{
		eleContainer_.push_back(ele);
	}

	void AddFront(Element& ele)
	{
		eleContainer_.push_front(ele);
	}

	int PopLock(Element& ele)
	{
		return (nLocCount_=Pop(ele));
	}

    int GetLock(std::vector<Element>& eleArr, int nCount)
	{
		return (nLocCount_ = Get(eleArr, nCount));
	}

	void Unlock(int iCount)
	{
		nLocCount_ -= iCount;
	}

	bool IsEmpty()
	{
		return IsEmpty_i();
	}

	bool IsFull()
	{
		return IsFull_i();
	}

	void Clear()
	{
		eleContainer_.clear();
	}

protected:
	bool IsEmpty_i()
	{
		if (eleContainer_.size() == 0)
		{
			return true;
		}
		return false;
	}

	bool IsFull_i()
	{
		if ((int)eleContainer_.size() >= (maxCount_-nLocCount_))
		{
			return true;
		}
		return false;
	}

private:

    std::list<Element>	eleContainer_;
	int				maxCount_;
	int				nLocCount_;
};

template <typename Element> 
class NormalQueue
{
public:

	NormalQueue(int nMax)
		: m_maxCount(nMax)
	{
	}

	NormalQueue()
		: m_maxCount(50)
	{
	}

	~NormalQueue()
	{
	}

	void SetMaxCount(int nMax)
	{
        suic::Mutex::Locker guard(moni_lock);

		if (nMax > 0) 
        {
            m_maxCount = nMax;
        }
	}

	int GetMaxCount()
	{
		suic::Mutex::Locker guard(moni_lock);

		return m_maxCount;
	}

	int Size()
	{
		suic::Mutex::Locker guard(moni_lock);

		return (int)m_queue.size();
	}

	bool Add(Element ele)
	{
		suic::Mutex::Locker guard(moni_lock);

		// queue is full
		if ((int)m_queue.size() >= m_maxCount) 
        {
            return false;
        }

		m_queue.push_back(ele);
		return true;
	}

	bool Pop(Element& ele)
	{
		suic::Mutex::Locker guard(moni_lock);
		if (m_queue.size() == 0) 
        {
            return false;
        }

		ele = m_queue.front();
		m_queue.pop_front();

		return true;
	}

	bool IsEmpty()
	{
		suic::Mutex::Locker guard(moni_lock);

		if (m_queue.size()  == 0) 
        {
            return true;
        }
        else
        {
            return false;
        }
	}

	bool IsFull()
	{
		suic::Mutex::Locker guard(moni_lock);
		if ((int)m_queue.size() >= m_maxCount) 
        {
            return true;
        }
		else
        {
            return false;
        }
	}

    suic::Mutex& GetLock()
    {
        return moni_lock;
    }

    bool PopItem(Element& ele)
	{
		if (m_queue.size() == 0) 
        {
            return false;
        }

		ele = m_queue.front();
		m_queue.pop_front();

		return true;
	}

    void Clear()
    {
        m_queue.clear();
    }

private:

    suic::Mutex		moni_lock;
    std::list<Element>	m_queue;
	int				m_maxCount;
};

// =================================================================
// implement a monitor queue, if the queue is empty, calling thread will be 
// waiting until some data put in queue, if the queue is full, pushing thread 
// will be waiting too until some threads get data from queue, so there are some 
// spaces in the queue, if going to exit, please call exit to allow waiting threads 
// have change to exit waiting state. then if want to reuse the queue, please call 
// Reset to reuse it. note: if call Reset,  data in queue will be cleared and some threads
// will be waiting again.
// =================================================================

template <typename MyQueue, typename SyncMode> 
class MonitorQueue : public MyQueue
{
public:

	typedef typename MyQueue::_MyElement Element;

    MonitorQueue()
    {
        this->MonitorQueue::MonitorQueue(1);
    }

    MonitorQueue(int nMax)
        : MyQueue(nMax)
        , isLockQueue(false)
        , bIsExit(false)
    {
    }

    ~MonitorQueue()
    {
    }

    void SetMaxCount(int nMax)
	{
        SyncMode::Mutex::Locker guard(moni_lock);

        MyQueue::SetMaxCount(nMax);
	}

    size_t Size()
    {
        // SyncMode::Mutex::Locker guard(moni_lock);

        return MyQueue::Size();
    }

    int Add(Element& ele, int iTimeout=INFINITE)
    {
        SyncMode::Mutex::Locker guard(moni_lock);

        if (!WaitEmpty(iTimeout)) 
        {
            return -1;
        }

        MyQueue::Add(ele);
        not_empty.NotifyAll();

        return 1;
    }

    int Push(Element ele, int iTimeout=INFINITE)
    {
        return Add(ele, iTimeout);
    }

    int Pop(Element& ele, int iTimeout=INFINITE)
    {
        SyncMode::Mutex::Locker guard(moni_lock);

        if (!WaitFull(iTimeout, isLockQueue)) 
        {
            return -1;
        }

        MyQueue::Pop(ele);
        not_full.NotifyAll();

        return 1;
    }

    int Get(std::vector<Element>& eleArr, int nCount, int iTimeout=INFINITE)
    {
        SyncMode::Mutex::Locker guard(moni_lock);

        if (!WaitFull(iTimeout, isLockQueue)) 
        {
            return -1;
        }

        int nReal = MyQueue::Get(eleArr, nCount);
        not_full.NotifyAll();

        return nReal;
    }

    void Reset()
    {
        SyncMode::Mutex::Locker guard(moni_lock);

        MyQueue::Clear();

        not_full.NotifyAll();
        not_empty.NotifyAll();
    }

    bool IsEmpty()
    {
        SyncMode::Mutex::Locker guard(moni_lock);

        return MyQueue::IsEmpty();
    }

    bool IsFull()
    {
        SyncMode::Mutex::Locker guard(moni_lock);

        return MyQueue::IsFull();
    }

    bool IsExit() const
    {
        SyncMode::Mutex::Locker guard(state_lock);
        return bIsExit;
    }

    void SetExit(bool bExit)
    {
        SyncMode::Mutex::Locker guard(state_lock);
        bIsExit = bExit;
    }

protected:

    bool WaitEmpty(int iTimeout)
    {
        if (IsExit())
        {
            return false;
        }

        while (MyQueue::IsFull())
        {
            // timeout
            if (!not_full.TimedWait(moni_lock, iTimeout)) 
            {
                return false;
            }
        }

        return true;
    }

    bool WaitFull(int iTimeout, bool bLock)
    {
        if (IsExit())
        {
            return false;
        }

        while (MyQueue::IsEmpty() || bLock)
        {
            // timeout
            if (!not_empty.TimedWait(moni_lock, iTimeout)) 
            {
                return false;
            }
        }

        return true;
    }

protected:

    typename SyncMode::Mutex moni_lock;
    typename SyncMode::Mutex state_lock;

    typename SyncMode::Condition not_empty;
    typename SyncMode::Condition not_full;
    bool isLockQueue;

    // 强行退出标识
    bool bIsExit;
};

template <typename MyQueue, typename SyncMode> 
class MonitorQueueLock : public MonitorQueue<MyQueue, SyncMode>
{
public:

    MonitorQueueLock()
    {
    }

    MonitorQueueLock(int nMax)
        : MonitorQueue(nMax)
    {
    }

    int PopLock(Element& ele, int iTimeout=INFINITE)
    {
        SyncMode::Mutex::Locker guard(moni_lock);

        if (!WaitFull(iTimeout, isLockQueue)) 
        {
            return -1;
        }

        MyQueue::PopLock(ele);
        isLockQueue = true;

        return 1;
    }

    int AddLock(Element& ele, int iTimeout=INFINITE)
    {
        SyncMode::Mutex::Locker guard(moni_lock);

        MyQueue::Unlock(1);

        if (!WaitEmpty(iTimeout)) 
        {
            return -1;
        }

        MyQueue::AddFront(ele);
        Unlock_i(0);

        return 1;
    }

    int AddLock(std::vector<Element>& eleArr, int iTimeout=INFINITE)
    {
        SyncMode::Mutex::Locker guard(moni_lock);
        MyQueue::Unlock((int)eleArr.size());
        if (!WaitEmpty(iTimeout)) 
        {
            return -1;
        }

        for (int i=(int)eleArr.size()-1;i>=0;--i)
        {
            MyQueue::AddFront(eleArr[i]);
        }

        Unlock_i(0);

        return 1;
    }

    int GetLock(std::vector<Element>& eleArr, int nCount, int iTimeout=INFINITE)
    {
        SyncMode::Mutex::Locker guard(moni_lock);

        if (!WaitFull(iTimeout, isLockQueue)) 
        {
            return -1;
        }

        int nReal = MyQueue::GetLock(eleArr, nCount);
        isLockQueue = true;
        not_full.NotifyAll();

        return nReal;
    }

    void Unlock(int iCount)
    {
        SyncMode::Mutex::Locker guard(moni_lock);
        Unlock_i(iCount);
    }

protected:

    void Unlock_i(int iCount)
    {
        MyQueue::Unlock(iCount);
        isLockQueue = false;
        not_empty.NotifyAll();
        not_full.NotifyAll();
    }
};

}

#endif
