// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����SourceRef.h
// ��  �ܣ�������Ŀ�İ�Source��
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2011-05-02
// 
// ============================================================================

#ifndef _UISOURCEREF_H_
#define _UISOURCEREF_H_

#include <System/Windows/Extension.h>
#include <System/Windows/EventHandler.h>

namespace suic
{

class Element;
class FrameworkElement;

enum RelativeSourceMode
{
    Self,
    PreviousData,
    TemplatedParent,
    FindAncestor,
};

class SUICORE_API SourceRef
{
public:

    virtual ~SourceRef() {}

    virtual Object* GetSource(DpObject* target, DpProperty* targetDp, bool bFirst) = 0;
};

struct SUICORE_API ObjectSource : public SourceRef
{
    String ObjName;

    Object* GetSource(DpObject* target, DpProperty* targetDp, bool bFirst);
};

struct SUICORE_API ElementNameSource : public SourceRef
{
    String Name;

    Object* GetSource(DpObject* target, DpProperty* targetDp, bool bFirst);
};

struct SUICORE_API RelativeSource : public SourceRef
{
    int AncestorLevel;
    RTTIOfInfo* AncestorType;
    RelativeSourceMode Mode;

    RelativeSource()
    {
        Mode = RelativeSourceMode::Self;
        AncestorType = NULL;
        AncestorLevel = 1;
    }

    RelativeSource(RelativeSourceMode val)
    {
        Mode = val;
        AncestorType = NULL;
        AncestorLevel = 1;
    }

    Object* GetSource(DpObject* target, DpProperty* targetDp, bool bFirst);
};

}

#endif
