// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����DataHostObj.h
// ��  �ܣ�������Ŀ���϶������װ����ʵ�֡�
// 
// ��  �ߣ�Sharpui������
// ʱ  �䣺2010-07-02
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
