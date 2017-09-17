
#include <System/Types/Structure.h>
#include <System/Types/Integer.h>
#include <System/Types/String.h>
#include <System/Types/Numeric.h>
#include <System/Interop/System.h>
#include <System/Graphics/SolidcolorBrush.h>

namespace suic 
{

ImplementRTTIOfClass(OType, Object)
ImplementRTTIOfClassName(ORect, OType, Rect)
ImplementRTTIOfClassName(OSize, OType, Size)
ImplementRTTIOfClassName(OPoint, OType, Point)
ImplementRTTIOfClassName(OColor, OType, Color)
ImplementRTTIOfClass(Boolean, OType)
ImplementRTTIOfClassName(OFloat, OType, Float)
ImplementRTTIOfClass(ImageSource, OType)
ImplementRTTIOfClass(Integer, OType)

ORect* ORect::EmptyRect;
OSize* OSize::EmptySize;
OPoint* OPoint::EmptyPoint;
OColor* OColor::DefaultColor;

OFloat* OFloat::NegInf;
OFloat* OFloat::PosInf;
OFloat* OFloat::ZeroFloat;
Integer* Integer::ZeroInteger;

OType::OType()
    : _sealed(0)
{

}

void OType::Sealed()
{
    _sealed = 1;
}

bool OType::IsSealed() const
{
    return _sealed != 0;
}

ORect* ORect::GetEmpty()
{
    if (NULL == EmptyRect)
    {
        EmptyRect = new ORect(Rect(), false);
        EmptyRect->Sealed();
    }
    return EmptyRect;
}

Rect ORect::FromObj(Object* obj)
{
    RTTIOfInfo* rInfo = obj->GetRTTIType();
    if (rInfo == ORect::RTTIType())
    {
        return ((ORect*)obj)->ToRect();
    }
    else if (rInfo == OfRect::RTTIType())
    {
        return ((OfRect*)obj)->TofRect().ToRect();
    }
    else if (rInfo == OString::RTTIType())
    {
        return ((OString*)obj)->ToRect();
    }
    else
    {
        return Rect();
    }
}

OSize* OSize::GetEmpty()
{
    if (NULL == EmptySize)
    {
        EmptySize = new OSize(Size(), false);
        EmptySize->Sealed();
    }
    return EmptySize;
}

Size OSize::FromObj(Object* obj)
{
    RTTIOfInfo* rInfo = obj->GetRTTIType();
    if (rInfo == OSize::RTTIType())
    {
        return ((OSize*)obj)->ToSize();
    }
    else if (rInfo == OfSize::RTTIType())
    {
        return ((OfSize*)obj)->TofSize().ToSize();
    }
    else if (rInfo == OString::RTTIType())
    {
        return ((OString*)obj)->ToSize();
    }
    else
    {
        return Size();
    }
}

OPoint* OPoint::GetEmpty()
{
    if (NULL == EmptyPoint)
    {
        EmptyPoint = new OPoint(Point(), false);
        EmptyPoint->Sealed();
    }
    return EmptyPoint;
}

Point OPoint::FromObj(Object* obj)
{
    RTTIOfInfo* rInfo = obj->GetRTTIType();
    if (rInfo == OPoint::RTTIType())
    {
        return ((OPoint*)obj)->ToPoint();
    }
    else if (rInfo == OfPoint::RTTIType())
    {
        return ((OfPoint*)obj)->TofPoint().ToPoint();
    }
    else if (rInfo == OString::RTTIType())
    {
        return ((OString*)obj)->ToPoint();
    }
    else
    {
        return Point();
    }
}

OColor* OColor::GetDefaultColor()
{
    if (NULL == DefaultColor)
    {
        DefaultColor = new OColor(0, false);
        DefaultColor->Sealed();
    }
    return DefaultColor;
}

Color OColor::FromObj(Object* obj)
{
    if (obj->GetRTTIType() == OColor::RTTIType())
    {
        return ((OColor*)obj)->ToColor();
    }
    else if (obj->GetRTTIType() == SolidColorBrush::RTTIType())
    {
        return ((SolidColorBrush*)obj)->ToColor();
    }
    else
    {
        return 0xFF000000;
    }
}

static OFloat* CreatOFloat(Float val)
{
    OFloat* oVal = new OFloat(val, false);
    oVal->Sealed();
    return oVal;
}

OFloat* OFloat::GetZeroFloat()
{
    if (NULL == ZeroFloat)
    {
        ZeroFloat = CreatOFloat(0.0f);
    }
    return ZeroFloat;
}

OFloat* OFloat::GetNegInfinity()
{
    if (NULL == NegInf)
    {
        NegInf = CreatOFloat(FloatUtil::NegInfinity);
    }
    return NegInf;
}

OFloat* OFloat::GetPosInfinity()
{
    if (NULL == PosInf)
    {
        PosInf = CreatOFloat(FloatUtil::PosInfinity);
    }
    return PosInf;
}

Float OFloat::FromObj(Object* obj)
{
    if (obj->GetRTTIType() == OFloat::RTTIType())
    {
        return ((OFloat*)obj)->ToFloat();
    }
    else
    {
        return 0.0f;
    }
}

ORect* ORect::Clone() const
{
    ORect* obj(new ORect());
    obj->_rect = _rect;
    return obj;
}

OSize* OSize::Clone() const
{
    OSize* obj(new OSize());
    obj->_size = _size;
    return obj;
}

OPoint* OPoint::Clone() const
{
    OPoint* obj(new OPoint());
    obj->_point = _point;
    return obj;
}

OColor* OColor::Clone() const
{
    OColor* obj(new OColor());
    obj->_color = _color;
    return obj;
}

Boolean* Boolean::Clone() const
{
    return Boolean::FromByte(_value);
}

OFloat* OFloat::Clone() const
{
    OFloat* obj(new OFloat());
    obj->_value = _value;
    return obj;
}

ORect::ORect()
{

}

ORect::ORect(const Rect& rc, bool autodel)
    : _rect(rc)
{
    setAutoDelete(autodel);
}

ORect::ORect(const Rect * rc)
    : _rect(*rc)
{
}

ORect::ORect(const Rect& rc)
    : _rect(rc)
{
}

ORect::ORect(const String& value)
{
    _rect = value.ToRect();
}

void ORect::SetRect(Rect rect)
{
    if (!IsSealed())
    {
        _rect = rect;
    }
}

ORect& ORect::operator=(const Rect& other) 
{
    if (!IsSealed())
    {
        _rect = other;
    }
    return (*this);
}

bool ORect::Equals(Object* Other)
{
    ORect* pOther(dynamic_cast<ORect*>(Other));
    if (NULL == pOther)
    {
        return false;
    }
    else
    {
        return (pOther->_rect.Equal(&_rect));
    }
}

String ORect::ToString()
{
    String str;

    if (_rect.left == _rect.right && _rect.top == _rect.bottom)
    {
        if (_rect.left == _rect.top)
        {
            str.Format(_T("%d"), _rect.left);
        }
        else
        {
            str.Format(_T("%d,%d"), _rect.left, _rect.top);
        }
    }
    else
    {
        str.Format(_T("%d,%d,%d,%d"), _rect.left, _rect.top, _rect.right, _rect.bottom);
    }

    return str;
}

//=================================================
// OfRect

OfRect* OfRect::Empty;

ImplementRTTIOfClassName(OfRect, Object, fRect)

OfRect* OfRect::GetEmpty()
{
    if (NULL == Empty)
    {
        Empty = new OfRect(fRect(), false);
        Empty->Sealed();
    }
    return Empty;
}

fRect OfRect::FromObj(Object* obj)
{
    RTTIOfInfo* rInfo = obj->GetRTTIType();
    if (rInfo == OfRect::RTTIType())
    {
        return ((OfRect*)obj)->TofRect();
    }
    else if (rInfo == ORect::RTTIType())
    {
        return ((ORect*)obj)->ToRect().TofRect();
    }
    else if (rInfo == OString::RTTIType())
    {
        return ((OString*)obj)->TofRect();
    }
    else
    {
        return fRect();
    }
}

OfRect::OfRect()
{

}

OfRect::OfRect(const String& value)
{
    _rect = value.TofRect();
}

OfRect::OfRect(const fRect& rc)
{
    _rect = rc;
}

OfRect::OfRect(const fRect& rc, bool autodel)
{
    _rect = rc;
    setAutoDelete(autodel);
}

OfRect* OfRect::Clone() const
{
    OfRect* obj(new OfRect());
    obj->_rect = _rect;
    return obj;
}

void OfRect::SetfRect(fRect rect)
{
    if (!IsSealed())
    {
        _rect = rect;
    }
}

bool OfRect::Equals(Object* Other)
{
    OfRect* pOther(dynamic_cast<OfRect*>(Other));
    if (NULL == pOther)
    {
        return false;
    }
    else
    {
        return (pOther->_rect == _rect);
    }
}

String OfRect::ToString()
{
    String str;
    str.Format(_T("%g,%g,%g,%g"), _rect.left, _rect.top, _rect.right, _rect.bottom);
    return str;
}

//=================================================
// OfSize

OfSize* OfSize::Empty;
OfPoint* OfPoint::Empty;

ImplementRTTIOfClassName(OfSize, OType, fSize)
ImplementRTTIOfClassName(OfPoint, OType, fPoint)

OfSize* OfSize::GetEmpty()
{
    if (NULL == Empty)
    {
        Empty = new OfSize(fSize(), false);
        Empty->Sealed();
    }
    return Empty;
}

fSize OfSize::FromObj(Object* obj)
{
    RTTIOfInfo* rInfo = obj->GetRTTIType();
    if (rInfo == OfSize::RTTIType())
    {
        return ((OfSize*)obj)->TofSize();
    }
    else if (rInfo == OSize::RTTIType())
    {
        return ((OSize*)obj)->ToSize().TofSize();
    }
    else
    {
        return fSize();
    }
}

OfSize::OfSize()
{

}

OfSize::OfSize(const String& value)
{
    _size = value.TofSize();
}

OfSize::OfSize(const fSize& size)
{
    _size = size;
}

OfSize::OfSize(const fSize& size, bool autodel)
{
    _size = size;
    setAutoDelete(autodel);
}

OfSize* OfSize::Clone() const
{
    OfSize* obj(new OfSize());
    obj->_size = _size;
    return obj;
}

bool OfSize::Equals(Object* Other)
{
    OfSize* pOther(dynamic_cast<OfSize*>(Other));
    if (NULL == pOther)
    {
        return false;
    }
    else
    {
        return (pOther->_size == _size);
    }
}

void OfSize::SetfSize(fSize size)
{
    if (!IsSealed())
    {
        _size = size;
    }
}

String OfSize::ToString()
{
    String str;
    str.Format(_T("%g,%g"), _size.cx, _size.cy);
    return str;
}

//=================================================
// OfPoint

OfPoint::OfPoint()
{

}

OfPoint::OfPoint(const String& value)
{
    _point = value.TofPoint();
}

OfPoint::OfPoint(const fPoint& pt)
{
    _point = pt;
}

OfPoint::OfPoint(const fPoint& pt, bool autodel)
{
    _point = pt;
    setAutoDelete(autodel);
}

OfPoint* OfPoint::Clone() const
{
    OfPoint* obj(new OfPoint());
    obj->_point = _point;
    return obj;
}

bool OfPoint::Equals(Object* Other)
{
    OfPoint* pOther(dynamic_cast<OfPoint*>(Other));
    if (NULL == pOther)
    {
        return false;
    }
    else
    {
        return (pOther->_point == _point);
    }
}

void OfPoint::SetfPoint(fPoint point)
{
    if (!IsSealed())
    {
        _point = point;
    }
}

String OfPoint::ToString()
{
    String str;
    str.Format(_T("%g,%g"), _point.x, _point.y);
    return str;
}

OfPoint* OfPoint::GetEmpty()
{
    if (NULL == Empty)
    {
        Empty = new OfPoint(fPoint(), false);
        Empty->Sealed();
    }
    return Empty;
}

fPoint OfPoint::FromObj(Object* obj)
{
    RTTIOfInfo* rInfo = obj->GetRTTIType();
    if (rInfo == OfPoint::RTTIType())
    {
        return ((OfPoint*)obj)->TofPoint();
    }
    else if (rInfo == OPoint::RTTIType())
    {
        return ((OPoint*)obj)->ToPoint().TofPoint();
    }
    else
    {
        return fPoint();
    }
}

//=================================================
// OSize
OSize::OSize()
{

}

OSize::OSize(const Size& size, bool autodel)
    : _size(size)
{
    setAutoDelete(autodel);
}

OSize::OSize(const Size * size)
    : _size(*size)
{
}

OSize::OSize(const Size& size)
    : _size(size)
{
}

OSize::OSize(const String& value)
{
    _size = value.ToSize();
}

void OSize::SetSize(Size size)
{
    if (this != GetEmpty())
    {
        _size = size;
    }
}

bool OSize::Equals(Object* Other)
{
    OSize* pOther(dynamic_cast<OSize*>(Other));
    if (NULL == pOther)
    {
        return false;
    }
    else
    {
        return (pOther->_size == _size);
    }
}

String OSize::ToString()
{
    String str;
    str.Format(_T("%d,%d"), _size.cx, _size.cy);
    return str;
}

//=================================================
// OColor
OColor::OColor(const String& value)
    : _color(0)
{
    _color = FromString(value);
}

OColor::OColor()
    : _color(0)
{

}

OColor::OColor(Uint32 value, bool autodel)
    : _color(value)
{
    setAutoDelete(autodel);
}

OColor::OColor(Uint32 value)
    : _color(value)
{
}

void OColor::SetColor(Color color)
{
    if (!IsSealed())
    {
        _color = color;
    }
}

OColor& OColor::operator=(const Uint32& other) 
{ 
    if (!IsSealed())
    {
        _color = other;
    }
    return (*this);
}

bool OColor::Equals(Object* Other)
{
    OColor* pOther(dynamic_cast<OColor*>(Other));
    if (NULL == pOther)
    {
        return false;
    }
    else
    {
        return (pOther->_color == _color);
    }
}

Color OColor::FromString(const String& value)
{
    return Color::Parse(value);
}

Color OColor::StringAsColor(const String& clr, bool& bValid)
{
    Uint32 color = 0;
    if (ColorManager::Ins()->GetColor(clr, color))
    {
        bValid = true;
        return color;
    }
    else
    {
        String val(clr);
        val.Trim();
        if (val.Empty() || val[0] != _U('#'))
        {
            bValid = false;
            return 0;
        }
        else
        {
            bValid = true;
            return String::HexToInt32(val);
        }
    }
}

String OColor::ToString()
{
    String str;
    str.Format(_T("#%0X"), _color);
    return str;
}

//=================================================
// OPoint
OPoint::OPoint()
{

}

OPoint::OPoint(const Point& pt, bool autodel)
    : _point(pt)
{
    setAutoDelete(autodel);
}

OPoint::OPoint(const Point * pt)
    : _point(pt)
{
}

OPoint::OPoint(const Point& pt)
    : _point(pt)
{
}

OPoint::OPoint(const String& value)
{
    _point = value.ToPoint();
}

void OPoint::SetPoint(Point point)
{
    if (!IsSealed())
    {
        _point = point;
    }
}

OPoint& OPoint::operator=(const Point& other) 
{ 
    if (!IsSealed())
    {
        _point = other;
    }

    return (*this);
}

bool OPoint::Equals(Object* Other)
{
    OPoint* pOther(dynamic_cast<OPoint*>(Other));
    if (NULL == pOther)
    {
        return false;
    }
    else
    {
        return (pOther->_point == _point);
    }
}

Rect OPoint::ToRect()
{
    return Rect(0, 0, _point.x, _point.y);
}

Size OPoint::ToSize()
{
    return Size(_point.x, _point.y);
}

Point OPoint::ToPoint()
{
    return _point;
}

String OPoint::ToString()
{
    String str;
    str.Format(_T("%d,%d"), _point.x, _point.y);
    return str;
}

//=================================================
// Boolean
Boolean::Boolean(Byte val)
    : _value(val)
{
    setAutoDelete(false);
    Sealed();
}

Boolean* Boolean::FromBool(bool val)
{
    return BOOLTOBOOLEAN(val);
}

Boolean* Boolean::FromByte(Byte val)
{
    switch (val)
    {
    case -1:
        return Boolean::Nullable;

    case 1:
        return Boolean::True;

    default:
        return Boolean::False;
    }
}

Boolean* Boolean::FromString(String val)
{
    if (val.Equals(_U("x:Null")))
    {
        return Boolean::Nullable;
    }
    else if (val.Equals(_U("True")))
    {
        return Boolean::True;
    }
    else
    {
        return Boolean::False;
    }
}

bool Boolean::Equals(Object* Other)
{
    Boolean* pOther(dynamic_cast<Boolean*>(Other));
    if (NULL == pOther)
    {
        return false;
    }
    else
    {
        return (pOther->_value == _value);
    }
}

bool Boolean::ToBool() const
{
    return _value == 1;
}

Int32 Boolean::ToInt()
{
    return _value;
}

Float Boolean::ToFloat()
{
    return ToInt();
}

String Boolean::ToString()
{
    if (-1 == _value)
    {
        return _U("x:Null");
    }
    else if (1 == _value)
    {
        return _U("True");
    }
    else
    {
        return _U("False");
    }
}

Boolean* Boolean::True = new Boolean(1);
Boolean* Boolean::False = new Boolean(0);
Boolean* Boolean::Nullable = new Boolean(-1);

//=================================================
// ODouble
OFloat::OFloat(Float value)
    : _value(value)
{
}

OFloat::OFloat()
    : _value(0)
{

}

OFloat::OFloat(Float value, bool autodel)
    : _value(value)
{
    setAutoDelete(autodel);
}

OFloat::OFloat(const String& value)
{
    _value = value.ToFloat();
}

bool OFloat::Equals(Object* Other)
{
    OFloat* pOther(dynamic_cast<OFloat*>(Other));
    if (NULL == pOther)
    {
        Integer* pInte(dynamic_cast<Integer*>(Other));
        if (NULL == pInte)
        {
            return false;
        }
        else
        {
            return FloatUtil::AreClose(pInte->ToFloat(), _value);
        }
    }
    else
    {
        return FloatUtil::AreClose(pOther->_value, _value);
    }
}

void OFloat::SetFloat(Float val)
{
    if (!IsSealed())
    {
        _value = val;
    }
}

OFloat& OFloat::operator=(const Float& other) 
{ 
    if (!IsSealed())
    {
        _value = other;
    }
    return (*this);
}

String OFloat::ToString()
{
    String str;
    str.Format(_T("%g"), _value);
    return str;
}

ImageSource::~ImageSource()
{

}

ImageSource::ImageSource(const String& uri)
{
    _uri = uri;
}

ImageSource::ImageSource()
{
}

void ImageSource::SetUri(ResourceUri uri)
{
    _bmp.Clear();
    _uri = uri;
}

ResourceUri ImageSource::GetUri() const
{
    return _uri;
}

int ImageSource::Width()
{
    return GetBitmap()->Width();
}

int ImageSource::Height()
{
    return GetBitmap()->Height();
}

Bitmap* ImageSource::GetBitmap()
{
    if (!_bmp.IsLoaded())
    {
        _bmp.Load(&_uri);
    }
    return &_bmp;
}

bool Integer::_initInteger = false;
Integer* Integer::PosInteger[16];
Integer* Integer::NegInteger[16];

Integer::Integer(const String& value)
{
    if (value.Equals(_T("Auto")))
    {
        _value = -1;
    }
    else
    {
        _value = value.ToInt();
    }
}

Integer::Integer(const String& value, int iDef)
{
    if (value.Equals(_T("Auto")))
    {
        _value = iDef;
    }
    else
    {
        _value = value.ToInt();
    }
}

Integer* Integer::GetZeroInt()
{
    if (ZeroInteger == NULL)
    {
        ZeroInteger = new Integer(0, false);
        ZeroInteger->Sealed();
    }

    return ZeroInteger;
}

Integer* Integer::GetPosInt(Uint16 index)
{
    if (index == 0)
    {
        return GetZeroInt();
    }
    else if (index > 16)
    {
        return NULL;
    }
    else if (!_initInteger)
    {
        _initInteger = true;
        int i = 0;

        for (i = 0; i < 16; ++i)
        {
            PosInteger[i] = new Integer(i + 1, false);
            PosInteger[i]->ref();
            PosInteger[i]->Sealed();
        }
        for (i = 1; i <= 16; ++i)
        {
            NegInteger[i - 1] = new Integer(-i, false);
            NegInteger[i - 1]->Sealed();
            NegInteger[i - 1]->ref();
        }
    }

    return PosInteger[index - 1];
}

Integer* Integer::GetNegInt(Int16 index)
{
    if (index < -16 || index >= 0)
    {
        return NULL;
    }
    else if (!_initInteger)
    {
        _initInteger = true;
        int i = 0;

        for (i = 0; i < 16; ++i)
        {
            PosInteger[i] = new Integer(i + 1, false);
            PosInteger[i]->ref();
            PosInteger[i]->Sealed();
        }
        for (i = 1; i <= 16; ++i)
        {
            NegInteger[i - 1] = new Integer(-i, false);
            NegInteger[i - 1]->ref();
            NegInteger[i]->Sealed();
        }
    }

    return NegInteger[-index - 1];
}

Int32 Integer::ToInt()
{
    return _value;
}

Float Integer::ToFloat()
{
    return (Float)_value;
}

int Integer::GetHashCode() const
{
    return (int)_value;
}

void Integer::SetInteger(Int32 val)
{
    if (!IsSealed())
    {
        _value = val;
    }
}

Integer& Integer::operator=(const Integer& Other)
{
    if (&Other != this && !IsSealed())
    {
        _value = Other._value;
    }

    return *this;
}

bool Integer::Equals(Object* Other)
{
    Integer* pOther(dynamic_cast<Integer*>(Other));
    if (NULL == pOther)
    {
        OFloat* pDouble(dynamic_cast<OFloat*>(Other));
        if (NULL == pDouble)
        {
            return false;
        }
        else
        {
            return pDouble->ToInt() == _value;
        }
    }
    return (pOther->_value == _value);
}

String Integer::ToString()
{
    String str;
    str.Format(_T("%d"), _value);
    return str;
}

String IntegerType::ToString()
{
    String str;
    str.Format(_T("%d"), _value);
    return str;
}

}
