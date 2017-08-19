// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����ScrollContentPresenter.h
// ��  �ܣ�������ͼ��Ⱦ
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2010-08-31
// 
// =============================================================

#ifndef _UISCROLLCONTENTPRESENTER_H_
#define _UISCROLLCONTENTPRESENTER_H_

#include <System/Windows/ScrollInfo.h>
#include <Framework/Controls/Decorator.h>
#include <Framework/Presenter/ContentPresenter.h>

namespace suic
{

/// <summary>
/// ScrollContentPresenter����TemplateParentΪScrollViewer��
/// </summary>
class SUICORE_API ScrollContentPresenter : public ContentPresenter
{
public:

    RTTIOfClass(ScrollContentPresenter)

    ScrollContentPresenter();
    virtual ~ScrollContentPresenter();

    AdornerLayer* GetAdornerLayer() const;

    int GetVisualChildrenCount();
    Element* GetVisualChild(int index);

    /// <summary>
    ///  �ҽӹ��������ʵ��IScrollInfo�ӿڵĽ���Ԫ�أ�
    ///  ��ȡ��ӿڰ󶨵�ScrollViewer��ִ�й����¼�֪ͨ
    /// </summary>
    /// <param name="value">��</param>
    /// <returns>��</returns>
    void LinkScrollInfo();
    IScrollInfo* GetScrollInfo();

    bool CanContentScroll();

    static int CoerceOffset(int offset, int extent, int viewport);

protected:

    void OnApplyTemplate();

    Size OnMeasure(const Size& constraint);
    void OnArrange(const Size& arrangeSize);

    FrameworkElement* GetTemplateChild();
    void SetTemplateChild(FrameworkElement* elem);

private:

    ScrollData* GetScrollData();
    bool IsScrollClient() const;
    void EnsureScrollInfo();
    void VerifyScrollData(Size viewport, Size extent);

protected:

    // ������Ԫ�ر��
    // LinkScrollInfoû���ҵ��������
    // ʹ��������ΪĬ�Ϲ������
    bool _scrollClient;
    // ���_scrollClientΪtrue������LinkScrollInfo
    // �ҽӶ�������������ʵ��
    IScrollInfo* _scrollInfo;

    // װ������
    AdornerLayer* _adornerLayer;
};

inline AdornerLayer* ScrollContentPresenter::GetAdornerLayer() const
{
    return _adornerLayer;
}

inline bool ScrollContentPresenter::IsScrollClient() const
{
    return _scrollClient;
}

}

#endif
