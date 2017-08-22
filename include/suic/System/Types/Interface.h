// 华勤科技版权所有 2010-2011
// 
// 文件名：Interface.h
// 功  能：定义核心库的用户注册接口。
// 
// 作  者：MPF开发组
// 时  间：2010-07-02
// 
// ============================================================================

#ifndef _UIINTERFACE_H_
#define _UIINTERFACE_H_

#include <System/Types/XamlNode.h>
#include <System/Windows/Element.h>

namespace suic 
{

class NOVTABLE IReadNodeCb
{
public:

    virtual ~IReadNodeCb() {}

    virtual void CreateElement(IXamlNode* node, ObjectPtr& elem) = 0;
    virtual ResourceItem ConvertExpression(RTTIOfInfo* d, DpProperty* dp, const String& expr) = 0;
};

/// <summary>
/// 系统日志级别定义
/// </summary>
class LogLevel 
{
public:

    enum {None, Error, Warn, Info, Trace};
};

}

#endif
