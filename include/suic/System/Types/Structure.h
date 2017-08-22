// 华勤科技版权所有 2010-2011
// 
// 文件名：UIInterface.h
// 功  能：定义核心库的用户注册接口。
// 
// 作  者：MPF开发组
// 时  间：2010-07-02
// 
// ============================================================================

#ifndef _UISTRUCTURE_H_
#define _UISTRUCTURE_H_

#include <System/Types/Const.h>
#include <System/Windows/Event.h>
#include <System/Graphics/Bitmap.h>
#include <System/Tools/Array.h>
#include <System/Types/Integer.h>

namespace suic 
{

struct fRRect;

//------------------------------------------------------

struct SUICORE_API fPoint
{
    Float x;
    Float y;

    fPoint()
        : x(0), y(0)
    {

    }

    fPoint(const fPoint& Other)
    {
        x = Other.x;
        y = Other.y;
    }

    fPoint(Float v1, Float v2)
        : x(v1), y(v2)
    {

    }

    Point ToPoint() const
    {
        return Point((int)x, (int)y);
    }

    fPoint& operator=(const fPoint& Other)
    {
        if (&Other != this)
        {
            x = Other.x;
            y = Other.y;
        }
        return (*this);
    }

    fPoint& operator+=(const fPoint& Other)
    {
        x += Other.x;
        y += Other.y;
        return (*this);
    }

    fPoint& operator-=(const fPoint& Other)
    {
        x -= Other.x;
        y -= Other.y;
        return (*this);
    }

    bool operator!=(const fPoint& Other) const;
    bool operator==(const fPoint& Other) const;
};

struct SUICORE_API fSize
{
    Float cx;
    Float cy;

    fSize()
        : cx(0), cy(0)
    {

    }

    fSize(const fSize& Other)
    {
        cx = Other.cx;
        cy = Other.cy;
    }

    fSize(Float v1, Float v2)
        : cx(v1), cy(v2)
    {

    }

    Float Width() const
    {
        return cx;
    }

    Float Height() const
    {
        return cy;
    }

    Size ToSize() const
    {
        return Size((int)cx, (int)cy);
    }

    fSize& operator=(const fSize& Other)
    {
        if (&Other != this)
        {
            cx = Other.cx;
            cy = Other.cy;
        }
        return (*this);
    }

    fSize& operator+=(const fSize& Other)
    {
        cx += Other.cx;
        cy += Other.cy;
        return (*this);
    }

    fSize& operator-=(const fSize& Other)
    {
        cx -= Other.cx;
        cy -= Other.cy;
        return (*this);
    }

    bool operator!=(const fSize& Other) const;
    bool operator==(const fSize& Other) const;
};

struct SUICORE_API fRect
{
    Float left;
    Float top;
    Float right;
    Float bottom;

    fRect()
        : left(0), top(0), right(0), bottom(0)
    {

    }

    fRect(const fRect& Other)
    {
        left = Other.left;
        top = Other.top;
        right = Other.right;
        bottom = Other.bottom;
    }

    fRect(Float x, Float y, Float w, Float h)
        : left(x), top(y), right(x + w), bottom(y + h)
    {
    }

    fRect(Point pt, Size size)
        : left((Float)pt.x)
        , top((Float)pt.y)
        , right((Float)(pt.x+size.cx))
        , bottom((Float)(pt.y+size.cy))
    {
    }

    fRect(fPoint pt, fSize size)
        : left(pt.x)
        , top(pt.y)
        , right(pt.x+size.cx)
        , bottom(pt.y+size.cy)
    {

    }

    static fRect MakeXYWH(Float x, Float y, Float w, Float h)
    {
        return fRect(x, y, w, h);
    }

    static fRect MakeLTRB(Float l, Float t, Float r, Float b)
    {
        fRect rect;
        rect.left = l;
        rect.top = t;
        rect.right = r;
        rect.bottom = b;
        return rect;
    }

    Float SumLR() const
    {
        return (right + left);
    }

    Float SumTB() const
    {
        return (bottom + top);
    }

    Float Left() const
    {
        return left;
    }

    Float Top() const
    {
        return top;
    }

    Float Right() const
    {
        return right;
    }

    Float Bottom() const
    {
        return bottom;
    }

    Float Width() const
    {
        return right - left;
    }

    Float Height() const
    {
        return bottom - top;
    }

    Rect ToRect() const
    {
        return Rect((int)left, (int)top, (int)Width(), (int)Height());
    }

    bool Empty() const
    {
        return (left > right || top > bottom);
    }

    bool IsZero() const;

