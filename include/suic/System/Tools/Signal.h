// 华勤科技版权所有 2010-2011
// 
// 文件名：uisignal.h>
// 功  能：信号量。
// 
// 作  者：MPF开发组
// 时  间：2010-12-12
// 
// ============================================================

#ifndef __UISIGNAL_H_
#define __UISIGNAL_H_

#include <System/Windows/Object.h>

namespace suic
{

class SUICORE_API UISignal
{
public:

	UISignal(bool bSignal=true);
	UISignal(bool bMenuReset,bool bSignal);

	~UISignal();

	operator Handle() const;

	bool Wait(int iTimeout=INFINITE);

    /// <summary>
    ///     等待内部和外部事件。
    /// </summary>
    /// <param name="_Other">外部事件</param>
    /// <param name="iTimeout">超时时间</param>
    /// <returns>如果本身事件返回,返回1;如果外部事件返回,返回2;超时返回3;错误-1</returns>
    int Wait(UISignal& _Other, int iTimeout=INFINITE);

	void Notify();

	void Reset();

private:

	Handle _signal;
};

}

#endif