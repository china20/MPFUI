// 华勤科技版权所有 2010-2021
// 
// 文件名：ThreadSignal.h
// 功  能：线程安全队列。
// 
// 作  者：MPF开发组
// 时  间：2017-01-02
// 
// ============================================================================

#ifndef __XPSIGNAL_H
#define __XPSIGNAL_H

namespace core
{

class Signal
{
public:

	Signal(bool bSignal=true);
	Signal(bool bMenuReset,bool bSignal);

	~Signal();

	operator HANDLE() const;

	bool Wait(int iTimeout=INFINITE);

    /**
    * 功  能：等待内部和外部事件。
    * 
    * 参  数：
    *         _Other - 外部事件
    *         iTimeout - 超时时间
    *	
    * 返回值：如果本身事件返回,返回1;如果外部事件返回,返回2;超时返回3;错误-1
    * 异  常：无
    */
    int Wait(core::Signal& _Other, int iTimeout=INFINITE);

	void Notify();

	void Reset();

private:

	HANDLE signal_cond;
};

}

#endif
