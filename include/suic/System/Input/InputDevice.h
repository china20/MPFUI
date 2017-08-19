// 华勤科技版权所有 2010-2011
// 
// 文件名：InputDevice.h
// 功  能：输入对象接口定义。
// 
// 作  者：Sharpui开发组
// 时  间：2010-07-02
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
