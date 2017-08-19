// 华勤科技版权所有 2010-2011
// 
// 文件名：Image.h
// 功  能：实现图像绘制。
// 
// 作  者：MPF开发组
// 时  间：2010-05-02
// 
// ============================================================================

#ifndef _UIIMAGE_H_
#define _UIIMAGE_H_

#include <System/Windows/FrameworkElement.h>

namespace suic
{

/// <summary>
/// 图像控件
/// </summary>
class SUICORE_API Image : public FrameworkElement
{
public:

    static DpProperty* SourceProperty;
    static DpProperty* StretchProperty;
    static DpProperty* ZoomRatioProperty;
    static DpProperty* EnableGreyProperty;
    static DpProperty* StretchDirectionProperty;

    static bool StaticInit();

    static void OnSourcePropChanged(DpObject* d, DpPropChangedEventArg* e);

    Image();
    virtual ~Image();

    RTTIOfClass(Image)

    ImageSource* GetSource();
    void SetSource(ImageSource* val);

    int GetStretch();
    void SetStretch(int val);

    Float GetZoomRatio();
    void SetZoomRatio(Float val);

    int GetStretchDirection();
    void SetStretchDirection(int val);

protected:

    void OnRender(Drawing * drawing);

    Size OnMeasure(const Size& constraint);
    void OnArrange(const suic::Size& arrangeSize);

private:

    Size InternalMeasureArrange(Size size);
};

inline int Image::GetStretch()
{
    return GetValue(StretchProperty)->ToInt();
}

inline int Image::GetStretchDirection()
{
    return GetValue(StretchDirectionProperty)->ToInt();
}

inline void Image::SetStretch(int val)
{
    Integer* intVal = StretchBox::From(val);
    if (intVal)
    {
        SetValue(StretchProperty, intVal);
    }
}

inline void Image::SetStretchDirection(int val)
{
    Integer* intVal = StretchDirectionBox::From(val);
    if (intVal)
    {
        SetValue(StretchDirectionProperty, intVal);
    }
}

inline Float Image::GetZoomRatio()
{
    return GetValue(ZoomRatioProperty)->ToFloat();
}

inline void Image::SetZoomRatio(Float val)
{
    SetValue(ZoomRatioProperty, new OFloat(val));
}

}

#endif