    fRect& operator=(const fRect& Other)
    {
        if (&Other != this)
        {
            left = Other.left;
            top = Other.top;
            right = Other.right;
            bottom = Other.bottom;
        }
        return (*this);
    }

    fRect& operator+=(const fRect& Other)
    {
        right = (Other.Right() + Right());
        bottom = (Other.Bottom() + Bottom());
        left += Other.left;
        top += Other.top;
        return (*this);
    }

    fRect& operator-=(const fRect& Other)
    {
        right = (Other.Right() - Right());
        bottom = (Other.Bottom() - Bottom());
        left -= Other.left;
        top -= Other.top;
        return (*this);
    }

    bool operator!=(const fRect& Other) const;
    bool operator==(const fRect& Other) const;

    void SetLTRB(Float l, Float t, Float r, Float b)
    {
        left = l; 
        top = t;
        right = r; 
        bottom = b;
    }

    void SetXYWH(Float x, Float y, Float w, Float h)
    {
        left = x; 
        top = y;
        right = x + w; 
        bottom = y + h;
    }

    void Offset(Float x, Float y)
    {
        left += x;
        right += x;
        top += y;
        bottom += y;
    }

    void Offset(const fPoint& offset)
    {
        Offset(offset.x, offset.y);
    }

    void Inflate(Float x, Float y)
    {
        Inflate(x, y, x, y);
    }

    void Inflate(Float l, Float t, Float r, Float b)
    {
        left -= l;
        top -= t;
        right +=  r;
        bottom += b;
    }

    void Inflate(const fRect& rc)
    {
        Inflate(rc.left, rc.top, rc.right, rc.bottom);
    }

    void Deflate(Float x, Float y)
    {
        Deflate(x, y, x, y);
    }

    void Deflate(Float l, Float t, Float r, Float b)
    {
        left += l;
        top += t;
        right -=  r;
        bottom -= b;
    }

    void Deflate(const fRect & rc)
    {
        Deflate(rc.left, rc.top, rc.right, rc.bottom);
    }

    fRect Intersect(const fRect* lprc)
    {
        fRect ret;
        ret.left = Math::Max(left, lprc->left);
        ret.top = Math::Max(top, lprc->top);
        ret.right = Math::Min(right, lprc->right);
        ret.bottom = Math::Min(bottom, lprc->bottom);
        return ret;
    }

    fRect Union(const fRect* lprc1, const fRect* lprc2)
    {
        left = Math::Min(lprc1->left, lprc2->left);
        top = Math::Min(lprc1->top, lprc2->top);
        right = Math::Max(lprc1->right, lprc2->right);
        bottom = Math::Max(lprc1->bottom, lprc2->bottom);
        return (*this);
    }

    fRect Union(const fRect* lprc2)
    {
        return Union(this, lprc2);
    }

    fPoint LeftTop() const
    {
        return fPoint(left, top);
    }

    fSize TofSize() const 
    { 
        return fSize(Width(), Height()); 
    }

    fRRect TofRRect() const;

    operator fRect& () 
    { 
        return *(this); 
    }

    operator fRect* () 
    { 
        return this; 
    }
};

inline fPoint Point::TofPoint() const
{
    return fPoint((Float)x, (Float)y);
}

inline fSize Size::TofSize() const
{
    return fSize((Float)cx, (Float)cy);
}

inline fRect Rect::TofRect() const
{
    return fRect::MakeLTRB((Float)left, (Float)top, (Float)right, (Float)bottom);
}

struct Thickness
{
    Float left;
    Float top;
    Float right;
    Float bottom;

    Thickness()
        : left(0)
        , top(0)
        , right(0)
        , bottom(0)
    {
    }
};

struct Radii
{
    Float LeftTop;
    Float TopLeft;
    Float TopRight;
    Float RightTop;
    Float RightBottom;
    Float BottomRight;
    Float BottomLeft;
    Float LeftBottom;
};

struct SUICORE_API CornerRadius
{
    Float topLeft;
    Float topRight;
    Float bottomLeft;
    Float bottomRight;

    CornerRadius()
        : topLeft(0)
        , topRight(0)
        , bottomLeft(0)
        , bottomRight(0)
    {
    }
};

struct fRRect
{
    fRect rect;
    fPoint radii[4];

    fRRect()
    {
        memset(&radii[0], 0, sizeof(radii));
    }

    fRRect(fRect rc)
    {
        rect = rc;
    }

    operator fRRect& () { return *(this); }
    operator fRRect* () { return this; }
};

inline fRRect fRect::TofRRect() const
{
    return fRRect(*this);
}

class SUICORE_API ORect : public OType
{
public:

