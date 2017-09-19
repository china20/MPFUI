// 华勤科技版权所有 2008-2022
// 
// 文件名：Brush.h
// 功  能：定义刷子基类。
// 
// 作  者：MPF开发组
// 时  间：2010-07-02
// 
// ============================================================================

#ifndef _UIBRUSH_H_
#define _UIBRUSH_H_

#include <System/Types/XamlNode.h>
#include <System/Graphics/Resource.h>

namespace suic
{

class Drawing;

class SUICORE_API Brush : public Resource
{
public:

    enum eBrushIndex
    {
        biImage,
        biSolidBrush,
        biLinearBrush,
        biRadialBrush,
    };

    RTTIOfAbstractClass(Brush)

    Brush() : _opacity(1.0), _index(0) {}
    virtual ~Brush() {}

    Brush* Clone() const;

    int GetIndex();

    Float GetOpacity() const;
    void SetOpacity(Float val);

    virtual Color ToColor();
    virtual bool Parse(const String& xml);
    virtual bool Parse(IXamlNode* pNode);

protected:

    int _index;
    Float _opacity;
};

typedef shared<Brush> BrushPtr;

inline Float Brush::GetOpacity() const
{
    return _opacity;
}

inline void Brush::SetOpacity(Float val)
{
    _opacity = val;
}

inline int Brush::GetIndex()
{
    return _index;
}

class SUICORE_API TileBrush : public Brush
{
public:

    RTTIOfClass(TileBrush)

    TileBrush();
    virtual ~TileBrush();

    suic::Byte GetAlignmentX() const;
    void GetAlignmentX(suic::Byte val);

    suic::Byte GetAlignmentY() const;
    void SetAlignmentY(suic::Byte val);

    suic::Byte GetViewboxUnits() const;
    void SetViewboxUnits(suic::Byte val);

    suic::Byte GetViewportUnits() const;
    void SetViewportUnits(suic::Byte val);

    suic::Byte GetStretch() const;
    void SetStretch(suic::Byte val);

    suic::Byte GetTileMode() const;
    void SetTileMode(suic::Byte val);

    fRect GetViewbox() const;
    void SetViewbox(fRect val);

    fRect GetViewport() const;
    void SetViewport(fRect val);

protected:

    // 基于原图的显示区域，如果为图像
    // 则可指定绘制图像的某一区域
    fRect _viewbox;
    // 基于画布的显示区域
    fRect _viewport;

    suic::Byte _stretch;
    suic::Byte _tileMode;

    suic::Byte _viewBoxUnits;
    suic::Byte _viewportUnits;

    // 如果_stretch不是Fill或者UniformToFill
    // 则_alignmentX和_alignmentY有效
    suic::Byte _alignmentX;
    suic::Byte _alignmentY;
};

}

#endif
