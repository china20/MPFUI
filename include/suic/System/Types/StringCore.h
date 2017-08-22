// 华勤科技版权所有 2010-2011
// 
// 文件名：uistringcore.h>
// 功  能：定义核心库的字符串对象。
// 
// 作  者：MPF开发组
// 时  间：2010-07-02
// 
// ============================================================================

#ifndef _STRINGCORE_H_
#define _STRINGCORE_H_

#include <System/Types/Types.h>
#include <System/Types/Colors.h>

namespace suic
{

/// <summary>
/// 字符串类的封装.
/// </summary>
class SUICORE_API String
{
public:

    static const Int32 npos;

    String();

    String(const char* value);
    String(const Char* value);
    String(const Char* value, int count);
    String(const char* value, int count);
    String(Char c, int count);
    String(const String& other);

    ~String();

    static bool Equals(const String& a, const String& b);
    static Int32 GetCharSize(const Char* value);
    static Uint32 HexToInt32(const String& str);
    static String Int32ToHex(Uint32 hex);

    int GetHashCode() const;
    int Length() const;

    bool EndsWith(const String& value) const;
    bool Contains(const String& value) const;
    bool Equals(const String& value) const;

    int Compare(const String& Other) const;
    int CompareI(const String& Other) const;

    bool Empty() const;

    String FromInt(int value);
    String FromFloat(Float value);

    int ToInt() const;
    Float ToFloat() const;
    bool ToBool() const;
    Rect ToRect() const;
    fRect TofRect() const;
    Size ToSize() const;
    fSize TofSize() const;
    Point ToPoint() const;
    fPoint TofPoint() const;
    Color ToColor() const;

    const Char* c_str() const;
    Char* c_str();

    //------------------------------------ UTF8支持
    String FromUtf8(const char* value, int count);
    String FromChar(const char* value, int count);

    void AppendUtf8(const char* value, int count);
    void AppendChar(const char* value, int count);

    void Assign(const Char* value, int count);
    void Append(const Char* value, int count);

    bool operator <(const String& Other);
    bool operator >(const String& Other);
    bool operator ==(const String& Other);

    String& operator=(const String& Other);
    String& operator+=(const String& Other);

    SUICORE_API friend bool operator<(const String& first, const String& second);
    SUICORE_API friend bool operator>(const String& first, const String& second);
    SUICORE_API friend bool operator==(const String& first, const String& second);
    SUICORE_API friend String operator+(const String& first, const String& second);

    Int32 IndexOf(Char value, int startIndex) const;
    Int32 IndexOf(const String& value) const;
    Int32 IndexOf(const String& value, int startIndex) const;
    Int32 IndexOf(Char value, int startIndex, int count) const;
    Int32 IndexOf(const String& value, int startIndex, int count) const;

    String Remove(int startIndex);
    String Remove(int startIndex, int count);
    String Replace(Char oldChar, Char newChar);
    String Replace(const String& oldValue, const String& newValue);

    String Insert(int startIndex, const String& insertValue);

    String Substring(int startIndex) const;
    String Substring(int startIndex, int length) const;

    String ToLower();
    String ToString();
    String ToUpper();
    String Trim();

    //--------------------------------------------------
    bool IsDigital() const;
    bool IsFloat() const;

    void Resize(int size);
    void Clear();

    String Format(const Char * szFormat, ...);

    Char& operator[](int index);
	const Char& operator[](int index) const;

protected:

    Char* _Ptr;
};

/// <summary>
/// 多字节字符串类的封装.
/// </summary>
class SUICORE_API Mulstr
{
public:

    Mulstr();

    Mulstr(const Char* value);
    Mulstr(const char* value);
    Mulstr(const char* value, int count);
    Mulstr(const Mulstr& other);

    ~Mulstr();

    int Compare(const Mulstr& Other) const;
    bool Equals(const Mulstr& Other) const;

    bool operator <(const Mulstr& Other);
    bool operator >(const Mulstr& Other);
    bool operator ==(const Mulstr& Other);

    Mulstr& operator=(const Mulstr& Other);
    Mulstr& operator+=(const Mulstr& Other);

    int Length() const;
    bool Empty() const;

    int ToInt() const;
    Float ToDouble() const;

    Mulstr FromInt(int value);
    Mulstr FromDouble(Float value);

    const char* c_str() const;
    char* c_str();

    Mulstr FromUtf8(const char* value, int count);
    void FromUStr(const Char* value, int count);

    Mulstr Substring(int startIndex) const;
    Mulstr Substring(int startIndex, int length) const;

    Int32 IndexOf(const Mulstr& value, int startIndex) const;
    Int32 IndexOf(const Mulstr& value, int startIndex, int count) const;

    Mulstr Insert(int startIndex, const Mulstr& insertValue);

    Mulstr Replace(char oldChar, char newChar);
    Mulstr Replace(const Mulstr& oldValue, const Mulstr& newValue);

    void Assign(const char* value, int count);
    void Append(const char* value, int count);

    Mulstr EncodeUtf8();

    void Resize(int size);
    void Clear();

    char& operator[](int index);
    const char& operator[](int index) const;

protected:

    char* _Ptr;
};

}

#endif
