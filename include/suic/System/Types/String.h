// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����UIString.h
// ��  �ܣ�������Ŀ��String���͡�
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2010-07-02
// 
// ============================================================================

#ifndef _UISTRING_H_
#define _UISTRING_H_

#include <System/Windows/Object.h>
#include <System/Types/StringCore.h>

namespace suic
{

class SUICORE_API OType : public Object
{
public:

    RTTIOfClass(OType)

    OType();

    void Sealed();
    bool IsSealed() const;

private:

    Byte _sealed;
};

class SUICORE_API OString : public OType
{
public:

    static OString* EmptyString;

    RTTIOfClass(OString)

    OString();
    OString(const String& val, bool autodel);
    OString(const String& value);
    OString(const OString& Other);
    OString(Int32 value);
    ~OString();

    OString* Clone() const;
    bool Equals(Object* Other);

    String ToString();
    Float ToFloat();
    Int32 ToInt();
    bool ToBool() const;
    Rect ToRect();
    fRect TofRect();
    Size ToSize();
    fSize TofSize();
    Point ToPoint();
    fPoint TofPoint();

    Color ToColor();

    void SetString(const String& str);

    String operator*() { return _value; }
    operator String() { return _value; }

protected:

    String _value;
};

}

#endif
