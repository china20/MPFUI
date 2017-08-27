// 华勤科技版权所有 2008-2022
// 
// 文件名：integer.h
// 功  能：定义核心库的整型对象。
// 
// 作  者：MPF开发组
// 时  间：2011-07-02
// 
// ============================================================================

#ifndef _UIINTEGER_H_
#define _UIINTEGER_H_

#include <System/Windows/Object.h>
#include <System/Types/String.h>

namespace suic 
{

class SUICORE_API Integer : public OType
{
public:

    static Integer* GetZeroInt();
    static Integer* GetPosInt(Uint16 index);
    static Integer* GetNegInt(Int16 index);

    RTTIOfClass(Integer)

    Integer() : _value(0) {}
    Integer(Int32 value, bool autodel) : _value(value) 
    { 
        setAutoDelete(autodel); 
    }

    Integer(Int32 value) : _value(value) {}
    Integer(const String& value);
    Integer(const String& value, Int32 iDef);

    Int32 ToInt();
    Float ToFloat();
    String ToString();

    bool Equals(Object* Other);
    int GetHashCode() const;

    void SetInteger(Int32 val);
    Integer& operator=(const Integer& Other);

    operator Int32() { return _value; }
    bool operator<(const Integer& Other) { return _value < Other._value; }
    bool operator==(const Integer& Other) { return _value == Other._value; }
    bool operator!=(const Integer& Other) { return _value != Other._value; }
    bool operator>(const Integer& Other) { return _value > Other._value; }

protected:

    static bool _initInteger;
    static Integer* ZeroInteger;
    static Integer* PosInteger[16];
    static Integer* NegInteger[16];

protected:

    Int32 _value;
};

class SUICORE_API IntegerType : public Object
{
public:

    IntegerType() 
        : _value(0)
        , Object(false) 
    {}

    IntegerType(int value) 
        : _value(value)
        , Object(false) 
    {}

    int GetHashCode() const { return (int)_value;}
    Int32 ToInt() { return _value; }
    String ToString();

    bool operator==(const IntegerType& Other)
    {
        return _value == Other._value;
    }

    operator Int32() { return _value; }

protected:

    Int32 _value;
};

}

#endif
