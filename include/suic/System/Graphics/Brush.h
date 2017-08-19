// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����Brush.h
// ��  �ܣ�����ˢ�ӻ��ࡣ
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2010-07-02
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

    RTTIOfAbstractClass(Brush)

    Brush() : _opacity(1.0) {}
    virtual ~Brush() {}

    Brush* Clone() const;

    Float GetOpacity() const;
    void SetOpacity(Float val);

    virtual Color ToColor();
    virtual bool Parse(const String& xml);
    virtual bool Parse(IXamlNode* pNode);

protected:

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

    // ����ԭͼ����ʾ�������Ϊͼ��
    // ���ָ������ͼ���ĳһ����
    fRect _viewbox;
    // ���ڻ�������ʾ����
    fRect _viewport;

    suic::Byte _stretch;
    suic::Byte _tileMode;

    suic::Byte _viewBoxUnits;
    suic::Byte _viewportUnits;

    // ���_stretch����Fill����UniformToFill
    // ��_alignmentX��_alignmentY��Ч
    suic::Byte _alignmentX;
    suic::Byte _alignmentY;
};

}

#endif
