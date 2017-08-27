/////////////////////////////////////////////////////////////////////////
// GridViewColumnCollection.cpp


#include <Framework/Controls/GridViewColumnCollection.h>

namespace suic
{

ImplementRTTIOfClass(GridViewColumnCollection, Object)

bool GridViewColumnCollection::StaticInit()
{
    return true;
}

//----------------------------------------------------

GridViewColumnCollection::GridViewColumnCollection()
{
    _owner = NULL;
}

Element* GridViewColumnCollection::GetOwner() const
{
    return _owner;
}

void GridViewColumnCollection::SetOwner(Element* owner)
{
    _owner = owner;
}

}
