// 华勤科技版权所有 2008-2022
// 
// 文件名：UIDictionary.h
// 功  能：定义核心库的键/值对象。
// 
// 作  者：MPF开发组
// 时  间：2010-07-02
// 
// ============================================================================

#ifndef _UIMICRODEF_H_
#define _UIMICRODEF_H_

//--------------------------------------------------------------------

#define __RTTIOfClass(This) \
    static suic::RTTIOfInfo type##This;\
    static suic::RTTIOfInfo* __stdcall RTTIType() {return (suic::RTTIOfInfo*)&(This::type##This);}\
    static const suic::Char* __stdcall RTTIName() {return (type##This).typeName;}\
    virtual suic::RTTIOfInfo* GetRTTIType() const {return (suic::RTTIOfInfo*)&(This::type##This);}\

#define __ImplementRTTIOfClass(This,Base,index) \
    suic::RTTIOfInfo This::type##This = {index,L#This,&Base::RTTIType,&This::Create,&This::StaticInit};\

#define __ImplementRTTIOfClassName(This,Base,Name,index) \
    suic::RTTIOfInfo This::type##This = {index,L#Name,&Base::RTTIType,&This::Create,&This::StaticInit};\

//--------------------------------------------------------------------

#define RTTIOfClass(This) \
    __RTTIOfClass(This)\
    static suic::Object* __stdcall Create() {return new This();}\

#define RTTIOfAbstractClass(This) \
    __RTTIOfClass(This)\
    static suic::Object* __stdcall Create() {return NULL;}\

#define ImplementRTTIOfClass(This,Base) __ImplementRTTIOfClass(This,Base,++suic::RTTIOfInfo::classCount)

#define ImplementRTTIOfClassName(This,Base,Name) __ImplementRTTIOfClassName(This,Base,Name,++suic::RTTIOfInfo::classCount)

// 支持XML配置的路由事件宏定义
#define IMPLEMENTEVENTHANDLER(N) ImplementRTTIOfClass(N##Handler, suic::Handler)\

//--------------------------------------------------------------------

#define INITORVALUE(n,v,t) if (t) { n |= v; } else {n &= ~(v);}

// 定义句柄转换
#define HANDLETOHWND(h) ((HWND)(LONG_PTR)h)
#define HWNDTOHANDLE(h) ((suic::Handle)(LONG_PTR)h)

#define HANDLETOHDC(dc) ((HDC)(ULONG_PTR)dc)
#define HDCTOHANDLE(dc) ((suic::Handle)(ULONG_PTR)dc)

#define HANDLETOBITMAP(bmp) ((HBITMAP)(ULONG_PTR)bmp)
#define BITMAPTOHANDLE(bmp) ((suic::Handle)(ULONG_PTR)bmp)

#define BOOLTOBOOLEAN(val) ((val) ? suic::Boolean::True : suic::Boolean::False)

#define CASTTOE(obj) (suic::RTTICast<suic::Element>(obj))
#define CASTTOC(obj) (suic::RTTICast<suic::Control>(obj))
#define CASTTOFE(obj) (suic::RTTICast<suic::FrameworkElement>(obj))

#define UITESTFLAG(o,v) ((o & v) == v)
#define UISETFLAG(o,v,b) b ? o |= v : o &= ~v;

#define ARGB(a,r,g,b)  ((COLORREF)(((BYTE)(b)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(r))<<16)|(((DWORD)(BYTE)(a))<<24)))

#define _U(x) L##x
#define NOVTABLE __declspec(novtable)

// 系统定时器定义
#define UI_SYSTIMERID  2
#define UI_SYSINTERVAL 30

#define SETREFOBJ(dest,src) if (dest != src)\
{\
    if (dest) dest->unref();\
    dest = src;\
    if (dest) dest->ref();\
}\

#define ADDREFOBJ(r) if (r) {r->ref(); }
#define FREEREFOBJ(r) if (r) {r->unref(); r = NULL;}

#define BeginRoutedEvent(theClass,baseClass) \
    suic::FnEventDic* GetEventInfo() \
    {\
        return theClass::ClassEventInfo();\
    }\
    suic::FnEventDic* __stdcall ClassEventInfo() \
    {\
        typedef theClass ThisClass;\
        typedef baseClass BaseClass;\
        static bool s_init = false;\
        static suic::FnEventDic _eventDic;\
        if (!s_init) { \
            s_init = true;\

#define MemberEvent(name, t, a) \
    _eventDic.Add(#name,\
    (suic::FnEventCall)(void(suic::Object::*)(t##*,a##*))(&ThisClass::##name));\

#define MemberRouted(name) \
    _eventDic.Add(#name,\
    (suic::FnEventCall)(void(suic::Object::*)(suic::DpObject*,suic::RoutedEventArg*))(&ThisClass::##name));\

#define MemberClickEvent(name) \
    _eventDic.Add(#name,\
    (suic::FnEventCall)(void(suic::Object::*)(suic::Element*,suic::RoutedEventArg*))(&ThisClass::##name));\

#define MemberMouseButton(name) \
    _eventDic.Add(#name,\
    (suic::FnEventCall)(void(suic::Object::*)(suic::Element*,suic::MouseButtonEventArg*))(&ThisClass::##name));\

#define MemberKeyboardFocus(name) \
    _eventDic.Add(#name,\
    (suic::FnEventCall)(void(suic::Object::*)(suic::Element*,suic::KeyboardFocusEventArg*))(&ThisClass::##name));\

#define EndRoutedEvent() \
        }\
        return &_eventDic;\
    }\

// 事件宏定义
#define BeginMember(theClass,baseClass) \
    const suic::MemberInfo* GetMemberInfo() \
    {\
        return theClass::ClassMemberInfo();\
    }\
    static const suic::MemberInfo* __stdcall ClassMemberInfo() \
    {\
        typedef theClass ThisClass;\
        typedef baseClass TheBaseClass;\
        static const suic::MemberEntry _funcEntries[] =  \
        { \

#define MemberType(name,T,DT) \
    { suic::UIDataType::##DT, #name, \
    (suic::FnMemberCall)(void (suic::Object::*)(T))(&ThisClass::##Set##name),\
    (suic::FnMemberCall)(T (suic::Object::*)(void))(&ThisClass::##Get##name) }, \

#define MemberGetType(name,T,DT) \
    { suic::UIDataType::##DT, #name, \
    NULL,\
    (suic::FnMemberCall)(T (suic::Object::*)(void))(&ThisClass::##Get##name) }, \

#define MemberSetType(name,T,DT) \
    { suic::UIDataType::##DT, #name, \
    (suic::FnMemberCall)(void (suic::Object::*)(T))(&ThisClass::##Set##name),\
    NULL }, \

#define MemberInt(name) MemberType(name,int,IntType)
#define MemberGetInt(name) MemberGetType(name,int,IntType)
#define MemberSetInt(name) MemberSetType(name,int,IntType)

#define MemberFloat(name) MemberType(name,suic::Float,FloatType)
#define MemberGetFloat(name) MemberGetType(name,suic::Float,FloatType)
#define MemberSetFloat(name) MemberSetType(name,suic::Float,FloatType)

#define MemberUint32Type(name) MemberType(name,suic::Uint32,Uint32Type)
#define MemberGetUint32Type(name) MemberGetType(name,suic::Uint32,Uint32Type)
#define MemberSetUint32Type(name) MemberSetType(name,suic::Uint32,Uint32Type)

#define MemberString(name) MemberType(name,suic::String,StringType)
#define MemberGetString(name) MemberGetType(name,suic::String,StringType)
#define MemberSetString(name) MemberSetType(name,suic::String,StringType)

#define MemberPoint(name) MemberType(name,suic::Point,PointType)
#define MemberGetPoint(name) MemberGetType(name,suic::Point,PointType)
#define MemberSetPoint(name) MemberSetType(name,suic::Point,PointType)

#define MemberRect(name) MemberType(name,suic::Rect,RectType)
#define MemberGetRect(name) MemberGetType(name,suic::Rect,RectType)
#define MemberSetRect(name) MemberSetType(name,suic::Rect,RectType)

#define MemberSize(name) MemberType(name,suic::Size,SizeType)
#define MemberGetSize(name) MemberGetType(name,suic::Size,SizeType)
#define MemberSetSize(name) MemberSetType(name,suic::Size,SizeType)

#define MemberBool(name) MemberType(name,bool,BoolType)
#define MemberGetBool(name) MemberGetType(name,bool,BoolType)
#define MemberSetBool(name) MemberSetType(name,bool,BoolType)

#define MemberObject(name) MemberType(name,suic::Object*,ObjectType)
#define MemberGetObject(name) MemberGetType(name,suic::Object*,ObjectType)
#define MemberSetObject(name) MemberSetType(name,suic::Object*,ObjectType)

#define MemberAttributes(name) \
    { suic::UIDataType::AttriType, "_MA", \
    (suic::FnMemberCall)(bool (suic::Object::*)(const suic::String&, suic::Object*))(&ThisClass::##Set##name),\
    (suic::FnMemberCall)(bool (suic::Object::*)(const suic::String&, suic::ObjectPtr&))(&ThisClass::##Get##name) }, \

#define MemberGetAttributes(name) \
    { suic::UIDataType::AttriType, "_MA", \
    NULL,\
    (suic::FnMemberCall)(bool (suic::Object::*)(const suic::String&, suic::ObjectPtr&))(&ThisClass::##Get##name) }, \

#define MemberSetAttributes(name) \
    { suic::UIDataType::AttriType, "_MA", \
    (suic::FnMemberCall)(bool (suic::Object::*)(const suic::String&, suic::Object*))(&ThisClass::##Set##name),\
    NULL }, \

#define EndMember() \
            {0,_U(""),(suic::FnMemberCall)0, (suic::FnMemberCall)0} \
        }; \
        static const suic::MemberInfo _funcCall = \
        { &TheBaseClass::ClassMemberInfo, &_funcEntries[0] }; \
        return &_funcCall;\
    }\

#define DeclareMember(name,T) \
    private: \
        T _m##name; \
    public: \
        T Get##name() { return _m##name; } \
        void Set##name(T val) { _m##name = val; }\

#define DeclareGetMember(name,T) \
    private: \
        T _m##name; \
    public: \
        T Get##name() { return _m##name; } \

#define DeclareSetMember(name,T) \
    private: \
        T _m##name; \
    public: \
        void Set##name(T val) { _m##name = val; }\

#define DefineMember(name,T) \
    private: \
        T _m##name; \
    public: \
        T Get##name() { return _m##name; } \
        void Set##name(T val) { _m##name = val; NotifyChanged(#name); }\

// 声明Bool属性
#define DefineBool(name) DefineMember(name,bool)
#define DefineGetBool(name) DeclareGetMember(name,bool)
#define DefineSetBool(name) DeclareSetMember(name,bool)

// 声明Float属性
#define DefineFloat(name) DefineMember(name,suic::Float)
#define DefineGetFloat(name) DeclareGetMember(name,suic::Float)
#define DefineSetFloat(name) DeclareSetMember(name,suic::Float)

// 声明Point属性
#define DefinePoint(name) DefineMember(name,suic::Point)
#define DefineGetPoint(name) DeclareGetMember(name,suic::Point)
#define DefineSetPoint(name) DeclareSetMember(name,suic::Point)

// 声明Size属性
#define DefineSize(name) DefineMember(name,suic::Size)
#define DefineGetSize(name) DeclareGetMember(name,suic::Size)
#define DefineSetSize(name) DeclareSetMember(name,suic::Size)

// 声明Rect属性
#define DefineRect(name) DefineMember(name,suic::Rect)
#define DefineGetRect(name) DeclareGetMember(name,suic::Rect)
#define DefineSetRect(name) DeclareSetMember(name,suic::Rect)

// 声明Int属性
#define DefineInt(name) DefineMember(name,int)
#define DefineGetInt(name) DeclareGetMember(name,int)
#define DefineSetInt(name) DeclareSetMember(name,int)

// 声明String属性
#define DefineString(name) DefineMember(name,suic::String)
#define DefineGetString(name) DeclareGetMember(name,suic::String)
#define DefineSetString(name) DeclareSetMember(name,suic::String)

// 声明Object属性
#define DefineObject(name) \
    private: \
        suic::ObjectPtr _m##name; \
    public: \
        suic::Object* Get##name() { return _m##name.get(); } \
        void Set##name(suic::Object* val) \
        { \
            _m##name = val; \
            NotifyChanged(#name); \
        }\

#define DefineGetObject(name) \
    private: \
        suic::ObjectPtr _m##name; \
    public: \
        suic::Object* Get##name() { return _m##name.get(); } \

#define DefineSetObject(name) \
    private: \
        suic::ObjectPtr _m##name; \
    public: \
        void Set##name(suic::Object* val) \
        { \
            _m##name = val; \
            NotifyChanged(#name); \
        }\


#endif
