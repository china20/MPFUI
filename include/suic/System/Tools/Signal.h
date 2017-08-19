// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����uisignal.h>
// ��  �ܣ��ź�����
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2010-12-12
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
    ///     �ȴ��ڲ����ⲿ�¼���
    /// </summary>
    /// <param name="_Other">�ⲿ�¼�</param>
    /// <param name="iTimeout">��ʱʱ��</param>
    /// <returns>��������¼�����,����1;����ⲿ�¼�����,����2;��ʱ����3;����-1</returns>
    int Wait(UISignal& _Other, int iTimeout=INFINITE);

	void Notify();

	void Reset();

private:

	Handle _signal;
};

}

#endif