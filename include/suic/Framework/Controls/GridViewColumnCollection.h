// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����GridViewColumnCollection.h
// ��  �ܣ��б���ͼ�м��ϡ�
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2010-11-02
// 
// ============================================================================

#ifndef _UIGRIDVIEWCOLUMNCOLLECTION_H_
#define _UIGRIDVIEWCOLUMNCOLLECTION_H_

#include <System/Tools/Collection.h>
#include <Framework/Controls/ScrollViewer.h>
#include <Framework/Controls/GridViewColumn.h>

namespace suic
{

class SUICORE_API GridViewColumnCollection : public AutoColl<GridViewColumn>
                                           , public Object
{
public:

    RTTIOfClass(GridViewColumnCollection)

    static bool StaticInit();

    GridViewColumnCollection();

    Element* GetOwner() const;
    void SetOwner(Element* owner);

private:

    Element* _owner;
};

}

#endif
