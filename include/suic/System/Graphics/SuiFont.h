// 华勤科技版权所有 2010-2011
// 
// 文件名：uifont
// 功  能：定义核心库的字体属性。
// 
// 作  者：Sharpui开发组
// 时  间：2010-07-02
// 
// ============================================================================

#ifndef _UIFONT_H_
#define _UIFONT_H_

#include <System/Types/Types.h>
#include <System/Graphics/Resource.h>

namespace suic
{

class FontStyle
{
public:

    enum {Italic=0x0001, Underline=0x0002, StrikeOut=0x0004, Bold=0x0008};
};

struct Typeface {};

class SUICORE_API Font : public Resource
{
public:

    enum Style 
    {
        fNormal = 0,
        fBold   = 0x01,
        fItalic = 0x02,
        fBoldItalic = 0x03
    };

    DeclareTypeofClass(Font, Resource)

    Font();
    virtual ~Font();

    void Reset();

    Font* Clone() const;

    Handle GetHandle();
    Typeface* GetTypeface();

    void SetFamily(String name);
    String GetFamily() const;

    void SetSize(Int32 size);
    Int32 GetSize() const;

    void SetWeight(Int32 weight);
    Int32 GetWeight() const;

    void SetItalic(Byte value);
    Byte GetItalic() const;

    void SetUnderline(Byte value);
    Byte GetUnderline() const;

    void SetAngle(int val);
    int GetAngle() const;

    void SetType(Uint32 val);
    Uint32 GetType() const;

    String ToString();

protected:

    Handle _h;
    String _key;
    String _name;
    Int32 _size;
    Int32 _weight;
    Byte _underline;
    Byte _italic;
    Uint32 _type;
    int _angle;
    Byte _crs;

    Uint32 _uniqueId;
    Style _style;
    bool _isFixedWidth;
};

typedef shared<Font> FontPtr;

inline String Font::GetFamily() const
{
    return _name;
}

inline Int32 Font::GetSize() const
{
    return _size;
}

inline Int32 Font::GetWeight() const
{
    return _weight;
}

inline Byte Font::GetItalic() const
{
    return _italic;
}

inline Byte Font::GetUnderline() const
{
    return _underline;
}

inline int Font::GetAngle() const
{
    return _angle;
}

inline Uint32 Font::GetType() const
{
    return _type;
}

}

#endif
