// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����HwndHost.h
// ��  �ܣ�ʵ����ʵ����Ƕ��
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2012-08-31
// 
// ============================================================================

#ifndef _UIHWNDHOST_H_
#define _UIHWNDHOST_H_

#include <System/Windows/FrameworkElement.h>

namespace suic
{

/// <summary>
/// ������ʵ���ھ����������ʱ�Ŵ����С���ڡ�
/// </summary>
class SUICORE_API HwndHost : public FrameworkElement
{
public:

    HwndHost();
    virtual ~HwndHost();

    RTTIOfClass(HwndHost)

    Handle GetHandle();
    void Sethandle(Handle h);

protected:

    void OnInitialized(EventArg* e);

    void OnLoaded(LoadedEventArg* e);
    void OnUnloaded(LoadedEventArg* e);

    void OnRender(Drawing * drawing);
    void OnRenderSizeChanged(SizeChangedInfo& e);

protected:

    void BuildWindow(Handle hwndParent);
    void UpdateWindowPos();
    void OnWindowPositionChanged(Rect rcBox);
    void OnIsVisibleChanged(IsVisibleChangedEventArg* e);

protected:

    // ��ʵ���ھ��
    Handle _handle;
    // �Ƿ�Ϊ�ڲ��������
    bool _selfHost;
};

inline Handle HwndHost::GetHandle()
{
    return _handle;
}

};

#endif
