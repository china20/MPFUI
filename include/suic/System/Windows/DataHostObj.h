// 华勤科技版权所有 2010-2011
// 
// 文件名：DataHostObj.h
// 功  能：定义核心库的拖动对象包装对象实现。
// 
// 作  者：Sharpui开发组
// 时  间：2010-07-02
// 
// ============================================================================

#ifndef _UIDATAHOSTOBJ_H_
#define _UIDATAHOSTOBJ_H_

#include <System/Windows/Object.h>

class SUICORE_API DataHostObj : public suic::Object
{
public:

    DataHostObj(IDataObject* obj)
    {
        _dataObj = obj;
    }

    IDataObject* GetDataObject() const
    {
        return _dataObj;
    }

private:

    IDataObject* _dataObj;
};

#endif
