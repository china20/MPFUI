// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����uieventarg.h
// ��  �ܣ�������Ŀ���¼����ࡣ
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2010-07-02
// 
// ============================================================================

#ifndef _UIEVENTARG_H_
#define _UIEVENTARG_H_

#include <System/Windows/Object.h>
#include <System/Windows/Delegate.h>

namespace suic
{

/// <summary>
/// ����Ԫ�ص��¼����������н���Ԫ�صĻ��࣬�����Ľ���Ԫ�ض���ֱ�ӻ�
/// ���ӵ�������
/// </summary>
class SUICORE_API EventArg
{
public:

    static EventArg Empty;

    EventArg();
    Uint32 Timestamp();

protected:

    Uint32 _timestamp;
};

class SUICORE_API AccessKeyEventArg : public EventArg
{
public:

    AccessKeyEventArg(Uint16 modifier, const String& key, bool isMultiple);

    String GetKey() const;
    bool IsMultiple() const;
    Uint16 GetModifier() const;

private:

    String _key;
    Uint16 _modifier;
    bool _isMultiple;
};

};

#endif
