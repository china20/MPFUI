/////////////////////////////////////////////////////////////////////////
// uistring.cpp

#include <System/Types/String.h>
#include <System/Tools/Array.h>
#include <System/Windows/CoreTool.h>
#include <System/Types/StringCoreHelper.h>

#include <System/Interop/System.h>

#include <stdio.h>
#include <wchar.h>
#include <stdlib.h>

namespace suic
{

const Int32 String::npos = -1;

int StringArray::SplitString(const String& str, const String & delims, Uint32 maxSplits)
{
    Uint32 numSplits = 0;
    Int32 start = 0;
    Int32 pos = 0;

    do
    {
        pos = str.IndexOf(delims, start);

        if (pos == start)
        {
            start = pos + 1;
        }
        else if (pos == String::npos || (maxSplits && numSplits == maxSplits))
        {
            Add(str.Substring(start) );
            break;
        }
        else
        {
            Add(str.Substring(start, static_cast<Int32>(pos - start)));
            start = pos + 1;
        }

        ++numSplits;

    } while (pos != String::npos && start != String::npos);

    return (int)Length();
}

void utf8_to_unicode(const char *in, String& out, int len, UINT code)
{
	if (len == 0)
    {
        len = (int)strlen(in);
    }

    int wcsLen = ::MultiByteToWideChar(code, NULL, in, len, NULL, 0);
    out.Resize(wcsLen);
    ::MultiByteToWideChar(code, NULL, in, len, out.c_str(), wcsLen);

    out[wcsLen] = '\0';
}

void unicode_to_utf8(const wchar_t* in, Mulstr& out, int len, int code)
{
    if (len == 0)
    {
        len = (int)wcslen(in);
    }

    int iSize = ::WideCharToMultiByte(code, NULL, in, len, NULL, 0, NULL, NULL);

    out.Resize(iSize);
    ::WideCharToMultiByte(code, NULL, in, len, out.c_str(), iSize, NULL, NULL);

    out[iSize] = '\0';
}

static __nilStringDataW __g_defnilW = __nilStringDataW();
static __nilStringDataA __g_defnilA = __nilStringDataA();

static StringInfo * getNilStringW()
{
    __g_defnilW.__incRef();
    return (&__g_defnilW);
}

static StringInfo * getNilStringA()
{
    __g_defnilA.__incRef();
    return (&__g_defnilA);
}

///////////////////////////////////////////////////////////////////////////
//

String::String()
    : _Ptr(static_cast<Char*>(getNilStringW()->data()))
{
}

String String::FromInt(int value)
{
    Char tmp[20] = {0};
    swprintf_s(tmp, 20, L"%d", value);

    *this = String(tmp, (Int32)_tcslen(tmp));
    return (*this);
}

String String::FromFloat(Float value)
{
    Char tmp[20] = {0};
    swprintf_s(tmp, 20, L"%lf", value);
    *this = String(tmp, (int)_tcslen(tmp));
    return (*this);
}

String::String(const Char* value)
{
    StringInfo * _stNewData = __initString<Char>(value, GetCharSize(value));
    _Ptr = static_cast<Char*>(_stNewData->data());
}

String::String(const Char* value, int count)
{
    StringInfo * _stNewData = __initString<Char>(value, count);
    _Ptr = static_cast<Char*>(_stNewData->data());
}

String::String(const char* value)
    : _Ptr(static_cast<Char*>(getNilStringW()->data()))
{
    String strNew;
    utf8_to_unicode(value, strNew, strlen(value), CP_ACP);
    StringInfo * _stNewData = (reinterpret_cast<StringInfo *>(strNew._Ptr) - 1);
    _stNewData->__incRef();
    _Ptr = static_cast<Char*>(_stNewData->data());
}

String::String(const char* value, int count)
    : _Ptr(static_cast<Char*>(getNilStringW()->data()))
{
    String strNew;
    utf8_to_unicode(value, strNew, count, CP_ACP);
    StringInfo * _stNewData = (reinterpret_cast<StringInfo *>(strNew._Ptr) - 1);
    _stNewData->__incRef();
    _Ptr = static_cast<Char*>(_stNewData->data());
}

String::String(Char c, int count)
{
    StringInfo * _stNewData = __initString<Char>(c, count);
    _Ptr = static_cast<Char*>(_stNewData->data());
}

String::String(const String& other)
{
	if (&other != this)
	{
		StringInfo * _stNewData = (reinterpret_cast<StringInfo *>(other._Ptr) - 1);
        StringInfo* _stData = (reinterpret_cast<StringInfo *>(_Ptr) - 1);
		_stNewData->__incRef();
		_Ptr = static_cast<Char*>(_stNewData->data());
	}
}

String::~String()
{
    StringInfo* _stData = (reinterpret_cast<StringInfo *>(_Ptr) - 1);
    _stData->__decRef();
}

String String::FromUtf8(const char* value, int count)
{
    String strNew;
    utf8_to_unicode(value, strNew, count, CP_UTF8);
    *this = strNew;

    return (*this);
}

String String::FromChar(const char* value, int count)
{
    String strNew;
    utf8_to_unicode(value, strNew, count, CP_ACP);
    *this = strNew;

    return (*this);
}

void String::AppendUtf8(const char* value, int count)
{
    String str;
    str.FromUtf8(value, count);
    *this += str;
}

void String::AppendChar(const char* value, int count)
{
    String str;
    str.FromChar(value, count);
    *this += str;
}

void String::Assign(const Char* value, int count)
{
    if (count <= 0)
    {
        count = (int)wcslen(value);
    }
    StringInfo * _stNewData = __initString<Char>(value, count);
    StringInfo * _stData = (reinterpret_cast<StringInfo *>(_Ptr) - 1);

    _stData->__decRef();
    _Ptr = static_cast<Char*>(_stNewData->data());
}

void String::Append(const Char* value, int count)
{
    if (count <= 0)
    {
        count = (int)wcslen(value);
    }

    StringInfo * _stData = (reinterpret_cast<StringInfo *>(_Ptr) - 1);
    StringInfo * _stNewData = __AppendString<Char>(_stData, value, count);
    _Ptr = static_cast<Char*>(_stNewData->data());
}

bool Equals(const String& a, const String& b)
{
	return a.Equals(b);
}

Int32 String::GetCharSize(const Char* value)
{
    Char* data = const_cast<Char*>(value);

    while (*(data++)) {}

    return (Int32)(data - value - 1);
}

const static char* CST_HEX_U = "0123456789ABCDEF";
const static char* CST_HEX_L = "0123456789abcdef";

static Char* tohex (Char * _Buf, unsigned char ch)
{
    _Buf[0] = CST_HEX_U[(int)((ch & 0xF0) > 4)];
    _Buf[1] = CST_HEX_U[(int)(ch & 0x0F)];

    return _Buf;
}

Uint32 String::HexToInt32(const String& str)
{
    String val = str;

    if (val.Empty())
    {
        return 0;
    }

    if (val[0] == _T('#'))
    {
        val = _T("0x");
        val += str.Substring(1);
    }
    else
    {
        val = _T("0x");
        val += str;
    }

    return _tcstoul(val.c_str(), 0, 16);
}

String String::Int32ToHex(Uint32 hex)
{
    Color clr(hex);
    return Color::ToHex(clr);
}

int String::GetHashCode() const
{
    int code = 0;
    const Char* chPtr = c_str();
    int num = 0x15051505;
    int num2 = num;
    int* numPtr = (int*)chPtr;

    for (int i = Length(); i > 0; i -= 4)
    {
        num = (((num << 5) + num) + (num >> 0x1b)) ^ numPtr[0];
        if (i <= 2)
        {
            break;
        }
        num2 = (((num2 << 5) + num2) + (num2 >> 0x1b)) ^ numPtr[1];
        numPtr += 2;
    }

    code = (num + (num2 * 0x5d588b65));

    return code;

    /*int code = 0;
    for (int i = 0; i < Length(); ++i)
    {
        code += (int)(*this)[i];
    }
    return code;*/
}

int String::Length() const
{
    return (reinterpret_cast<StringInfo *>(_Ptr) - 1)->_Mysize;
}

bool String::EndsWith(const String& value) const
{
    StringInfo * _stOther = (reinterpret_cast<StringInfo *>(value._Ptr) - 1);
    StringInfo * _stSelf = (reinterpret_cast<StringInfo *>(_Ptr) - 1);

    if (_stOther->_Mysize > _stSelf->_Mysize)
    {
        return false;
    }
    else
    {
        bool bFind = true;
        int i = _stOther->_Mysize - 1;
        int j = _stSelf->_Mysize - 1;

        for (int k = i; k >= 0; --k,--j)
        {
            if (value._Ptr[k] != _Ptr[j])
            {
                bFind = false;
                break;
            }
        }

        return bFind;
    }
}

bool String::Contains(const String& value) const
{
    return IndexOf(value, 0) != -1;
}

bool String::Equals(const String& Other) const
{
    if (Other._Ptr == _Ptr)
    {
        return true;
    }

    StringInfo * _stOther = (reinterpret_cast<StringInfo *>(Other._Ptr) - 1);
    StringInfo * _stSelf = (reinterpret_cast<StringInfo *>(_Ptr) - 1);

    if (_stOther->_Mysize != _stSelf->_Mysize)
    {
        return false;
    }
    else
    {
        return ::wmemcmp(Other._Ptr, _Ptr, _stSelf->_Mysize) == 0; 
    }
}

int String::Compare(const String& Other) const
{
    return wcscmp(_Ptr, Other._Ptr);
}

int String::CompareI(const String& Other) const
{
    return wcsicmp(_Ptr, Other._Ptr);
}

bool String::Empty() const
{
	StringInfo * _stData = (reinterpret_cast<StringInfo *>(_Ptr) - 1);
	return (_stData->_Mysize == 0 ? true : false);
}

int String::ToInt() const
{
	Char * _EndPtr = 0;
    return ::wcstol(_Ptr, &_EndPtr, 10);;
}

Float String::ToFloat() const
{
    Char * _EndPtr = 0;
	return ::wcstod(_Ptr, &_EndPtr);
}

const Char* String::c_str() const
{
    return _Ptr;
}

Char* String::c_str()
{
    return _Ptr;
}

bool String::operator <(const String& Other)
{
	return (Compare(Other) < 0 ? true : false);
}

bool String::operator >(const String& Other)
{
    return (Compare(Other) > 0 ? true : false);
}

bool String::operator ==(const String& Other)
{
    return Equals(Other);
}

String& String::operator=(const String& Other)
{
	if (&Other != this)
	{
		StringInfo * _stNewData = (reinterpret_cast<StringInfo *>(Other._Ptr) - 1);
		StringInfo * _stData = (reinterpret_cast<StringInfo *>(_Ptr) - 1);

		_stData->__decRef();
		_stNewData->__incRef();
		_Ptr = static_cast<Char*>(_stNewData->data());
	}

    return *this;
}

String& String::operator+=(const String& Other)
{
    Append(Other.c_str(), Other.Length());
    return *this;
}

bool operator<(const String& first, const String& second)
{
    return (first.Compare(second) < 0 ? true : false);
}

bool operator>(const String& first, const String& second)
{
    return (first.Compare(second) > 0 ? true : false);
}

bool operator==(const String& first, const String& second)
{
    return first.Equals(second);
}

String operator+(const String& first, const String& second)
{
    String ret = first;
    ret += second;
    return ret;
}

Int32 String::IndexOf(Char ch, int startIndex) const
{
	for (int i = startIndex; i < Length(); ++i)
    {
        if ((*this)[i] == ch)
        {
            return i;
        }
    }

    return String::npos;
}

Int32 String::IndexOf(const String& value, int startIndex) const
{
	return IndexOf(value, startIndex, value.Length());
}

Int32 String::IndexOf(const String& value) const
{
    return IndexOf(value, 0);
}

Int32 String::IndexOf(Char value, int startIndex, int count) const
{
    Char * result = ::wmemchr(_Ptr + startIndex, value, count);

	if (NULL == result)
	{
		return String::npos;
	}
	else
	{
		return (Int32)(result - _Ptr);
	}
}

static const Char * find(const Char *_First, Int32 _Count, const Char& _Ch)
{
	return ((const Char *)::wmemchr(_First, _Ch, _Count));
}

static const char * findA(const char *_First, Int32 _Count, const char& _Ch)
{
    return ((const char *)::memchr(_First, _Ch, _Count));
}

Int32 String::IndexOf(const String& value, int startIndex, int count) const
{
	StringInfo * _stData = (reinterpret_cast<StringInfo *>(_Ptr) - 1);

	if (count == 0 && startIndex <= _stData->_Mysize)
	{
		return (startIndex);	// null string always matches (if inside string)
	}

	Int32 _Nm;

	if (startIndex < _stData->_Mysize && count <= (_Nm = _stData->_Mysize - startIndex))
	{	
		const Char * _Uptr = NULL;
		const Char *_Vptr = NULL;

		for (_Nm -= count - 1, _Vptr = _Ptr + startIndex;
			(_Uptr = find(_Vptr, _Nm, *(value._Ptr))) != 0;
			_Nm -= (Int32)(_Uptr - _Vptr + 1), _Vptr = _Uptr + 1)
		{
			if (wmemcmp(_Uptr, value._Ptr, count) == 0)
			{
				//
				// ƥ�䵽
				//
				return static_cast<Int32>(_Uptr - _Ptr);
			}
		}
	}

	return (String::npos);
}

String String::Remove(int startIndex)
{
    String str = Substring(0, startIndex);
    *this = str;
    return str;
}

String String::Remove(int startIndex, int count)
{
    String str = Substring(0, startIndex);

    if (startIndex + count < Length())
    {
        str += Substring(startIndex + count);
    }

    *this = str;

    return str;
}

String String::Replace(Char oldChar, Char newChar)
{
    for (int i = 0; i < Length(); ++i)
    {
        if (_Ptr[i] == oldChar)
        {
            _Ptr[i] = newChar;
        }
    }

    return *this;
}

String String::Replace(const String& oldValue, const String& newValue)
{
    String strPos = *this;

    Int32 iStartPos = 0;
    Int32 iPos = IndexOf(oldValue, 0);

    while (iPos != String::npos)
    {
        String str = strPos.Substring(0, iPos);
        str += newValue;
        str += strPos.Substring(iPos + oldValue.Length());
        
        iStartPos = iPos + newValue.Length();
        strPos = str;

        iPos = strPos.IndexOf(oldValue, iStartPos);
    }

    *this = strPos;
    
    return *this;
}

String String::Insert(int startIndex, const String& insertValue)
{
    if (startIndex >= Length())
    {
        *this += insertValue;
    }
    else if (startIndex >= 0)
    {
        String str = Substring(0, startIndex);

        str += insertValue;
        str += Substring(startIndex);

        *this = str;
    }

    return (*this);
}

String String::Substring(int startIndex) const
{
    if (startIndex >= Length())
    {
        return String();
    }
    else
    {
        return String(_Ptr + startIndex, Length() - startIndex);
    }
}

String String::Substring(int startIndex, int length) const
{
    if (startIndex >= Length())
    {
        return String();
    }
    else
    {
        return String(_Ptr + startIndex, min(length, Length() - startIndex));
    }
}

String String::ToLower()
{
    for (int i = 0; i < Length(); ++i)
    {
        _Ptr[i] = _totlower(_Ptr[i]);
    }

    return (*this);
}

String String::ToUpper()
{
    for (int i = 0; i < Length(); ++i)
    {
        _Ptr[i] = _totupper(_Ptr[i]);
    }

    return (*this);
}

String String::Trim()
{
    int i = 0;
    int beg = 0;
    int end = Length() - 1;

    for (i = 0; i < Length(); ++i)
    {
        if (_Ptr[i] != _T(' '))
        {
            beg = i;
            break;
        }
    }

    for (i = Length() - 1; i >= beg; --i)
    {
        if (_Ptr[i] != _T(' '))
        {
            end = i;
            break;
        }
    }

    if (beg > 0 || end < Length() - 1)
    {
        String str = Substring(beg, end - beg + 1);
        *this = str;
    }

    return (*this);
}

bool String::IsDigital() const
{
    bool bRet = true;
    for (int i = 0; i < Length(); ++i)
    {
        Char ch = *(_Ptr + i);
        if (ch < _T('0') || ch > _T('9'))
        {
            bRet = false;
            break;
        }
    }
    return bRet;
}

bool String::IsFloat() const
{
    return false;
    /*if (Length() < 1)
    {
        return false;
    }

    Char ch = *(_Ptr);
    int i = 1;

    if (ch != _T('-') && (ch < _T('0') || ch > _T('9')))
    {
        return false;
    }

    bool bLine = false;
    bool bRet = true;
    for (int i = 0; i < Length(); ++i)
    {
        Char ch = *(_Ptr + i);
        if (ch < _T('0') || ch > _T('9'))
        {
            bRet = false;
            break;
        }
    }
    return bRet;*/
}

void String::Resize(int size)
{
    StringInfo * _stSelfData = (reinterpret_cast<StringInfo *>(_Ptr) - 1);
    _stSelfData->__decRef();

    StringInfo * _stNewData = __allocString<Char>(size);
    __setBaseString<Char>(_stNewData, size);
    _Ptr = static_cast<Char*>(_stNewData->data());
}

void String::Clear()
{
    StringInfo * _stData = (reinterpret_cast<StringInfo *>(_Ptr) - 1);
    _stData->__decRef();
    _Ptr = static_cast<Char*>(getNilStringW()->data());
}

String String::Format(const Char * szFormat, ...)
{
    va_list argList;
    va_start(argList, szFormat);
    int len = _vsctprintf(szFormat, argList);

    Resize(len);
    _vstprintf_s(_Ptr, len + 1, szFormat, argList);
    va_end(argList);

    return (*this);
}

Char& String::operator[](int index)
{
    return (*(_Ptr + index));
}

const Char& String::operator[](int index) const
{
    return (*(_Ptr + index));
}

//------------------------------------------------------\\

ImplementRTTIOfClassName(OString, OType, String)

OString* OString::EmptyString = new OString();

OString::OString()
{
}

OString::~OString()
{
}

OString::OString(const String& val, bool autodel)
    : _value(val)
{
    setAutoDelete(autodel);
}

OString::OString(const String& value)
    : _value(value)
{
}

OString::OString(const OString& Other)
{
    _value = Other._value;
}

OString::OString(Int32 value)
{
    _value = String().FromInt(value);
}

OString* OString::Clone() const
{
    OString* oStr(new OString());
    oStr->_value = _value;
    return oStr;
}

bool OString::Equals(Object* Other)
{
    OString* oStr(DynamicCast<OString>(Other));
    if (!oStr || !oStr->_value.Equals(_value))
    {
        return false;
    }
    else
    {
        return true;
    }
}

void OString::SetString(const String& str)
{
    if (!IsSealed())
    {
        _value = str;
    }
    else
    {
        ;
    }
}

String OString::ToString()
{
    return _value;
}

Float OString::ToFloat()
{
    return _value.ToFloat();
}

Int32 OString::ToInt()
{
    return _value.ToInt();
}

bool String::ToBool() const
{
    return (Equals(_T("True")));
}

Rect String::ToRect() const
{
    Rect value;
    StringArray vec;

    vec.SplitString(*this, _T(","));

    switch (vec.Length())
    {
    case 1:
        value.left = vec[0].ToInt();
        value.top = value.left;
        value.right = value.left;
        value.bottom = value.left;
        break;
    case 2:
        value.left = vec[0].ToInt();
        value.top = vec[1].ToInt();
        value.right = value.left;
        value.bottom = value.top;
        break;
    case 4:
        value.left = vec[0].ToInt();
        value.top = vec[1].ToInt();
        value.right = vec[2].ToInt();
        value.bottom = vec[3].ToInt();
        break;
    }

    return value;
}

fRect String::TofRect() const
{
    fRect value;
    StringArray vec;

    vec.SplitString(*this, _T(","));

    switch (vec.Length())
    {
    case 1:
        value.left = vec[0].ToFloat();
        value.top = value.left;
        value.right = value.left;
        value.bottom = value.left;
        break;
    case 2:
        value.left = vec[0].ToFloat();
        value.top = vec[1].ToFloat();
        value.right = value.left;
        value.bottom = value.top;
        break;
    case 4:
        value.left = vec[0].ToFloat();
        value.top = vec[1].ToFloat();
        value.right = vec[2].ToFloat();
        value.bottom = vec[3].ToFloat();
        break;
    }

    return value;
}

Size String::ToSize() const
{
    Size value;
    StringArray vec;

    vec.SplitString(*this, _T(","));

    switch (vec.Length())
    {
    case 1:
        value.cx = vec[0].ToInt();
        value.cy = value.cx;
        break;
    case 2:
        value.cx = vec[0].ToInt();
        value.cy = vec[1].ToInt();
        break;
    }

    return value;
}

fSize String::TofSize() const
{
    fSize value;
    StringArray vec;

    vec.SplitString(*this, _T(","));

    switch (vec.Length())
    {
    case 1:
        value.cx = vec[0].ToFloat();
        value.cy = value.cx;
        break;
    case 2:
        value.cx = vec[0].ToFloat();
        value.cy = vec[1].ToFloat();
        break;
    }

    return value;
}

Point String::ToPoint() const
{
    Size size = ToSize();
    return Point(size.cx, size.cy);
}

fPoint String::TofPoint() const
{
    fSize size = TofSize();
    return fPoint(size.cx, size.cy);
}

Color String::ToColor() const
{
    return Color::Parse(*this);
}

bool OString::ToBool() const
{
    return (_value.Equals(_T("True")));
}

Rect OString::ToRect()
{
    return _value.ToRect();
}

fRect OString::TofRect()
{
    return _value.TofRect();
}

Size OString::ToSize()
{
    return _value.ToSize();
}

fSize OString::TofSize()
{
    return _value.TofSize();
}

Point OString::ToPoint()
{
    Size size = ToSize();
    return Point(size.cx, size.cy);
}

fPoint OString::TofPoint()
{
    fSize size = TofSize();
    return fPoint(size.cx, size.cy);
}

Color OString::ToColor()
{
    return Color::Parse(_value);
}

//------------------------------------------------------\\

Mulstr::Mulstr()
    : _Ptr(static_cast<char*>(getNilStringA()->data()))
{
}

Mulstr::~Mulstr()
{
    StringInfo* _stData = (reinterpret_cast<StringInfo *>(_Ptr) - 1);
    _stData->__decRef();
}

int Mulstr::ToInt() const
{
    char * _EndPtr = 0;
    return strtol(_Ptr, &_EndPtr, 10);
}

Float Mulstr::ToDouble() const
{
    char * _EndPtr = 0;
    return strtod(_Ptr, &_EndPtr);
}

Mulstr Mulstr::FromInt(int value)
{
    char tmp[20] = {0};
    sprintf_s(tmp, "%d", value);
    *this = Mulstr(tmp);
    return (*this);
}

Mulstr Mulstr::FromDouble(Float value)
{
    char tmp[20] = {0};
    sprintf_s(tmp, 20, "%lf", value);
    *this = Mulstr(tmp);
    return (*this);
}

Mulstr::Mulstr(const char* value, int count)
{
    StringInfo * _stNewData = __initString<char>(value, count);
    _Ptr = static_cast<char*>(_stNewData->data());
}

Mulstr::Mulstr(const Char* value)
    : _Ptr(static_cast<char*>(getNilStringA()->data()))
{
    Mulstr str;
    str.FromUStr(value, 0);

    StringInfo * _stOther = (reinterpret_cast<StringInfo *>(str._Ptr) - 1);
    _stOther->__incRef();
    _Ptr = static_cast<char*>(_stOther->data());
}

Mulstr::Mulstr(const char* value)
{
    StringInfo * _stNewData = __initString<char>(value, (int)strlen(value));
    _Ptr = static_cast<char*>(_stNewData->data());
}

Mulstr::Mulstr(const Mulstr& other)
{
    StringInfo * _stOther = (reinterpret_cast<StringInfo *>(other._Ptr) - 1);
    _stOther->__incRef();
    _Ptr = static_cast<char*>(_stOther->data());
}

int Mulstr::Compare(const Mulstr& Other) const
{
    return strcmp(_Ptr, Other._Ptr);
}

bool Mulstr::Equals(const Mulstr& Other) const
{
    if (Other._Ptr == _Ptr)
    {
        return true;
    }

    StringInfo * _stOther = (reinterpret_cast<StringInfo *>(Other._Ptr) - 1);
    StringInfo * _stSelf = (reinterpret_cast<StringInfo *>(_Ptr) - 1);

    if (_stOther->_Mysize != _stSelf->_Mysize)
    {
        return false;
    }
    else
    {
        return ::memcmp(Other._Ptr, _Ptr, _stSelf->_Mysize) == 0; 
    }
}

bool Mulstr::operator <(const Mulstr& Other)
{
    return Compare(Other) < 0;
}

bool Mulstr::operator >(const Mulstr& Other)
{
    return Compare(Other) > 0;
}

bool Mulstr::operator ==(const Mulstr& Other)
{
    return Equals(Other);
}

Mulstr& Mulstr::operator=(const Mulstr& Other)
{
    if (&Other != this)
    {
        StringInfo * _stNewData = (reinterpret_cast<StringInfo *>(Other._Ptr) - 1);
        StringInfo * _stData = (reinterpret_cast<StringInfo *>(_Ptr) - 1);

        _stData->__decRef();
        _stNewData->__incRef();
        _Ptr = static_cast<char*>(_stNewData->data());
    }

    return (*this);
}

Mulstr& Mulstr::operator+=(const Mulstr& Other)
{
    Append(Other.c_str(), Other.Length());
    return (*this);
}

int Mulstr::Length() const
{
    return (reinterpret_cast<StringInfo *>(_Ptr) - 1)->_Mysize;
}

bool Mulstr::Empty() const
{
    StringInfo * _stData = (reinterpret_cast<StringInfo *>(_Ptr) - 1);
    return (_stData->_Mysize == 0 ? true : false);
}

const char* Mulstr::c_str() const
{
    return _Ptr;
}

char* Mulstr::c_str()
{
    return _Ptr;
}

Mulstr Mulstr::FromUtf8(const char* value, int count)
{
    String val;
    val.FromUtf8(value, count);
    FromUStr(val.c_str(), val.Length());
    return (*this);
}

void Mulstr::FromUStr(const Char* value, int count)
{
    unicode_to_utf8(value, *this, count, CP_ACP);
}

Mulstr Mulstr::Substring(int startIndex) const
{
    return Mulstr(_Ptr + startIndex, Length() - startIndex);
}

Mulstr Mulstr::Substring(int startIndex, int length) const
{
    return Mulstr(_Ptr + startIndex, min(length, Length() - startIndex));
}

Int32 Mulstr::IndexOf(const Mulstr& value, int startIndex) const
{
    return IndexOf(value, startIndex, value.Length());
}

Int32 Mulstr::IndexOf(const Mulstr& value, int startIndex, int count) const
{
    StringInfo * _stData = (reinterpret_cast<StringInfo *>(_Ptr) - 1);

    if (count == 0 && startIndex <= _stData->_Mysize)
    {
        return (startIndex);
    }

    Int32 _Nm;
    if (startIndex < _stData->_Mysize && count <= (_Nm = _stData->_Mysize - startIndex))
    {	
        const char * _Uptr = NULL;
        const char *_Vptr = NULL;

        for (_Nm -= count - 1, _Vptr = _Ptr + startIndex;
            (_Uptr = findA(_Vptr, _Nm, *(value._Ptr))) != 0;
            _Nm -= (Int32)(_Uptr - _Vptr + 1), _Vptr = _Uptr + 1)
        {
            if (memcmp(_Uptr, value._Ptr, count) == 0)
            {
                return static_cast<Int32>(_Uptr - _Ptr);
            }
        }
    }

    return (String::npos);
}

Mulstr Mulstr::Insert(int startIndex, const Mulstr& insertValue)
{
    if (startIndex >= Length())
    {
        *this += insertValue;
    }
    else if (startIndex >= 0)
    {
        Mulstr str = Substring(0, startIndex);

        str += insertValue;
        str += Substring(startIndex);

        *this = str;
    }

    return (*this);
}

Mulstr Mulstr::Replace(char oldChar, char newChar)
{
    for (int i = 0; i < Length(); ++i)
    {
        if (_Ptr[i] == oldChar)
        {
            _Ptr[i] = newChar;
        }
    }

    return *this;
}

Mulstr Mulstr::Replace(const Mulstr& oldValue, const Mulstr& newValue)
{
    Mulstr strPos = *this;

    Int32 iStartPos = 0;
    Int32 iPos = IndexOf(oldValue, 0);

    while (iPos != String::npos)
    {
        Mulstr str = strPos.Substring(0, iPos);
        str += newValue;
        str += strPos.Substring(iPos + oldValue.Length());

        iStartPos = iPos + newValue.Length();
        strPos = str;

        iPos = strPos.IndexOf(oldValue, iStartPos);
    }

    *this = strPos;

    return *this;
}

void Mulstr::Resize(int size)
{
    StringInfo * _stSelfData = (reinterpret_cast<StringInfo *>(_Ptr) - 1);
    _stSelfData->__decRef();

    StringInfo * _stNewData = __allocString<char>(size);
    __setBaseString<char>(_stNewData, size);
    _Ptr = static_cast<char*>(_stNewData->data());
}

void Mulstr::Clear()
{
    *this = Mulstr();
}

void Mulstr::Assign(const char* value, int count)
{
    if (count <= 0)
    {
        count = (int)strlen(value);
    }
    StringInfo * _stNewData = __initString<char>(value, count);
    _Ptr = static_cast<char*>(_stNewData->data());
}

void Mulstr::Append(const char* value, int count)
{
    if (count <= 0)
    {
        count = (int)strlen(value);
    }

    StringInfo * _stData = (reinterpret_cast<StringInfo *>(_Ptr) - 1);
    StringInfo * _stNewData = __AppendString<char>(_stData, value, count);
    _Ptr = static_cast<char*>(_stNewData->data());
}

Mulstr Mulstr::EncodeUtf8()
{
    String tmp(this->c_str());
    unicode_to_utf8(tmp.c_str(), *this, tmp.Length(), CP_UTF8);
    return (*this);
}

char& Mulstr::operator[](int index)
{
    return *(_Ptr + index);
}

const char& Mulstr::operator[](int index) const
{
    return *(_Ptr + index);
}

//==================================================
// UtfStr

}
