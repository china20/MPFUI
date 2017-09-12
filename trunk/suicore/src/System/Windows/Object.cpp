/////////////////////////////////////////////////////////////////////////
// Object.cpp

#include <System/Tools/Memory.h>
#include <System/Tools/SSE.h>
#include <System/Windows/Object.h>
#include <System/Windows/CoreTool.h>

#include <System/Types/Integer.h>
#include <System/Types/Numeric.h>
#include <Float.h>

#ifdef _DEBUG
//#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

namespace suic
{

Uint16 RTTIOfInfo::classCount = 0;
IntPtr IntPtr::Zero;

static int alloc_count = 0;
static int free_count = 0;

static int allocobj_count = 0;
static int freeobj_count = 0;

bool fRect::IsZero() const
{
    return (FloatUtil::IsZero(left) && 
        FloatUtil::IsZero(top) && 
        FloatUtil::IsZero(right) && 
        FloatUtil::IsZero(bottom));
}

bool fRect::operator==(const fRect& Other) const
{
    return (FloatUtil::AreClose(left, Other.left) 
        && FloatUtil::AreClose(top, Other.top)
        && FloatUtil::AreClose(right, Other.right) 
        && FloatUtil::AreClose(bottom, Other.bottom));
}

bool fRect::operator!=(const fRect& Other) const
{
    return (!FloatUtil::AreClose(left, Other.left) 
        || !FloatUtil::AreClose(top, Other.top)
        || !FloatUtil::AreClose(right, Other.right) 
        || !FloatUtil::AreClose(bottom, Other.bottom));
}

bool fSize::operator!=(const fSize& Other) const
{
    return !FloatUtil::AreClose(cx, Other.cx) ||
        !FloatUtil::AreClose(cy, Other.cy);
}

bool fSize::operator==(const fSize& Other) const
{
    return FloatUtil::AreClose(cx, Other.cx) && 
        FloatUtil::AreClose(cy, Other.cy);
}

bool fPoint::operator==(const fPoint& Other) const
{
    return FloatUtil::AreClose(x, Other.x) && 
        FloatUtil::AreClose(y, Other.y);
}

bool fPoint::operator!=(const fPoint& Other) const
{
    return !FloatUtil::AreClose(x, Other.x) || 
        !FloatUtil::AreClose(y, Other.y);
}

//----------------------------------------------------

Integer* DockBox::LeftBox = Integer::GetPosInt(0);
Integer* DockBox::TopBox = Integer::GetPosInt(1);
Integer* DockBox::RightBox = Integer::GetPosInt(2);
Integer* DockBox::BottomBox = Integer::GetPosInt(3);

Integer* DockBox::From(int index)
{
    switch (index)
    {
    case 0:
        return DockBox::LeftBox;
    case 1:
        return DockBox::TopBox;
    case 2:
        return DockBox::RightBox;
    case 3:
        return DockBox::BottomBox;
    default:
        return NULL;
    }
}

Integer* VisibilityBox::CollapsedBox = Integer::GetPosInt(2);
Integer* VisibilityBox::HiddenBox = Integer::GetPosInt(1);
Integer* VisibilityBox::VisibleBox = Integer::GetPosInt(0);

Integer* VisibilityBox::From(int index)
{
    switch (index)
    {
    case 2:
        return VisibilityBox::CollapsedBox;
    case 1:
        return VisibilityBox::HiddenBox;
    case 0:
        return VisibilityBox::VisibleBox;
    default:
        return NULL;
    }
}

Integer* OrientationBox::VerticalBox = Integer::GetPosInt(1);
Integer* OrientationBox::HorizontalBox = Integer::GetPosInt(0);

Integer* OrientationBox::From(int index)
{
    switch (index)
    {
    case 0:
        return OrientationBox::HorizontalBox;
    case 1:
        return OrientationBox::VerticalBox;
    default:
        return NULL;
    }
}

Integer* HoriAlignBox::LeftBox = Integer::GetPosInt(0);
Integer* HoriAlignBox::RightBox = Integer::GetPosInt(1);
Integer* HoriAlignBox::CenterBox = Integer::GetPosInt(2);
Integer* HoriAlignBox::StretchBox = Integer::GetPosInt(3);

Integer* HoriAlignBox::From(int index)
{
    switch (index)
    {
    case 0:
        return HoriAlignBox::LeftBox;
    case 1:
        return HoriAlignBox::RightBox;
    case 2:
        return HoriAlignBox::CenterBox;
    case 3:
        return HoriAlignBox::StretchBox;
    default:
        return NULL;
    }
}

Integer* VertAlignBox::TopBox = Integer::GetPosInt(0);
Integer* VertAlignBox::BottomBox = Integer::GetPosInt(1);
Integer* VertAlignBox::CenterBox = Integer::GetPosInt(2);
Integer* VertAlignBox::StretchBox = Integer::GetPosInt(3);

Integer* VertAlignBox::From(int index)
{
    switch (index)
    {
    case 0:
        return VertAlignBox::TopBox;
    case 1:
        return VertAlignBox::BottomBox;
    case 2:
        return VertAlignBox::CenterBox;
    case 3:
        return VertAlignBox::StretchBox;
    default:
        return NULL;
    }
}

Integer* TileModeBox::NoneBox = Integer::GetPosInt(0);
Integer* TileModeBox::FlipXBox = Integer::GetPosInt(1);
Integer* TileModeBox::FlipYBox = Integer::GetPosInt(2);
Integer* TileModeBox::FlipXYBox = Integer::GetPosInt(3);
Integer* TileModeBox::TileBox = Integer::GetPosInt(4);

Integer* TileModeBox::From(int index)
{
    switch (index)
    {
    case 0:
        return TileModeBox::NoneBox;
    case 1:
        return TileModeBox::FlipXBox;
    case 2:
        return TileModeBox::FlipYBox;
    case 3:
        return TileModeBox::FlipXYBox;
    case 4:
        return TileModeBox::TileBox;
    default:
        return NULL;
    }
}

Integer* AlignmentXBox::LeftBox = Integer::GetPosInt(0);
Integer* AlignmentXBox::CenterBox = Integer::GetPosInt(1);
Integer* AlignmentXBox::RightBox = Integer::GetPosInt(2);

Integer* AlignmentXBox::From(int index)
{
    switch (index)
    {
    case 0:
        return AlignmentXBox::LeftBox;
    case 1:
        return AlignmentXBox::CenterBox;
    case 2:
        return AlignmentXBox::RightBox;
    default:
        return NULL;
    }
}

Integer* AlignmentYBox::TopBox = Integer::GetPosInt(0);
Integer* AlignmentYBox::CenterBox = Integer::GetPosInt(1);
Integer* AlignmentYBox::BottomBox = Integer::GetPosInt(2);

Integer* AlignmentYBox::From(int index)
{
    switch (index)
    {
    case 0:
        return AlignmentYBox::TopBox;
    case 1:
        return AlignmentYBox::CenterBox;
    case 2:
        return AlignmentYBox::BottomBox;
    default:
        return NULL;
    }
}

Integer* StretchBox::NoneBox = Integer::GetPosInt(0);
Integer* StretchBox::FillBox = Integer::GetPosInt(1);
Integer* StretchBox::UniformBox = Integer::GetPosInt(2);
Integer* StretchBox::UniformToFillBox = Integer::GetPosInt(3);

Integer* StretchBox::From(int index)
{
    switch (index)
    {
    case 0:
        return StretchBox::NoneBox;
    case 1:
        return StretchBox::FillBox;
    case 2:
        return StretchBox::UniformBox;
    case 3:
        return StretchBox::UniformToFillBox;
    default:
        return NULL;
    }
}

Integer* StretchDirectionBox::UpOnlyBox = Integer::GetPosInt(0);
Integer* StretchDirectionBox::DownOnlyBox = Integer::GetPosInt(1);
Integer* StretchDirectionBox::BothBox = Integer::GetPosInt(2);

Integer* StretchDirectionBox::From(int index)
{
    switch (index)
    {
    case 0:
        return StretchDirectionBox::UpOnlyBox;
    case 1:
        return StretchDirectionBox::DownOnlyBox;
    case 2:
        return StretchDirectionBox::BothBox;
    default:
        return NULL;
    }
}

Integer* HitTestFilterAction::From(int index)
{
    switch (index)
    {
    case HitTestFilterAction::Continue:
        return HitTestFilterAction::ContinueBox;

    case HitTestFilterAction::Stop:
        return HitTestFilterAction::StopBox;

    case HitTestFilterAction::ContinueSkipSelf:
        return HitTestFilterAction::ContinueSkipSelfBox;

    case HitTestFilterAction::ContinueSkipChildren:
        return HitTestFilterAction::ContinueSkipChildrenBox;

    case HitTestFilterAction::ContinueSkipSelfAndChildren:
        return HitTestFilterAction::ContinueSkipSelfAndChildrenBox;

    default:
        return NULL;
    }
}

Integer* HitTestFilterAction::ContinueBox = Integer::GetPosInt(HitTestFilterAction::Continue);
Integer* HitTestFilterAction::StopBox = Integer::GetPosInt(HitTestFilterAction::Stop);
Integer* HitTestFilterAction::ContinueSkipSelfBox = Integer::GetPosInt(HitTestFilterAction::ContinueSkipSelf);
Integer* HitTestFilterAction::ContinueSkipChildrenBox = Integer::GetPosInt(HitTestFilterAction::ContinueSkipChildren);
Integer* HitTestFilterAction::ContinueSkipSelfAndChildrenBox = Integer::GetPosInt(HitTestFilterAction::ContinueSkipSelfAndChildren);

//====================================================
//
void LogObjAlloc(Object* obj)
{
    //allocobj_count++;
    //Debug::Trace(_U("[%s]Alloc count: %d - %d\n")
    //    ,obj->GetTypeName().c_str(), allocobj_count, allocobj_count - freeobj_count);
}

void LogObjFree(Object* obj)
{
    //freeobj_count++;
    //Debug::Trace(_U("[%s]Free count: %d - %d\n")
    //    ,obj->GetTypeName().c_str(), freeobj_count, allocobj_count - freeobj_count);
}

void* AllocMem(int size)
{
    return malloc(size);
}

void FreeMem(void* _Ptr)
{
    free(_Ptr);
}

void MemZero(void* ptr, int size)
{
    CSSE::MemSetDWord(0, ptr, size);
    //memset(ptr, 0, size);
}

void MemMove(void* dest, void* src, int size)
{
    memmove(dest, src, size);
}

void MemCopy(void* dest, void* src, int size)
{
    memcpy(dest, src, size);
    //CSSE::MemCopy(dest, src, size);
}

//========================================================

const MemberInfo* Object::GetMemberInfo()
{
    return NULL;
}

ImplementRTTIOfClass(Handler, Object)

//========================================================

bool RTTIOfInfo::Equals(RTTIOfInfo* Other) 
{ 
    return this == Other && classIndex == Other->classIndex; 
}

UnrefObj::UnrefObj()
{
    sealAutoDelete();
}

UnrefObj::~UnrefObj()
{

}

Object::Object()
    : _holder(NULL)
    , _count(0)
    , _flag(1)
{
    LogObjAlloc(this);
}

Object::Object(bool autodel)
    : _holder(NULL)
    , _count(0)
    , _flag(0)
{
    setAutoDelete(autodel);
    if (!autodel)
    {
        _flag |= 2;
    }
}

Object::~Object() 
{
    LogObjFree(this);
    if (_holder != NULL)
    {
        _holder->target = NULL;
        _holder = NULL;
    }
}

static suic::RTTIOfInfo* __stdcall BaseRTTIType()
{
    return NULL;
}

RTTIOfInfo Object::typeObject = {0, _U("Object"), BaseRTTIType, &Object::Create};

//String __stdcall Object::RTTIName()
//{
//    return RTTIType()->typeName;
//}
//
RTTIOfInfo* __stdcall Object::RTTIType()
{
    return &typeObject;
}

/*bool Object::IsShowObj()
{
    return ((_flag & 8) == 0);
}

void Object::ShowObj(bool bShow)
{
    if (bShow)
    {
        _flag &= ~8;
    }
    else
    {
        _flag |= 8;
    }
}*/

bool Object::InheritFrom(Object* other)
{
    return GetRTTIType()->InheritFrom(other->GetRTTIType());
}

//bool Object::EqualType(Object* other)
//{
//    return (GetRTTIType() == other->GetRTTIType());
//}

RTTIOfInfo* Object::GetRTTIType() const
{ 
    return &typeObject; 
}

Object* __stdcall Object::Create()
{
    return NULL;//new Object();
}

void Object::StaticInit()
{
}

const suic::MemberInfo* __stdcall Object::ClassMemberInfo()
{
    return NULL;
}

//String Object::GetTypeName() const
//{ 
//    return GetRTTIType()->typeName; 
//}

void Object::Dispose()
{

}

static int new_count = 0;
static int del_count = 0;

//#ifdef _DEBUG
//void* Object::operator new(size_t size, const char* name, int line)
//{
//    return ::operator new(size);
//}
//void* operator new(size_t size, void* ptr, const char* name, int line)
//{
//    return ptr;
//}
//#else

void LogAlloc()
{
    //alloc_count++;
    //Debug::Trace(_U("Alloc count: %d - %d\n"), alloc_count, alloc_count - free_count);
}

void LogFree()
{
    //free_count++;
    //Debug::Trace(_U("Free count: %d - %d\n"), free_count, alloc_count - free_count);
}

//#endif

//#ifdef _DEBUG
//void Object::operator delete(void* ptr, const char* name, int line)
//{
//    ::operator delete(ptr);
//}
//#endif

void Object::operator delete(void* ptr)
{
    LogFree();
    //suiFree(ptr);
    ::operator delete(ptr);
}

void Object::operator delete(void *, void *)
{
    LogFree();
}

void* Object::operator new[](size_t size)
{
    LogAlloc();
    //return suiMalloc(size);
    return ::operator new[](size);
    return malloc(size);
}

void Object::operator delete[](void* ptr)
{
    LogFree();
    //suiFree(ptr);return;
    ::operator delete[](ptr);
}

#ifdef _DEBUG
void* Object::operator new(size_t size, const char* filename, int line)
{
    LogAlloc();
    //return suiMalloc(size);
    return ::operator new(size);
}

void Object::operator delete(void* p, const char* filename, int line)
{
    LogFree();
    //suiFree(p);
    //return;
    ::operator delete(p);
}
#endif
void* Object::operator new(size_t size, void* ptr)
{
    LogAlloc();
    return ptr;
}
void * Object::operator new(size_t size)
{
    LogAlloc();
    //return suiMalloc(size);
    return ::operator new(size);
}

Object* Object::Clone()
{
    return NULL;
}

String Object::ToString()
{
    return String();
}

bool Object::ToBool() const
{
    return false;
}

Float Object::ToFloat()
{
    return 0;
}

double Object::ToDouble()
{
    return ToFloat();
}

Int32 Object::ToInt()
{
    return 0;
}

//Rect Object::ToRect()
//{
//    return Rect();
//}

//fRect Object::TofRect()
//{
//    return fRect();
//}

//Size Object::ToSize()
//{
//    return Size();
//}

//fSize Object::TofSize()
//{
//    return fSize();
//}

//Point Object::ToPoint()
//{
//    return Point();
//}

//Color Object::ToColor()
//{
//    return Color(0x01000000);
//}

int Object::GetHashCode()
{
    return static_cast<int>((DWORD_PTR)this);
}

bool Object::Equals(Object* Other)
{
    if (this == Other)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool RTTIOfInfo::InheritFrom(RTTIOfInfo* baseType)
{
    if (baseType == NULL)
    {
        return false;
    }

    RTTIOfInfo* pTypeThis = this;

    while (pTypeThis != NULL)
    {
        if (pTypeThis == baseType)
        {
            return true;
        }

        pTypeThis = (*pTypeThis->baseType)();
    }

    return false;
}

bool RTTIOfInfo::IsInstanceOfType(Object* Other)
{
    return InheritFrom(Other->GetRTTIType());
}

int Object::getRefCount()
{
    return _count;
    //return InterlockedExchangeAdd(const_cast<LONG*>((LONG*)(&_count)), 0);
}

bool Object::isAutoDelete()
{
    return ((_flag & 1) == 1);
}

bool Object::setAutoDelete(bool bDel)
{
    bool bAutoDel = isAutoDelete();
    if ((_flag & 2) == 0)
    {
        if (bDel)
        {
            _flag |= 1;
        }
        else
        {
            _flag &= ~1;
        }
    }
    return bAutoDel;
}

void Object::sealAutoDelete()
{
    _flag |= 2;
}

void Object::ref()
{
    ++_count;
    //::InterlockedIncrement((LONG*)&_count);
}

void Object::unref()
{
    --_count;
    //InterlockedDecrement((LONG*)&_count);
    if (_count == 0)
    {
        if (isAutoDelete())
        {
            Dispose();
            if (_holder != NULL)
            {
                _holder->target = NULL;
                _holder = NULL;
            }
            delete this;
        }
    }
}

/*void Object::ClearOTag()
{
    _flag &= ~(0xFE);
}

bool Object::TestOTag(Byte flag) const
{
    return (flag == (flag & _flag));
}

void Object::SetOTag(Byte flag, bool add)
{
    if (add)
    {
        _flag |= (flag & 0xFE);
    }
    else
    {
        _flag &= ~(flag & 0xFE);
    }
}*/

}
