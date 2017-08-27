// 华勤科技版权所有 2008-2022
// 
// 文件名：UITypes.h
// 功  能：定义核心库的基础类型。
// 
// 作  者：MPF开发组
// 时  间：2010-07-02
// 
// ============================================================================

#ifndef _UITYPES_H_
#define _UITYPES_H_

#include <Config/suicoreconfig.h>

namespace suic
{

typedef unsigned char Byte;
typedef unsigned char Uint8;
typedef unsigned short Uint16;
typedef unsigned int Uint32;
typedef unsigned __int64 Uint64;

typedef wchar_t Char;
typedef __int64 Time_t;

typedef char Int8;
typedef short Int16;
typedef int Int32;
typedef __int64 Int64;

typedef float Float;

#ifdef _WIN64
typedef __int64 HUIWND;
typedef __int64 DHDC;
typedef __int64 Handle;
#else
typedef unsigned long HUIWND;
typedef unsigned long DHDC;
typedef unsigned long Handle;
#endif

};

namespace suic
{

struct Size;
struct Rect;

struct fPoint;
struct fSize;
struct fRect;

enum UIDataType
{
    UnkownType=0,
    
    IntType,
    Uint32Type,
    FloatType,
    StringType,
    PointType,
    RectType,
    SizeType,
    BoolType,
    ColorType,
    ObjectType,
    AttriType,

    EventType = 100,
};

template<typename T>
struct Nullable
{
    T val;
    bool hasVal;

    typedef Nullable<T> MyType;

    Nullable()
    {
        val = T();
        hasVal = false;
    }

    Nullable(T v)
    {
        val = v;
        hasVal = true;
    }

    Nullable(const MyType& v)
    {
        val = v.val;
        hasVal = v.hasVal;
    }

    void Reset()
    {
        val = T();
        hasVal = false;
    }

    bool HasValue() const 
    { 
        return hasVal; 
    }

    T GetValue() const
    { 
        return val; 
    }

    T GetValueOrDefault(T defVal) const
    { 
        if (!HasValue()) 
        {
            return defVal;
        }
        else 
        {
            return val; 
        }
    }

    MyType& operator=(T v)
    {
        val = v;
        hasVal = true;
        return *this;
    }

    MyType& operator=(const MyType& v)
    {
        if (this != &v)
        {
            val = v.val;
            hasVal = v.hasVal;
        }

        return *this;
    }

    operator T() 
    { 
        return val; 
    }
};

typedef Nullable<Int32> IntNullable;
typedef Nullable<Uint32> UintNullable;
typedef Nullable<Time_t> TimetNullable;
typedef Nullable<Float> FloatNullable;

struct SUICORE_API IntPtr
{
    void* value;
    static IntPtr Zero;

    IntPtr() : value(NULL)
    {
    }

    IntPtr(void* v) : value(v)
    {
    }

    IntPtr(const IntPtr& Other) : value(Other.value)
    {
    }

    operator void*()
    {
        return value;
    }

    IntPtr& operator=(const IntPtr& Other)
    {
        if (&Other != this)
        {
            value = Other.value;
        }
        return *this;
    }

    bool operator==(const IntPtr& Other)
    {
        return (value == Other.value);;
    }

    bool operator!=(const IntPtr& Other)
    {
        return (value != Other.value);;
    }

    bool IsZero() const
    {
        return NULL == value;
    }
};

struct Point : public POINT
{
    Point() { x=0; y=0; }
    Point(int x, int y) { this->x=x; this->y=y; }
    Point(const Point* lppt) { x=lppt->x; y=lppt->y; }
    Point(const POINT* lppt) { x=lppt->x; y=lppt->y; }
    Point(const POINT& pt) { x=pt.x; y=pt.y; }
    Point(const Point& Other) { x=Other.x; y=Other.y; }

    operator Point& () { return *(this); }
    operator const Point& () const { return *(this); }

    Point& operator *() { return *(this); }

    Point& operator=(const Point& Other)
    {
        if (&Other != this)
        {
            x = Other.x;
            y = Other.y;
        }
        return (*this);
    }

    void Offset(int xf, int yf)
    {
        x += xf;
        y += yf;
    }

    void Offset(const Point& pt)
    {
        x += pt.x;
        y += pt.y;
    }

    Point& operator+=(const Point& Other)
    {
        x += Other.x;
        y += Other.y;
        return (*this);
    }

