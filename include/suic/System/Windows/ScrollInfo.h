// 华勤科技版权所有 2010-2011
// 
// 文件名：IScrollInfo.h
// 功  能：定义支持滚动接口，由Panel的子类实现支持滚动
// 
// 作  者：MPF开发组
// 时  间：2010-08-31
// 
// =============================================================

#ifndef _UISCROLLINFO_H_
#define _UISCROLLINFO_H_

#include <System/Windows/IScrollInfo.h>

namespace suic
{

struct SUICORE_API ScrollData
{
    bool allowHorizontal;
    bool allowVertical;
    Size extent;
    Size viewport;
    Point offset;
    Point computedOffset;

    ScrollViewer* scrollOwner;

    ScrollData()
    {
        allowHorizontal = true;
        allowVertical = true;
        scrollOwner = NULL;
    }

    void SetComputedOffset(const Point& offset);
};

class FrameworkElement;

typedef delegate<Rect (Element*, Rect)> MakeVisibleDelegate;

class SUICORE_API ScrollInfo : public IScrollInfo
{
public:

    MakeVisibleDelegate MakeVisibleCb;

    ScrollInfo();
    virtual ~ScrollInfo();

    virtual void LineDown();
    virtual void LineLeft();
    virtual void LineRight();
    virtual void LineUp();
    virtual void MouseWheelDown();
    virtual void MouseWheelLeft();
    virtual void MouseWheelRight();
    virtual void MouseWheelUp();
    virtual void PageDown();
    virtual void PageLeft();
    virtual void PageRight();
    virtual void PageUp();

    Rect MakeVisible(Element* visual, Rect rect);

    void SetOwner(FrameworkElement* owner);

    int GetComputeHorizontalOffset();
    int GetComputeVerticalOffset();

    int GetHorizontalOffset() const;
    int GetVerticalOffset() const;

    void SetHorizontalOffset(int offset);
    void SetVerticalOffset(int offset);

    bool CanHorizontalScroll() const;
    bool CanVerticalScroll() const;

    void SetCanHorizontalScroll(bool val);
    void SetCanVerticalScroll(bool val);

    void EnableHorizontalScroll(bool val);
    void EnableVerticalScroll(bool val);
    
    int GetExtentHeight() const;
    int GetExtentWidth() const;

    int GetViewportHeight() const;
    int GetViewportWidth() const;

    ScrollViewer* GetScrollOwner() const;
    void SetScrollOwner(ScrollViewer* val);

    ScrollData* GetScrollData();
    const ScrollData* GetScrollData() const;

protected:

    FrameworkElement* _owner;
    // 存储滚动信息
    ScrollData _scrollData;
};

}

#endif
