// 华勤科技版权所有 2010-2011
// 
// 文件名：uidres.h
// 功  能：设计器资源导出接口定义。
// 
// 作  者：汪荣
// 时  间：2012-07-02
// 
// ============================================================================

#ifndef _UIDRES_H_
#define _UIDRES_H_

#include <uidcore/uidprop.h>

class IUIDRes : public suic::Object
{
public:

    virtual ~IUIDRes() {}

    virtual Element* GetOwner() = 0;

    virtual int ReadRes(const suic::String& path) = 0;
    virtual int SaveAsRource(const suic::String& path) = 0;

    virtual int ParseRes(const IXamlNode* node) = 0;
};

typedef suic::shared<IUIDRes> UIDResPtr;

extern "C" void UIDGetRes(suic::Element* owner, UIDResPtr& frame);

#endif
