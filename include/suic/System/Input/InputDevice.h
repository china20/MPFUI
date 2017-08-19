// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����InputDevice.h
// ��  �ܣ��������ӿڶ��塣
// 
// ��  �ߣ�Sharpui������
// ʱ  �䣺2010-07-02
// 
// ============================================================================

#ifndef _UIINPUTDEVICE_H_
#define _UIINPUTDEVICE_H_

#include <System/Windows/RenderTarget.h>

namespace suic
{

class SUICORE_API InputDevice : public DpObject
{
public:

    virtual ~InputDevice() {}

    virtual VisualHost* GetVisualHost() = 0;
};

class SUICORE_API KeyboardDeice : public InputDevice
{
public:

    static KeyboardDeice* Current();

    VisualHost* GetVisualHost();
    void SetVisualHost(VisualHost* pHost);

private:

    KeyboardDeice();

private:

    VisualHost* _visualHost;
};

}

#endif
