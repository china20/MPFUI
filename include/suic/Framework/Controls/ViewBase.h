// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����ViewBase.h
// ��  �ܣ��б���ͼ���ࡣ
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2010-11-02
// 
// ============================================================================

#ifndef _UIVIEWBASE_H_
#define _UIVIEWBASE_H_

#include <Framework/Controls/Control.h>

namespace suic
{

class SUICORE_API ViewBase : public DpObject
{
public:

    RTTIOfClass(ViewBase)

    static bool StaticInit();

    ViewBase();
    ~ViewBase();

public:

    virtual void LinkViewOwner(DpObject* owner);
    virtual void UnlinkViewOwner(DpObject* owner);
    virtual void PrepareItem(DpObject* item);
};

}

#endif