    static ORect* GetEmpty();
    static Rect FromObj(Object* obj);

    RTTIOfClass(ORect)

        ORect();
    ORect(const Rect& rc, bool autodel);

    ORect(const Rect* rc);
    ORect(const Rect& rc);
    ORect(const String& value);

    ORect* Clone() const;
    bool Equals(Object* Other);

    String ToString();

    Rect ToRect() 
    { 
        return _rect; 
    }

    fRect TofRect() 
    {
        return _rect.TofRect();
    }

    void SetRect(Rect rect);

    operator Rect() 
    { 
        return _rect; 
    }

    ORect& operator=(const Rect& other);

protected:

    Rect _rect;
    static ORect* EmptyRect;
};

class SUICORE_API OfRect : public OType
{
public:

    static OfRect* GetEmpty();
    static fRect FromObj(Object* obj);

    RTTIOfClass(OfRect)

        OfRect();
    OfRect(const String& value);
    OfRect(const fRect& rc);
    OfRect(const fRect& rc, bool autodel);

    OfRect* Clone() const;
    bool Equals(Object* Other);

    Rect ToRect();
    String ToString();

    fRect TofRect() 
    {
        return _rect;
    }

    void SetfRect(fRect rect);

    operator fRect() 
    { 
        return _rect; 
    }

protected:

    fRect _rect;
    static OfRect* Empty;
};

inline Rect OfRect::ToRect() 
{
    return Rect((int)_rect.left, (int)_rect.top, 
        (int)(_rect.right - _rect.left), 
        (int)(_rect.bottom - _rect.top)); 
}

class SUICORE_API OfSize : public OType
{
public:

    static OfSize* GetEmpty();
    static fSize FromObj(Object* obj);

    RTTIOfClass(OfSize)

        OfSize();
    OfSize(const String& value);
    OfSize(const fSize& rc);
    OfSize(const fSize& rc, bool autodel);

    OfSize* Clone() const;
    bool Equals(Object* Other);

    String ToString();

    Size ToSize() 
    {
        return Size((int)_size.cx, (int)_size.cy); 
    }

    void SetfSize(fSize size);

    fSize TofSize() 
    {
        return _size;
    }

    Point ToPoint() 
    {
        return Point((int)_size.cx, (int)_size.cy); 
    }

    fPoint TofPoint() 
    {
        return fPoint(_size.cx, _size.cy); 
    }

    operator fSize() 
    { 
        return _size; 
    }

protected:

    fSize _size;
    static OfSize* Empty;
};

class SUICORE_API OfPoint : public OType
{
public:

    static OfPoint* GetEmpty();
    static fPoint FromObj(Object* obj);

    RTTIOfClass(OfPoint)

        OfPoint();
    OfPoint(const String& value);
    OfPoint(const fPoint& pt);
    OfPoint(const fPoint& lt, bool autodel);

    OfPoint* Clone() const;
    bool Equals(Object* Other);

    String ToString();

    Size ToSize() 
    {
        return Size((int)_point.x, (int)_point.y); 
    }

    fSize TofSize() 
    {
        return fSize(_point.x, _point.y); 
    }

    void SetfPoint(fPoint point);

    Point ToPoint() 
    {
        return Point((int)_point.x, (int)_point.y); 
    }

    fPoint TofPoint() 
    {
        return _point;
    }

    operator fPoint() 
    { 
        return _point; 
    }

protected:

    fPoint _point;
    static OfPoint* Empty;
};

class SUICORE_API OSize : public OType
{
public:

    static OSize* GetEmpty();
    static Size FromObj(Object* obj);

    RTTIOfClass(OSize)

        OSize();
    OSize(const Size& size, bool autodel);
    OSize(const Size* size);
    OSize(const Size& size);
    OSize(const String& value);

    OSize* Clone() const;
    bool Equals(Object* Other);

    Rect ToRect() 
    { 
        return Rect(0, 0, _size.cx, _size.cy); 
    }

    fRect TofRect() 
    { 
        return fRect(0, 0, _size.cx, _size.cy); 
    }

    Size ToSize() 
    { 
        return _size; 
    }

    fSize TofSize() 
    { 
        return _size.TofSize(); 
    }

    void SetSize(Size size);

    String ToString();

    operator Size() 
    { 
        return _size; 
    }

    OSize& operator=(const Size& other);

protected:

    Size _size;
    static OSize* EmptySize;
};

inline OSize& OSize::operator=(const Size& other) 
{ 
    _size = other;
    return (*this);
}

class SUICORE_API OPoint : public OType
{
public:

