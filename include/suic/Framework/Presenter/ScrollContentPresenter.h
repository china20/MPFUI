// 华勤科技版权所有 2010-2011
// 
// 文件名：ScrollContentPresenter.h
// 功  能：滚动视图渲染
// 
// 作  者：MPF开发组
// 时  间：2010-08-31
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
/// ScrollContentPresenter，其TemplateParent为ScrollViewer。
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
    ///  挂接滚动组件（实现IScrollInfo接口的界面元素）
    ///  获取其接口绑定到ScrollViewer，执行滚动事件通知
    /// </summary>
    /// <param name="value">无</param>
    /// <returns>无</returns>
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

    // 滚动子元素标记
    // LinkScrollInfo没有找到滚动组件
    // 使用自身作为默认滚动组件
    bool _scrollClient;
    // 如果_scrollClient为true，则由LinkScrollInfo
    // 挂接而来，否则，自身实现
    IScrollInfo* _scrollInfo;

    // 装饰器层
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
