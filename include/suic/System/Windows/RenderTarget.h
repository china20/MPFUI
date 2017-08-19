// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����uirendertarget.h
// ��  �ܣ����Ӹ�Ԫ�ص������࣬�ڴ�Ԫ������ʾ��Ԫ���൱�����ⴰ��
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2011-12-25
// 
// ============================================================================

#ifndef _UIRENDERTARGET_H_
#define _UIRENDERTARGET_H_

#include <System/Windows/FrameworkElement.h>
#include <System/Windows/HwndAdapter.h>

namespace suic
{

class RenderInfo;

/// <summary>
/// VisualHost������������Ϣ
/// </summary>
class SUICORE_API VisualHost : public Object
{
public:

    RTTIOfClass(VisualHost)

    VisualHost();
    virtual ~VisualHost();

    static DpProperty* RootSourceProperty;

    /// <summary>
    ///  ��ȡԪ�ص�Ŀ������
    /// </summary>
    /// <remarks>
    ///  Ŀ����������ӵ����ʵ���壬����Ⱦ���ݵ���Ļ��������Ϣ�ȡ�
    /// </remarks>
    /// <param name="elem">Ŀ��Ԫ��</param>
    /// <returns>Ŀ������</returns>
    static VisualHost* GetVisualHost(Element* elem);
    static FrameworkElement* FromHwnd(Handle h);

    virtual void SetValue(const String&, Object* val);
    virtual Object* GetValue(const String&);

    virtual Handle GetHandle();
    void SetHandle(Handle h);

    void SetNeedRender(bool val);
    bool IsNeedRender();

    void SetRootElement(FrameworkElement* host);
    virtual FrameworkElement* GetRootElement();

    /// <summary>
    ///     ���úͻ�ȡ����������ƫ��
    /// </summary>
    /// <remarks>
    ///     �������ɸ�Ԫ�ص������Ϊ��������Ԫ���ڻ����е�
    ///     λ����SetOffset���ã���С�ο�SetSize�������塣
    /// </remarks>
    /// <param name="offset">
    ///     ��Ԫ��ƫ��
    /// </param>
    /// <returns>��</returns>
    void SetOffset(Point offset);
    Point GetOffset();

    /// <summary>
    ///     ��ȡ��Ԫ������Ļ�е�λ��
    /// </summary>
    /// <remarks>
    ///     ��Ԫ������Ļ�е�λ���ɻ�������Ļλ��ƫ��
    ///     ��SetOffset���õ�ֵ��
    /// </remarks>
    /// <returns>��Ļλ��ƫ��</returns>
    virtual Point GetScreenPoint();

    void SetSize(Size size);
    Size GetSize();

    virtual void UpdateLayout();
    virtual void Invalidate(const Rect* lprc, bool bForce);

    virtual void SetMouseCapture();
    virtual void ReleaseMouseCapture();

    //-----------------------------------------------

    /// <summary>
    ///     ��Ⱦ����Ԫ��
    /// </summary>
    /// <remarks>
    ///     �˷���ˢ��ָ������Ľ���Ԫ�أ����lprcΪnull��
    ///     ��ˢ����������Ԫ�ء�
    /// </remarks>
    /// <param name="h">
    ///     ���ھ��
    /// </param>
    /// <param name="lprc">
    ///     ��Ч����
    /// </param>
    /// <returns>��</returns>
    virtual void OnRender(Handle h, const Rect* lprc);
    virtual void OnRender(const Rect* lprc);

    RenderInfo* GetRenderInfo();

public:

    virtual bool HandleEvent(MessageParam* mp);
    virtual bool HandleEvent(Handle h, Uint32 message, Uint32 wp, Uint32 lp, Uint32& result);

private:

    Handle _handle;
    Size _size;

    Point _offset;
    Point _screenOffset;

    RenderInfo* _renderInfo;
    FrameworkElement* _rootElement;
};

typedef shared<VisualHost> VisualHostPtr;

};

#endif
