// 华勤科技版权所有 2008-2022
// 
// 文件名：uieventarg.h
// 功  能：定义核心库的事件基类。
// 
// 作  者：MPF开发组
// 时  间：2010-07-02
// 
// ============================================================================

#ifndef _UIEVENTARG_H_
#define _UIEVENTARG_H_

#include <System/Windows/Object.h>
#include <System/Windows/Delegate.h>

namespace suic
{

/// <summary>
/// 界面元素的事件，这是所有界面元素的基类，其它的界面元素都会直接或
/// 或间接地派生它
/// </summary>
class SUICORE_API EventArg
{
public:

    static EventArg Empty;

    EventArg();
    Uint32 Timestamp();

protected:

    Uint32 _timestamp;
};

class SUICORE_API AccessKeyEventArg : public EventArg
{
public:

    AccessKeyEventArg(Uint16 modifier, const String& key, bool isMultiple);

    String GetKey() const;
    bool IsMultiple() const;
    Uint16 GetModifier() const;

private:

    String _key;
    Uint16 _modifier;
    bool _isMultiple;
};

};

#endif
