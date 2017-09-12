// 华勤科技版权所有 2008-2022
// 
// 文件名：GridViewColumnCollection.h
// 功  能：列表视图列集合。
// 
// 作  者：MPF开发组
// 时  间：2010-11-02
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

    static void StaticInit();

    GridViewColumnCollection();

    Element* GetOwner() const;
    void SetOwner(Element* owner);

private:

    Element* _owner;
};

}

#endif