    Point& operator-=(const Point& Other)
    {
        x -= Other.x;
        y -= Other.y;
        return (*this);
    }

    bool operator==(const Point& Other) const
    {
        return (x == Other.x && y == Other.y);
    }

    bool operator!=(const Point& Other) const
    {
        return (x != Other.x || y != Other.y);
    }

    Size ToPoint() const;
    fPoint TofPoint() const;
};

struct Size : public SIZE
{
    Size() { cx=0; cy=0; }
    Size(int cx, int cy) { this->cx=cx; this->cy=cy; }
    Size(const Size* lppt) { cx=lppt->cx; cy=lppt->cy; }
    Size(const SIZE* lppt) { cx=lppt->cx; cy=lppt->cy; }
    Size(const SIZE& Other) { cx=Other.cx; cy=Other.cy; }
    Size(const Size& Other) { cx=Other.cx; cy=Other.cy; }

    operator Size& () { return *(this); }
    Size& operator*() { return *(this); }

    Size& operator=(const SIZE& Other)
    {
        cx = Other.cx;
        cy = Other.cy;
        return (*this);
    }

    Size& operator=(const Size& Other)
    {
        if (&Other != this)
        {
            cx = Other.cx;
            cy = Other.cy;
        }
        return (*this);
    }

    Size& operator+=(const Size& Other)
    {
        cx += Other.cx;
        cy += Other.cy;
        return (*this);
    }

    Size& operator-=(const Size& Other)
    {
        cx -= Other.cx;
        cy -= Other.cy;
        return (*this);
    }

    bool operator==(const Size& Other) const
    {
        return (cx == Other.cx && cy == Other.cy);
    }

    bool operator!=(const Size& Other) const
    {
        return (cx != Other.cx || cy != Other.cy);
    }

    Point ToPoint() const
    {
        return Point(cx, cy);
    }

    int Width() const { return cx; }
    int Height() const { return cy; }

    Rect ToRect() const;
    fSize TofSize() const;
};

inline Size Point::ToPoint() const
{
    return Size(x, y);
}

struct Rect : public RECT
{
    Rect() { SetEmpty(); }
    Rect(int x) { left=x; top=x; right=x; bottom=x; }
    Rect(int x, int y) { left=x; top=y; right=x; bottom=y; }
    Rect(int l, int t, int w, int h) { left=l; top=t; right=l+w; bottom=t+h; }
    Rect(const Rect* lprc) { Copy(lprc); }
    Rect(const RECT* lprc) { ::CopyRect(this, lprc); }
    Rect(const Rect& Other) { ::CopyRect(this, &Other); }
    Rect(const RECT& Other) { ::CopyRect(this, &Other); }
    Rect(Size size) { left=0; top=0; right=size.cx; bottom=size.cy; }
    Rect(Point pt, Size size) { left=pt.x; top=pt.y; right=left+size.cx; bottom=top+size.cy; }

    operator Rect& () { return *(this); }
    operator Rect* () { return this; }
    Rect& operator*() { return *(this); }

    Rect& operator=(const Rect& Other)
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

    Rect& operator=(const RECT& Other)
    {
        left = Other.left;
        top = Other.top;
        right = Other.right;
        bottom = Other.bottom;
        return (*this);
    }

    Rect& operator+=(const Rect& Other)
    {
        left += Other.left;
        top += Other.top;
        right += Other.right;
        bottom += Other.bottom;
        return (*this);
    }

    Rect& operator-=(const Rect& Other)
    {
        left -= Other.left;
        top -= Other.top;
        right -= Other.right;
        bottom -= Other.bottom;
        return (*this);
    }

    bool operator==(const Rect& Other) const
    {
        return (left == Other.left 
                && top == Other.top
                && right == Other.right 
                && bottom == Other.bottom);
    }

    bool operator!=(const Rect& Other) const
    {
        return (left != Other.left 
                || top != Other.top
                || right != Other.right 
                || bottom != Other.bottom);
    }

    int Width() const;
    int Height() const;

    int SumLR() const;
    int SumTB() const;

    bool Equal(const Rect * lprc) const;

    void Offset(int x, int y);
    void Offset(const Point& offset);

    void Inflate(int x, int y);
    void Inflate(int l, int t, int r, int b);
    void Inflate(const Rect & rc);
    void Deflate(int x, int y);
    void Deflate(int l, int t, int r, int b);
    void Deflate(const Rect & rc);
    void Deflate(Size size);