    static OPoint* GetEmpty();
    static Point FromObj(Object* obj);

    RTTIOfClass(OPoint)

        OPoint();
    OPoint(const Point& pt, bool autodel);
    OPoint(const Point* pt);
    OPoint(const Point& pt);
    OPoint(const String& value);

    OPoint* Clone() const;
    bool Equals(Object* Other);

    Rect ToRect();
    Size ToSize();
    Point ToPoint();
    String ToString();

    void SetPoint(Point point);

    operator Point() 
    { 
        return _point; 
    }

    OPoint& operator=(const Point& other);

protected:

    Point _point;
    static OPoint* EmptyPoint;
};

class SUICORE_API OColor : public OType
{
public:

    static OColor* GetDefaultColor();
    static Color FromObj(Object* obj);
    static Color StringAsColor(const String& clr, bool& bValid);
    static Color FromString(const String& value);

    RTTIOfClass(OColor)

        OColor();
    OColor(Uint32 value, bool autodel);
    OColor(Uint32 value);
    OColor(const String& value);

    OColor* Clone() const;
    bool Equals(Object* Other);

    Color ToColor() { return _color; }
    bool ToBool() { return _color != 0; }
    Float ToFloat() { return (Float)_color; }
    Int32 ToInt() { return (Int32)_color; }

    void SetColor(Color color);

    String ToString();
    operator Uint32() { return _color; }
    OColor& operator=(const Uint32& other);

protected:

    Uint32 _color;
    static OColor* DefaultColor;
};

class SUICORE_API Boolean : public OType
{
public:

    RTTIOfAbstractClass(Boolean)

        static Boolean* FromBool(bool val);
    static Boolean* FromByte(Byte val);
    static Boolean* FromString(String val);

    Boolean* Clone() const;
    bool Equals(Object* Other);

    bool ToBool() const;
    Int32 ToInt();
    Float ToFloat();
    String ToString();

    operator bool() { return _value == 1; }
    Boolean& operator=(const bool& other);

    static Boolean* True;
    static Boolean* False;
    static Boolean* Nullable;

protected:

    Boolean(Byte val);

    Byte _value;
};

inline Boolean& Boolean::operator=(const bool& other) 
{ 
    _value = other;
    return (*this);
}

class SUICORE_API OFloat : public OType
{
public:

    static OFloat* GetZeroFloat();
    static OFloat* GetNegInfinity();
    static OFloat* GetPosInfinity();

    static Float FromObj(Object* obj);

    RTTIOfClass(OFloat)

        OFloat();
    OFloat(Float value, bool autodel);
    OFloat(Float value);
    OFloat(const String& value);

    OFloat* Clone() const;
    bool Equals(Object* Other);

    Int32 ToInt() { return (Int32)_value; }
    Float ToFloat() { return _value; }
    String ToString();

    void SetFloat(Float val);

    operator Float() { return _value; }
    OFloat& operator=(const Float& other);

protected:

    Float _value;

    static OFloat* NegInf;
    static OFloat* PosInf;
    static OFloat* ZeroFloat;
};

class SUICORE_API ImageSource : public OType
{
public:

    RTTIOfClass(ImageSource)

    ImageSource();
    ImageSource(const String& uri);

    void SetUri(ResourceUri uri);
    ResourceUri GetUri() const;

    int Width();
    int Height();

    Bitmap* GetBitmap();

private:

    Bitmap _bmp;
    ResourceUri _uri;
};

typedef shared<ImageSource> ImageSourcePtr;

template<typename K, typename V>
struct KeyValuePair
{
    KeyValuePair()
    {

    }

    KeyValuePair(K k, V v)
    {
        key = k;
        val = v;
    }

    K GetKey() const
    {
        return key;
    }

    V GetValue() const
    {
        return val;
    }

    void SetKey(const K& k)
    {
        key = k;
    }

    void SetValue(const V& v)
    {
        val = v;
    }

private:

    K key;
    V val;
};

//============================================================
// 定义鼠标测试结构
class HitTestParam
{
public:

    // 测试坐标点
    Point hitPoint;
};

struct SUICORE_API HitTestFilterAction
{
    enum
    {
        Continue,
        Stop,
        ContinueSkipSelf,
        ContinueSkipChildren,
        ContinueSkipSelfAndChildren,
    };

    static Integer* From(int index);

    static Integer* ContinueBox;
    static Integer* StopBox;
    static Integer* ContinueSkipSelfBox;
    static Integer* ContinueSkipChildrenBox;
    static Integer* ContinueSkipSelfAndChildrenBox;
};

}

#endif
