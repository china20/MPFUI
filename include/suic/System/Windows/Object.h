// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����UIObjecth
// ��  �ܣ�������Ŀ�Ļ��������еĶ��󶼱���Ӵ˼̳С�
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2010-07-02
// 
// ============================================================================

#ifndef _UIOBJECT_H_
#define _UIOBJECT_H_

#include <System/Types/Micro.h>
#include <System/Types/Colors.h>
#include <System/Types/ValueBox.h>
#include <System/Types/StringCore.h>
#include <System/Windows/Shared.h>
#include <System/Windows/WeakRef.h>
#include <System/Windows/Environment.h>

namespace suic
{

class Object;
typedef shared<Object> ObjectPtr;
typedef autoshared<Object> AutoObj;

struct SUICORE_API RTTIOfInfo
{
    Object* Create() { return (*typeCreate)(); }
    RTTIOfInfo* BaseType() { return (*baseType)(); }

    bool Equals(RTTIOfInfo* Other);
    bool InheritFrom(RTTIOfInfo* baseType);
    bool IsInstanceOfType(Object* Other);
    int GetHashCode() { return classIndex; }

    Uint16 classIndex;
    const Char* typeName;
    RTTIOfInfo* (__stdcall* baseType)();
    Object* (__stdcall* typeCreate)();

    static Uint16 classCount;
};

typedef void (Object::*FnMemberCall)(void);
typedef void (Object::*FnEventCall)(void);

struct MemberEntry
{
    Byte type;
    String name;
    FnMemberCall setfunc;
    FnMemberCall getfunc;
};

struct MemberInfo
{
    const MemberInfo* (__stdcall *fnGetMemberInfo)();
    const MemberEntry* fcall;
};

template<typename T>
inline T* RTTICast(suic::Object* obj)
{
    if (NULL == obj || !obj->GetRTTIType()->InheritFrom(T::RTTIType()))
    {
        return NULL;
    }
    else
    {
        return (T*)(obj);
    }
}

template<typename T>
inline T* DynamicCast(suic::Object* obj)
{
    return dynamic_cast<T*>(obj);
}

template<typename T>
inline T* RTTIEqualCast(suic::Object* obj)
{
    if (obj && obj->GetRTTIType() == T::RTTIType())
    {
        return (T*)(obj);
    }
    else
    {
        return NULL;
    }
}

/// <summary>
/// ����ģ����ռλ����ʵ������
/// </summary>
class SUICORE_API HoldObj
{
public:

    HoldObj() {}
};

/// <summary>
/// ��ܽ���������Ļ��࣬����new��delete��ʵ���ڴ����
/// </summary>
class SUICORE_API Object
{
public:

    Object();
    Object(bool autodel);

    virtual ~Object();

    /// <summary>
    ///  ���������һ�ݶ���
    /// </summary>
    /// <returns>Clone�¶���</returns> 
    virtual Object* Clone();
    
    virtual Int32 ToInt();
    virtual bool ToBool() const;
    virtual Float ToFloat();
    virtual double ToDouble();
    virtual String ToString();

    virtual int GetHashCode();
    virtual bool Equals(Object* Other);

    static void* operator new(size_t size);
    static void* operator new(size_t size, void* ptr);

    static void operator delete(void* ptr);
    static void operator delete(void*, void*);

    static void* operator new[](size_t size);
    static void operator delete[](void* ptr);

#ifdef _DEBUG
    static void* operator new(size_t nSize, const char* filename, int line);
    static void operator delete(void* p, const char* filename, int line);
#endif

public:

    static RTTIOfInfo typeObject;
    static RTTIOfInfo* __stdcall RTTIType();
    static Object* __stdcall Create();
    static const suic::MemberInfo* __stdcall ClassMemberInfo();

    bool InheritFrom(Object* other);

    void ref();
    void unref();
    int getRefCount();
    bool isAutoDelete(); 
    bool setAutoDelete(bool val);

    void sealAutoDelete();

    virtual RTTIOfInfo* GetRTTIType() const;
    virtual void Dispose();
    virtual const MemberInfo* GetMemberInfo();

protected:

    bool SetMember(const String& name, Object* val);
    bool GetMember(const String& name, ObjectPtr& retVal);

    friend class MemberVisitor;

private:

    int _count;
    Byte _flag;
    WeakHolder* _holder;
    friend class WeakRef;
    friend class WeakHolder;
};

class SUICORE_API UnrefObj : public Object
{
public:

    UnrefObj();
    virtual ~UnrefObj();
};

class SUICORE_API Handler : public suic::Object
{
public:

    RTTIOfAbstractClass(Handler)

    Handler() {}

    virtual void AddEvent(Object* t, FnEventCall fcall) = 0;
};

}

#endif