    void SetEmpty();
    void Copy(const Rect* lprc);
    bool Intersect(const Rect* lprc1, const Rect* lprc2);
    bool Intersect(const Rect* lprc2);
    Rect& IntersectRet(const Rect* lprc2);
    Rect IntersectRect(const Rect* lprc2);
    bool Union(const Rect* lprc1, const Rect* lprc2);
    bool Union(const Rect* lprc2);
    void UnionOnly(const Rect* lprc2);
    bool Empty() const;
    bool IsZero() const;
    bool PointIn(POINT pt) const;
    Size ToSize() const;
    bool Contains(Rect rect) const;

    Point LeftTop() const
    {
        return Point(left, top);
    }

    fRect TofRect() const;

    void SetLTRB(int l, int t, int r, int b)
    {
        left = l; 
        top = t;
        right = r; 
        bottom = b;
    }

    void SetXYWH(int x, int y, int w, int h)
    {
        left = x; 
        top = y;
        right = x + w; 
        bottom = y + h;
    }
};

inline Rect Size::ToRect() const
{
    return Rect(0, 0, cx, cy);
}

inline int Rect::Width() const
{
    return right - left;
}

inline int Rect::Height() const
{
    return bottom - top;
}

inline int Rect::SumLR() const
{
    return right + left;
}

inline int Rect::SumTB() const
{
    return bottom + top;
}

inline bool Rect::Equal(const Rect* lprc) const
{
    return (*this == *lprc);
}

inline void Rect::Offset(int x, int y)
{
    ::OffsetRect(this, x, y);
}

inline void Rect::Offset(const Point& offset)
{
    ::OffsetRect(this, offset.x, offset.y);
}

inline void Rect::Inflate(int x, int y)
{
    ::InflateRect(this, x, y);
}

inline void Rect::Inflate(const Rect& rc)
{
    left -= rc.left; top -= rc.top;
    right += rc.right; bottom += rc.bottom;
}

inline void Rect::Inflate(int l, int t, int r, int b)
{
    left -= l; top -= t;
    right += r; bottom += b;
}

inline void Rect::Deflate(int x, int y)
{
    ::InflateRect(this, -x, -y);
}

inline void Rect::Deflate(Size size)
{
    ::InflateRect(this, -size.cx, -size.cy);
}

inline void Rect::Deflate(int l, int t, int r, int b)
{
    left += l; top += t;
    right -= r; bottom -= b;
}

inline void Rect::Deflate(const Rect& rc)
{
    Deflate(rc.left, rc.top, rc.right, rc.bottom);
}

inline void Rect::SetEmpty()
{
    ::SetRectEmpty(this);
}

inline void Rect::Copy(const Rect* lprc)
{
    ::CopyRect(this, lprc);
}

inline bool Rect::Intersect(const Rect* lprc1, const Rect* lprc2)
{
    return (::IntersectRect(this, lprc1, lprc2) != 0);
}

inline bool Rect::Intersect(const Rect* lprc2)
{
    return (::IntersectRect(this, this, lprc2) != 0);
}

inline Rect& Rect::IntersectRet(const Rect * lprc2)
{
    Intersect(lprc2);
    return (*this);
}

inline Rect Rect::IntersectRect(const Rect* lprc2)
{
    Rect ret;
    ::IntersectRect(&ret, this, lprc2);
    return (ret);
}

inline bool Rect::Union(const Rect* lprc1, const Rect* lprc2)
{
    return (::UnionRect(this, lprc1, lprc2) ? true : false);
}

inline bool Rect::Union(const Rect* lprc2)
{
    return (::UnionRect(this, this, lprc2) ? true : false);
}

inline void Rect::UnionOnly(const Rect* lprc2)
{
    ::UnionRect(this, this, lprc2);
}

inline bool Rect::Empty() const
{
    return (::IsRectEmpty(this) ? true : false);
}

inline bool Rect::IsZero() const
{
    if (0 == left && 0 == top && 
        0 == right && 0 == bottom)
    {
        return true;
    }
    else
    {
        return false;
    }
}

inline bool Rect::PointIn(POINT pt) const
{
    return (::PtInRect(this, pt) ? true : false);
}

inline Size Rect::ToSize() const
{
    return Size(Width(), Height());
}

inline bool Rect::Contains(Rect rect) const
{
    return (left <= rect.left && top <= rect.top && right >= rect.right && bottom >= rect.bottom);
}

}

#endif
